#ifndef NO_VULKAN

#include "VulkanEngine/Run/Pipeline.h"

#include "VulkanEngine/Buffer/BufferHandler.h"

namespace RAYX {

ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo& createInfo) : m_name(createInfo.passName) {
    m_stagesCount = createInfo.shaderStagesCreateInfos.size();
    m_shaderStages.reserve(m_stagesCount);
    m_Pipelines.reserve(m_stagesCount);

    if (m_stagesCount != 0) {
        // Fill compute stages
        for (uint32_t i = 0; i < m_stagesCount; i++) {
            m_shaderStages[i] = std::make_shared<ShaderStage>(m_Device, createInfo.shaderStagesCreateInfos[i]);
        }
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
    // TODO(OS) : This is wrong, and needs fixing, the handler should not be here but in global VulkanEngine
    auto handler = BufferHandler();
    // std::vector<VkDescriptorSetLayoutBinding> bindings;
    auto bindings = handler.getVulkanBufferBindings();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    // Create the descriptor set layout.
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout));
}

void ComputePipeline::addShaderStage(const ShaderStageCreateInfo& createInfo){
    m_shaderStages.push_back(std::make_shared<ShaderStage>(m_Device,createInfo));
    m_stagesCount++;
}

void ComputePipeline::addShaderStage(const ShaderStage& newStage){
    m_shaderStages.push_back(std::make_shared<ShaderStage>(newStage));
    m_stagesCount++;
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

    // Controlled by shadeStage
    //vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
    // Controlled by DescriptorPool
    //vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

    for (auto pipeline : m_Pipelines) {
        vkDestroyPipeline(m_Device, pipeline.pipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, pipeline.pipelineLayout, nullptr);
    }
}
}  // namespace RAYX
#endif