#include "GridRenderSystem.h"

#include "CanonicalizePath.h"

GridRenderSystem::GridRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

GridRenderSystem::~GridRenderSystem() { vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr); }

void GridRenderSystem::render(FrameInfo& frameInfo) {
    // Bind the pipeline before issuing draw calls
    m_Pipeline->bind(frameInfo.commandBuffer);

    // Bind the descriptor sets that the shaders will use
    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

    // Since the vertex data is generated in the vertex shader, no need to bind a vertex buffer
    // Draw call - 6 vertices for the two-triangle grid (as indicated by the gridPlane array in the vertex shader)
    vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
}

void GridRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    // Define push constants or other pipeline layout configurations specific to the grid

    if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void GridRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
    // Configure the pipeline for the grid, this may differ from the RayRenderSystem
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.depthStencilInfo.depthWriteEnable = VK_FALSE;
    pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    // Use the grid-specific shaders
    const std::string vertexShader = RAYX::canonicalizeRepositoryPath("build/bin/shaders/grid_shader_vert.spv").string();
    const std::string fragmentShader = RAYX::canonicalizeRepositoryPath("build/bin/shaders/grid_shader_frag.spv").string();
    m_Pipeline = std::make_unique<GraphicsPipeline>(m_Device, vertexShader, fragmentShader, pipelineConfig);
}
