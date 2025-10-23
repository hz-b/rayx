#include "RenderSystem.h"

RenderSystem::RenderSystem(Device& device, const Input& input, const std::vector<VkDescriptorSetLayout>& setLayouts) : m_Device(device) {
    createPipelineLayout(setLayouts);
    createPipeline(input);
}

RenderSystem::~RenderSystem() { vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr); }

void RenderSystem::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& setLayouts) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset     = 0;
    pushConstantRange.size       = sizeof(PushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(setLayouts.size());
    pipelineLayoutInfo.pSetLayouts            = setLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;  // ? Parameterize
    pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
    if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void RenderSystem::createPipeline(const Input& input) {
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass     = input.renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayout;

    auto setIfPresent = [&](const auto& optionalValue, auto& target) {
        if (optionalValue.has_value()) target = *optionalValue;
    };

    setIfPresent(input.bindingDescriptions, pipelineConfig.bindingDescriptions);
    setIfPresent(input.attributeDescriptions, pipelineConfig.attributeDescriptions);
    setIfPresent(input.topology, pipelineConfig.inputAssemblyInfo.topology);
    setIfPresent(input.polygonMode, pipelineConfig.rasterizationInfo.polygonMode);
    setIfPresent(input.lineWidth, pipelineConfig.rasterizationInfo.lineWidth);
    setIfPresent(input.depthCompareOp, pipelineConfig.depthStencilInfo.depthCompareOp);

    m_Pipeline = std::make_unique<GraphicsPipeline>(m_Device, input.vertShaderPath, input.fragShaderPath, pipelineConfig);
}