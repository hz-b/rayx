#ifndef NO_VULKAN

#include "Pipeline.h"

namespace RAYX {

ComputePipeline::ComputePipeline(ComputePipelineCreateInfo createInfo) : m_name(createInfo.passName) {
    if (createInfo.computeShaderPaths.size() != createInfo.entryPoints.size()) {
        RAYX_ERR << "Count mismatch in Compute Pipeline Creation";
    }
    m_stagesCount = createInfo.computeShaderPaths.size();

    m_shaderModules.reserve(m_stagesCount);
    m_Pipelines.reserve(m_stagesCount);

    // Fill compute stages
    for (int i = 0; i < m_stagesCount; i++) {
        m_shaderModules[i].entryPoint = std::move(createInfo.entryPoints[i]);
        m_shaderModules[i].path = std::move(createInfo.computeShaderPaths[i]);
        createShaderModule(createInfo.entryPoints[i], m_shaderModules[i].m_shaderModule);
    }
}

void ComputePipeline::createShaderModule(const char* entryPoint, const VkShaderModule& shaderModule) {
    /* we specify the compute shader stage, and it's entry
    point(main).
    */
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = shaderModule;
    shaderStageCreateInfo.pName = entryPoint;
}

void ComputePipeline::createDescriptorSetLayout() {
    RAYX_PROFILE_FUNCTION();

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    auto bindings = m_BufferHandler.getVulkanBufferBindings();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    // Create the descriptor set layout.
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout));
}

ComputePipeline::ComputePipeline() {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "Creating pipeline...";
    // Todo: validtation layer warning : Consider adding VK_KHR_maintenance4  to support SPIR-V 1.6's localsizeid instead of
    // WorkgroupSizesdsdsdsdDS
    /*
    Now let us actually create the compute pipeline.
    It only consists of a single stage with a compute shader.
    So first we specify the compute shader stage, and it's entry
    point(main).
    */
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = m_shaderModule;
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
    pushConstant.size = m_pushConstants.size;
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
    // VK_CHECK_RESULT(vkCreateComputePipelines(m_Device, m_PipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline))
    RAYX_VERB << "Pipeline created.";
}

ComputePipeline::~ComputePipeline() {
    for (auto shader : m_shaderModules) {
        vkDestroyShaderModule(m_Device, shader.m_shaderModule, nullptr);
    }

    vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
    for (auto pipeline : m_Pipelines) {
        vkDestroyPipeline(m_Device, pipeline.pipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, pipeline.pipelineLayout, nullptr);
    }
}
}  // namespace RAYX
#endif