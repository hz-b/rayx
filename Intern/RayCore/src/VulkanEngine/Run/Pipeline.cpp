#ifndef NO_VULKAN

#include "VulkanEngine/Run/Pipeline.h"

#include <filesystem>
#include <utility>

#include "CanonicalizePath.h"
#include "VulkanEngine/Init/Initializers.h"

namespace RAYX {

Pass::Pipeline::Pipeline(std::string name, VkDevice& dev, const ShaderStageCreateInfo& shaderCreateInfo) : m_name(std::move(name)), m_device(dev) {
    shaderStage = std::make_unique<ShaderStage>(m_device, shaderCreateInfo);
    RAYX_D_LOG << "Pipeline " << m_name << " created";
}

Pass::Pipeline::~Pipeline() {
    cleanPipeline(m_device);
    storePipelineCache(m_device);
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
    auto pushConstant = m_pushConstant.getVkPushConstantRange(shaderStage->getShaderStageFlagBits());

    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;  // One struct of pushConstants

    VK_CHECK_RESULT(vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout))
    RAYX_D_LOG << "PipelineLayout created";
}

// FIXME(OS): Currently only creating compute pipelines. Pipeline, should be general for also other pipelines. Move this somewhere else.
void Pass::Pipeline::createPipeline() {
    /*
    Now, we finally create the compute pipeline.
    */
    auto pipelineCreateInfo = VKINIT::Pipeline::compute_pipeline_create_info(m_pipelineLayout, shaderStage->getPipelineShaderCreateInfo());

    VK_CHECK_RESULT(vkCreateComputePipelines(m_device, nullptr, 1, &pipelineCreateInfo, nullptr, &m_pipeline))
    RAYX_D_LOG << "Pipeline created";
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
    VK_CHECK_RESULT(vkCreatePipelineCache(m_device, &create_info, nullptr, &m_pipelineCache))
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
        VK_CHECK_RESULT(vkGetPipelineCacheData(device, m_pipelineCache, &size, nullptr))

        /* Get data of pipeline cache */
        std::vector<uint8_t> data(size);
        VK_CHECK_RESULT(vkGetPipelineCacheData(device, m_pipelineCache, &size, data.data()))

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
}

void Pass::Pipeline::updatePushConstants(void* data, size_t size) { m_pushConstant.update(data, size); }

// -------------------------------------------------------------------------------------------------------

void Pass::updatePushConstant(int stage, void* data, uint32_t size) { m_pass[stage]->updatePushConstants(data, size); }

// -------------------------------------------------------------------------------------------------------

ComputePass::ComputePass(VkDevice& device, const ComputePassCreateInfo& createInfo) : m_Device(device), m_name(std::string(createInfo.passName)) {
    m_stagesCount = createInfo.shaderStagesCreateInfos.size();
    m_pass.reserve(m_stagesCount);

    m_descriptorSetLayouts.reserve(createInfo.descriptorSetAmount);
    m_descriptorSets.reserve(createInfo.descriptorSetAmount);

    // Fill compute Piplines
    for (uint32_t i = 0; i < m_stagesCount; i++) {
        m_pass[i] = std::make_shared<Pipeline>(createInfo.shaderStagesCreateInfos[i].name, m_Device, createInfo.shaderStagesCreateInfos[i]);
    }
    // Global Desc. Pool
    // createDescriptorPool(10, 10);

    RAYX_D_LOG << "Pass " << m_name << " created";
}

ComputePass::~ComputePass() {
    for (const auto& pipeline : m_pass) {
        pipeline->cleanPipeline(m_Device);
    }

    for (auto layout : m_descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(m_Device, layout, nullptr);
    }
}

void ComputePass::createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    // TODO(OS): Only one Set supported
    auto descriptorSetLayoutCreateInfo = VKINIT::Descriptor::descriptor_set_layout_create_info(bindings);

    // Create the descriptor set layout.
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_oneDescSetLayout))
    RAYX_D_LOG << "DescriptorSetLayout created";
}

// Prepare Compute Pass according to buffers and layout
void ComputePass::prepare(std::vector<VkDescriptorSetLayoutBinding> bindings) {
    RAYX_D_LOG << "Preparing pipelines...";

    // Create Pool if non existent! (Pool sets DO NOT CHANGE) FIXME(OS)
    // if (globalDescriptorPool != nullptr) {
    //}

    createDescriptorSetLayout(bindings);
    createDescriptorPool(1, bindings.size());

    for (const auto& stage : m_pass) {
        stage->createPipelineLayout(&m_oneDescSetLayout);
        stage->createPipeline();
    }
    RAYX_D_LOG << "Done";
}

void ComputePass::addPipelineStage(const ShaderStageCreateInfo& createInfo) {
    m_pass.push_back(std::make_shared<Pipeline>(createInfo.name, m_Device, createInfo));
    m_stagesCount++;
}

// // TODO(OS): No copy cstr for PipelineStage
// void ComputePass::addPipelineStage(const Pass& newStage) {
//     m_pass.push_back(std::make_shared<Pipeline>(newStage));
//     m_stagesCount++;
// }

void ComputePass::createDescriptorPool(uint32_t maxSets, uint32_t bufferCount) {
    // TODO(OS): This should change once we need more sets
    // globalDescriptorPool = DescriptorPool::Builder(m_Device).setMaxSets(maxSets).addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, bufferCount).build();
    // globalDescriptorPool->allocateDescriptor(m_oneDescSetLayout, m_oneDescSet);

    /*
    one descriptor for each buffer
    */
    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSize.descriptorCount = bufferCount;  // = number of buffers

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets = maxSets;  // we need to allocate one descriptor sets from the pool.
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

    // create descriptor pool.
    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_simpleDescPool));

    /*
    With the pool allocated, we can now allocate the descriptor set.
    */
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = m_simpleDescPool;  // pool to allocate from.
    descriptorSetAllocateInfo.descriptorSetCount = 1;             // allocate a single descriptor set.
    descriptorSetAllocateInfo.pSetLayouts = &m_oneDescSetLayout;

    // allocate descriptor set.
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocateInfo, &m_oneDescSet));

    RAYX_D_LOG << "Global pool created ,max sets = " << maxSets << " buffer count = " << bufferCount;
}

void ComputePass::updateDescriptorSets(BufferHandler* bufferHandler) {
    auto writer = DescriptorWriter(m_oneDescSetLayout, *globalDescriptorPool);
    auto buffers = bufferHandler->getBuffers();

    for (auto& [name, b] : *buffers) {
        auto descInfo = b->getDescriptorInfo();
        writer.writeBuffer(b->getPassDescriptorBinding(m_name), &descInfo);
    }

    writer.build(m_oneDescSet);
}

void ComputePass::simpleupdate(BufferHandler* bufferHandler) {
    auto buffers = bufferHandler->getBuffers();
    RAYX_D_LOG << "Simple update";
    for (auto& [name, b] : *buffers) {
        // specify which buffer to use: input buffer
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = b->getBuffer();
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = b->getSize();

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = m_oneDescSet;  // write to this descriptor set.
        writeDescriptorSet.dstBinding = b->getPassDescriptorBinding(m_name);
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;                                 // update a single descriptor.
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // storage buffer.
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        ;

        // perform the update of the descriptor set.
        vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
    }
}

void ComputePass::bindDescriptorSet(const VkCommandBuffer& cmdBuffer, int stage) {
    vkCmdBindDescriptorSets(cmdBuffer, getPipelineBindPoint(), getPipelineLayout(stage), 0, 1, &m_descriptorSets[0], 0, nullptr);
}

void ComputePass::cleanPipeline(int stage) { m_pass[stage]->cleanPipeline(m_Device); }

}  // namespace RAYX
#endif