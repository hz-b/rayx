#ifndef NO_VULKAN

#include "VulkanEngine/Run/Pipeline.h"

#include <filesystem>
#include <utility>

#include "CanonicalizePath.h"
#include "VulkanEngine/Buffer/BufferHandler.h"
#include "VulkanEngine/Init/Initializers.h"

namespace RAYX {

Pass::Pipeline::Pipeline(std::string name, VkDevice& dev, const ShaderStageCreateInfo& shaderCreateInfo) : m_name(std::move(name)), m_device(dev) {
    // TODO(OS): Don't reserve 1 for the descriptors
    shaderStage = std::make_shared<ShaderStage>(m_device, shaderCreateInfo);
}
Pass::Pipeline::~Pipeline() { cleanPipeline(m_device); };

void Pass::Pipeline::createPipelineLayout() {
    // if (!m_descriptorSetLayouts.empty()) {
    /*
    The pipeline layout allows the pipeline to access descriptor sets.
    So we just specify the descriptor set layout we created earlier.
    */
    // TODO(OS): Only one Set supported
    auto pipelineLayoutCreateInfo =
        VKINIT::Pipeline::pipeline_layout_create_info(&m_descriptorSetLayouts[0], static_cast<uint32_t>(m_descriptorSetLayouts.size()));

    /*
    Add push constants to the Pipeline
    */
    auto pushConstant = VKINIT::misc::push_constant_range(VK_SHADER_STAGE_COMPUTE_BIT, pushConstants.size,
                                                          0);  // Can change Offset if some of the struct is to be ignored

    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;  // One struct of pushConstants

    VK_CHECK_RESULT(vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout))
    //}
}

void Pass::Pipeline::createPipeline() {
    if (m_pipelineLayout != nullptr) {
        /*
        Now, we finally create the compute pipeline.
        */
        auto pipelineCreateInfo = VKINIT::Pipeline::compute_pipeline_create_info(m_pipelineLayout, shaderStage->getPipelineShaderCreateInfo());
        /*
        Now, we finally create the compute pipeline.
        */
        VK_CHECK_RESULT(vkCreateComputePipelines(m_device, m_pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_pipeline))
    }
}

/**
 * Read Pipeline Cache from stored data
 */
void inline Pass::Pipeline::readPipelineCache() {
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
void inline Pass::Pipeline::storePipelineCache(VkDevice& device) {
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
/**
 * @brief Vulkan cleanup, destroying all Vk-Pipeline related members. Sotres cache at the end.
 *
 * @param device
 */
void Pass::Pipeline::cleanPipeline(VkDevice& device) {
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
    storePipelineCache(device);
}

// -------------------------------------------------------------------------------------------------------

ComputePass::ComputePass(VkDevice& device, const ComputePassCreateInfo& createInfo) : m_Device(device), m_name(createInfo.passName) {
    m_stagesCount = createInfo.shaderStagesCreateInfos.size();
    m_pass.reserve(m_stagesCount);

    m_descriptorSetLayouts.reserve(createInfo.descriptorSetAmount);
    descriptorSets.reserve(createInfo.descriptorSetAmount);

    // Fill compute Piplines
    for (uint32_t i = 0; i < m_stagesCount; i++) {
        // TODO(OS): Add missing pushconstants
        m_pass[i] = std::make_shared<Pipeline>(createInfo.shaderStagesCreateInfos[i].name, m_Device, createInfo.shaderStagesCreateInfos[i]);
    }
}

ComputePass::~ComputePass() {
    for (const auto& pipeline : m_pass) {
        pipeline->cleanPipeline(m_Device);
    }

    for (auto layout : m_descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(m_Device, layout, nullptr);
    }
}

void ComputePass::createPipelines() {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "Creating pipelines...";

    // Todo: validtation layer warning : Consider adding VK_KHR_maintenance4  to support SPIR-V 1.6's localsizeid instead of
    // WorkgroupSize
    for (const auto& stage : m_pass) {
        stage->createPipelineLayout();
        stage->createPipeline();
    }
    RAYX_VERB << "Pass created...";
}
void ComputePass::createDescriptorSetLayout() {
    auto bindings = getDescriptorBindings();

    // TODO(OS): Only one Set supported
    auto descriptorSetLayoutCreateInfo = VKINIT::Descriptor::descriptor_set_layout_create_info(bindings);

    // Create the descriptor set layout.
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayouts[0]));
}

std::vector<VkDescriptorSetLayoutBinding> ComputePass::getDescriptorBindings() {
    RAYX_PROFILE_FUNCTION();
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(m_DescriptorBindings.size());
    for (const auto& [binding, b] : m_DescriptorBindings) {
        bindings.push_back({binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr});
    }
    return bindings;
}

void ComputePass::addPipelineStage(const ShaderStageCreateInfo& createInfo) {
    m_pass.push_back(std::make_shared<Pipeline>(createInfo.name, m_Device, createInfo));
    m_stagesCount++;
}

// TODO(OS): No copy cstr for PipelineStage
void ComputePass::addPipelineStage(const Pass& newStage) {
    m_pass.push_back(std::make_shared<Pipeline>(newStage));
    m_stagesCount++;
}

void ComputePass::createDescriptorPool() {
    // TODO(OS): This should change once we need more sets
    globalDescriptorPool = DescriptorPool::Builder(m_Device).setMaxSets(1).addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1).build();
}

}  // namespace RAYX
#endif