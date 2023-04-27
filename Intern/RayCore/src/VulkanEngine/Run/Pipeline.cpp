#ifndef NO_VULKAN

#include "VulkanEngine/Run/Pipeline.h"

#include <filesystem>
#include <utility>

#include "CanonicalizePath.h"
#include "VulkanEngine/Buffer/BufferHandler.h"

namespace RAYX {

ComputePipeline::PipelineStage::PipelineStage(std::string name, VkDevice& dev, const ShaderStageCreateInfo& shaderCreateInfo, int descriptorSetAmount)
    : m_name(std::move(name)), m_device(dev) {
    // TODO(OS): Don't reserve 1 for the descriptors
    shaderStage = std::make_shared<ShaderStage>(m_device, shaderCreateInfo);
    m_descriptorSetLayouts.reserve(descriptorSetAmount);
    descriptorSets.reserve(descriptorSetAmount);
}
Pipeline::PipelineStage::~PipelineStage() { cleanPipeline(m_device); };

void Pipeline::PipelineStage::createDescriptorSetLayout() {
    auto bindings = shaderStage->getDescriptorBindings();

    // TODO(OS): Only one Set supported
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();
    // Create the descriptor set layout.
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_device, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayouts[0]));
}

void Pipeline::PipelineStage::createPipelineLayout() {
    if (!m_descriptorSetLayouts.empty()) {
        /*
        The pipeline layout allows the pipeline to access descriptor sets.
        So we just specify the descriptor set layout we created earlier.
        */
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
        pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayouts[0];  // TODO(OS): Only one Set supported

        /*
        Add push constants to the Pipeline
        */
        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;  // Can change this if some of the struct is to be ignored
        pushConstant.size = pushConstants.size;
        pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;  // One struct of pushConstants

        VK_CHECK_RESULT(vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout))
    }
}

void Pipeline::PipelineStage::createPipeline() {
    if (m_pipelineLayout != nullptr) {
        /*
        Now, we finally create the compute pipeline.
        */
        VkComputePipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stage = shaderStage->getPipelineShaderCreateInfo();
        pipelineCreateInfo.layout = m_pipelineLayout;
        /*
        Now, we finally create the compute pipeline.
        */
        VK_CHECK_RESULT(vkCreateComputePipelines(m_device, m_pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_pipeline))
    }
}
/**
 * Read Pipeline Cache from stored data
 */
void inline Pipeline::PipelineStage::readPipelineCache() {
    /* Try to read pipeline cache file if exists */
    std::vector<uint8_t> pipeline_data;

    // Cache is stored in OS TEMP
    auto tmpDir = std::filesystem::temp_directory_path();
    try {
        pipeline_data = readFile((tmpDir / "pipeline_cache.data").string());
    } catch (std::runtime_error& ex) {
        RAYX_LOG << "No pipeline cache found.";
    }

    /* Add initial pipeline cache data from the cached file */
    VkPipelineCacheCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    create_info.initialDataSize = pipeline_data.size();
    create_info.pInitialData = pipeline_data.data();

    /* Create Vulkan pipeline cache */
    VK_CHECK_RESULT(vkCreatePipelineCache(m_device, &create_info, nullptr, &m_pipelineCache));
}

/**
 * @brief Store and destroy Cache
 *
 * @param cache Cache Object
 * @param device Vulkan Logical device
 */
void inline Pipeline::PipelineStage::storePipelineCache(VkDevice& device) {
    if (m_pipelineCache != VK_NULL_HANDLE) {
        /* Get size of pipeline cache */
        size_t size{};
        VK_CHECK_RESULT(vkGetPipelineCacheData(device, m_pipelineCache, &size, nullptr));

        /* Get data of pipeline cache */
        std::vector<uint8_t> data(size);
        VK_CHECK_RESULT(vkGetPipelineCacheData(device, m_pipelineCache, &size, data.data()));

        // Cache is stored in OS TEMP
        auto tmpDir = std::filesystem::temp_directory_path();
        try {
            /* Write pipeline cache data to a file in binary format */
            writeFile(data, (tmpDir / "pipeline_cache.data").string());
        } catch (std::runtime_error& ex) {
            RAYX_WARN << "No pipeline cache written.";
        }

        /* Destroy Vulkan pipeline cache */
        vkDestroyPipelineCache(device, m_pipelineCache, nullptr);
    }
}

// -------------------------------------------------------------------------------------------------------

ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo& createInfo) : m_name(createInfo.passName) {
    m_stagesCount = createInfo.shaderStagesCreateInfos.size();
    m_pass.reserve(m_stagesCount);

    // Fill compute stages
    for (uint32_t i = 0; i < m_stagesCount; i++) {
        // TODO(OS): Add missing pushconstants
        m_pass[i] = std::make_shared<PipelineStage>(createInfo.shaderStagesCreateInfos[i].name, m_Device, createInfo.shaderStagesCreateInfos[i]);
    }
}

ComputePipeline::~ComputePipeline() {
    for (const auto& pipeline : m_pass) {
        vkDestroyPipeline(m_Device, pipeline->m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, pipeline->m_pipelineLayout, nullptr);
    }
}

void ComputePipeline::createPipelines() {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "Creating pipelines...";

    // Todo: validtation layer warning : Consider adding VK_KHR_maintenance4  to support SPIR-V 1.6's localsizeid instead of WorkgroupSize
    for (const auto& stage : m_pass) {
        stage->createPipelineLayout();
        stage->createPipeline();
    }
    RAYX_VERB << "Pass created...";
}

void ComputePipeline::createDescriptorSetLayout(int stage) { m_pass[stage]->createDescriptorSetLayout(); }

void ComputePipeline::createDescriptorSetLayouts() {
    for (size_t i = 0; i < m_pass.size(); i++) {
        createDescriptorSetLayout(static_cast<int>(i));
    }
}

void ComputePipeline::addPipelineStage(const ShaderStageCreateInfo& createInfo) {
    m_pass.push_back(std::make_shared<PipelineStage>(createInfo.name, m_Device, createInfo));
    m_stagesCount++;
}

// TODO(OS): No copy cstr for PipelineStage
void ComputePipeline::addPipelineStage(const PipelineStage& newStage) {
    m_pass.push_back(std::make_shared<PipelineStage>(newStage));
    m_stagesCount++;
}

void ComputePipeline::createDescriptorPool() {
    // TODO(OS): This should change once we need more sets
    globalPool = DescriptorPool::Builder(m_Device).setMaxSets(1).addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1).build();
}

}  // namespace RAYX
#endif