#ifndef NO_VULKAN

#include "Debug/Instrumentor.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * @brief Create Compute pipeline with PushConstants bound
 *
 */
void VulkanEngine::createComputePipeline() {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "Creating pipeline...";
    /*
    Now let us actually create the compute pipeline.
    It only consists of a single stage with a compute shader.
    So first we specify the compute shader stage, and it's entry
    point(main).
    */
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = m_ComputeShaderModule;
    shaderStageCreateInfo.pName = "main";

    /*
    The pipeline layout allows the pipeline to access descriptor sets.
    So we just specify the descriptor set layout we created earlier.
    */
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;

    /*
    Add push constants to the Pipeline
    */
    VkPushConstantRange pushConstant;
    pushConstant.offset = 0;  // Can change this if some of the struct is to be ignored
    pushConstant.size = m_pushConstantsData.size;
    pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;  // One struct of pushConstants

    VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout))

    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = shaderStageCreateInfo;
    pipelineCreateInfo.layout = m_PipelineLayout;
    /*
    Now, we finally create the compute pipeline.
    */
    VK_CHECK_RESULT(vkCreateComputePipelines(m_Device, m_PipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline))
    RAYX_VERB << "Pipeline created.";
}

}  // namespace RAYX

#endif
