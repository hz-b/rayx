#ifndef NO_VULKAN

#include "VulkanEngine/Run/Pass.h"

#include <filesystem>
#include <utility>

#include "CanonicalizePath.h"
#include "VulkanEngine/Init/Initializers.h"

namespace RAYX {

Pass::Pipeline::Pipeline(VkDevice& dev, const PipelineCreateInfo& shaderCreateInfo) : m_name(std::move(shaderCreateInfo.name)), m_device(dev) {
    m_shaderPath = shaderCreateInfo.shaderPath;
    m_shaderType = shaderCreateInfo.shaderType;
    createShaderModule();
    isCleaned = false;
    RAYX_D_LOG << "Pipeline " << m_name << " created";
}

Pass::Pipeline::~Pipeline() {
    cleanPipeline(m_device);
    vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
    // storePipelineCache(m_device); // TODO(OS): Restore pipeline cache (But for multiple pipelines.)
}

void Pass::Pipeline::createPipelineLayout(VkDescriptorSetLayout* setLayouts) {
    /*
    The pipeline layout allows the pipeline to access descriptor sets.
    So we just specify the descriptor set layout we created earlier.
    */
    // TODO(OS): Only one Set supported
    auto pipelineLayoutCreateInfo = VKINIT::Pipeline::pipeline_layout_create_info(setLayouts);

    /*
    Add push constants to the Pipeline
    */
    auto pushConstant = m_pushConstant.getVkPushConstantRange(m_shaderType);
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;  // One struct of pushConstants

    checkVkResult(vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
}

void Pass::Pipeline::createPipeline() {
    /*
    Now, we finally create the compute pipeline.
    */
    auto pipelineCreateInfo = VKINIT::Pipeline::compute_pipeline_create_info(m_pipelineLayout, getPipelineShaderCreateInfo());
    // FIXME(OS): Currently only creates compute pipelines. Pipeline should be general for also other types (E.g Graphics). Move this somewhere else!
    checkVkResult(vkCreateComputePipelines(m_device, nullptr, 1, &pipelineCreateInfo, nullptr, &m_pipeline));
    RAYX_D_LOG << m_name << "VkPipeline created";
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
    checkVkResult(vkCreatePipelineCache(m_device, &create_info, nullptr, &m_pipelineCache));
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
        checkVkResult(vkGetPipelineCacheData(device, m_pipelineCache, &size, nullptr));

        /* Get data of pipeline cache */
        std::vector<uint8_t> data(size);
        checkVkResult(vkGetPipelineCacheData(device, m_pipelineCache, &size, data.data()));

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
 * @brief Vulkan cleanup, destroying all VkPipeline related members. Stores cache at the end (TODO: Cache broken).
 *
 * @param device
 */
void Pass::Pipeline::cleanPipeline(VkDevice& device) {
    if (isCleaned) {
        return;
    }

    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
    isCleaned = true;
    RAYX_D_LOG << m_name << " cleaned.";
}

/*
Create a shader module. A shader module basically just encapsulates some shader code.
*/
void Pass::Pipeline::createShaderModule() {
    RAYX_PROFILE_FUNCTION_STDOUT();
    uint32_t filelength;
    // the code  was created by running the command: glslangValidator.exe -V <shaderFile>.comp
    std::string path = canonicalizeRepositoryPath(m_shaderPath).string();
    uint32_t* shaderCode = readFile(filelength, path.c_str());
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = shaderCode;
    createInfo.codeSize = filelength;

    checkVkResult(vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule));
    RAYX_VERB << "Shader module " << m_name << " created.";
    delete[] shaderCode;
}

VkPipelineShaderStageCreateInfo Pass::Pipeline::getPipelineShaderCreateInfo() {
    /* we specify the compute shader stage, and it's entry point(main) */
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = m_shaderType;
    shaderStageCreateInfo.module = m_shaderModule;
    shaderStageCreateInfo.pName = m_shaderEntryPoint.c_str();
    return shaderStageCreateInfo;
}

void Pass::Pipeline::updatePushConstant(void* data, size_t size) { m_pushConstant.update(data, size); }

// -------------------------------------------------------------------------------------------------------

void Pass::updatePushConstant(int stage, void* data, uint32_t size) { m_pass[stage]->updatePushConstant(data, size); }

// -------------------------------------------------------------------------------------------------------

ComputePass::ComputePass(VkDevice& device, const ComputePassCreateInfo& createInfo) : m_Device(device) {
    m_name = std::string(createInfo.passName);
    m_stagesCount = createInfo.pipelineCreateInfos.size();
    m_pass.reserve(m_stagesCount);
    m_descriptorSets.resize(createInfo.descriptorSetAmount);

    // Fill compute Pipelines
    for (uint32_t i = 0; i < m_stagesCount; i++) {
        m_pass.push_back(std::make_unique<Pipeline>(m_Device, createInfo.pipelineCreateInfos[i]));
    }
}

ComputePass::~ComputePass() {
    for (auto& layout : m_descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(m_Device, layout, nullptr);
    }
}

void ComputePass::createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    // TODO(OS): Only one Set supported
    auto descriptorSetLayoutCreateInfo = VKINIT::Descriptor::descriptor_set_layout_create_info(bindings);

    // Create the descriptor set layout.
    VkDescriptorSetLayout descriptorSetLayout;
    checkVkResult(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
    m_descriptorSetLayouts.push_back(descriptorSetLayout);
}

// Prepare Compute Pass according to buffers and layout
void ComputePass::prepare(std::vector<VkDescriptorSetLayoutBinding> bindings) {
    RAYX_D_LOG << "Preparing pipelines...";

    createDescriptorSetLayout(bindings);

    // if (m_globalDescriptorPool != nullptr) { // TODO(OS): Make Create this only if binding size changes or pool doesn't even exist!
    createDescriptorPool(1, bindings.size());
    //}

    for (const auto& stage : m_pass) {
        stage->createPipelineLayout(&m_descriptorSetLayouts[0]);
        stage->createPipeline();
    }
}

void ComputePass::addPipelineStage(const PipelineCreateInfo& createInfo) {
    m_pass.push_back(std::make_unique<Pipeline>(m_Device, createInfo));
    m_stagesCount++;
}

void ComputePass::createDescriptorPool(uint32_t maxSets, uint32_t bufferCount) {
    // TODO(OS): This should change once we need more sets
    m_globalDescriptorPool =
        DescriptorPool::Builder(m_Device).setMaxSets(maxSets).addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, bufferCount).build();
    m_globalDescriptorPool->allocateDescriptor(m_descriptorSetLayouts[0], m_descriptorSets[0]);
    RAYX_D_LOG << "Global pool created ,max sets = " << maxSets << " buffer count = " << bufferCount;
}

void ComputePass::updateDescriptorSets(BufferHandler* bufferHandler) {
    auto writer = DescriptorWriter(m_descriptorSetLayouts[0], *m_globalDescriptorPool);
    auto buffers = bufferHandler->getBuffers();

    for (auto& [name, b] : *buffers) {
        if (!b->hasPassDescriptorBinding(m_name)) {
            continue;
        }
        auto descInfo = b->getVkDescriptorBufferInfo();
        writer.writeBuffer(b->getPassDescriptorBinding(m_name), &descInfo);
    }

    writer.build(m_descriptorSets[0]);
}

// TODO(OS): To be replaced by a more sophisticated Desciptor manager(Hence now, it's only ´simple´)
void ComputePass::simpleUpdateDescriptorSets(BufferHandler* bufferHandler) {
    auto buffers = bufferHandler->getBuffers();
    std::vector<VkWriteDescriptorSet> writes;

    for (auto& [name, b] : *buffers) {
        if (!b->hasPassDescriptorBinding(m_name)) {
            continue;  // Buffer offers no binding to targeted pass
        }
        // Specify which buffer to use: input buffer
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = b->getBuffer();
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = b->getSize();

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = m_descriptorSets[0];  // write to this descriptor set.
        writeDescriptorSet.dstBinding = b->getPassDescriptorBinding(m_name);
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;                                 // update a single descriptor.
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // storage buffer.
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        writes.push_back(writeDescriptorSet);

        // Perform the update of the descriptor set.
        vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
    }
}

/**
 * @brief Bind push constants
 *
 * @param cmdBuffer
 * @param stage Index of stage/Pipeline
 */
void ComputePass::bindDescriptorSet(const VkCommandBuffer& cmdBuffer, int stage) {
    vkCmdBindDescriptorSets(cmdBuffer, getPipelineBindPoint(), getVkPipelineLayout(stage), 0, 1, &m_descriptorSets[0], 0, nullptr);
}

/**
 *
 * @brief Bind push constants
 *
 * @param cmdBuffer
 * @param stage Index of stage/Pipeline
 */
void ComputePass::cmdPushConstants(const VkCommandBuffer& cmdBuffer, int stage) {
    vkCmdPushConstants(cmdBuffer, getVkPipelineLayout(stage), m_pass[stage]->getShaderStageFlagBits(), 0, m_pass[stage]->m_pushConstant.getSize(),
                       m_pass[stage]->m_pushConstant.getData());
}

void ComputePass::cleanPipeline(int stage) { m_pass[stage]->cleanPipeline(m_Device); }

}  // namespace RAYX
#endif