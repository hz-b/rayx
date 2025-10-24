#include "GridRenderSystem.h"

#include "Rml/Locate.h"

GridRenderSystem::GridRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts)
    : RenderSystem(device, fillInput(renderPass), setLayouts) {}

void GridRenderSystem::render(FrameInfo& frameInfo, [[maybe_unused]] const std::vector<RenderObject>& objects) {
    assert(objects.empty() && "GridRenderSystem should not be used with render objects.");  //! This is still strange. Should be reworked

    // Bind the pipeline before issuing draw calls
    m_Pipeline->bind(frameInfo.commandBuffer);

    // Bind the descriptor sets that the shaders will use
    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

    // Since the vertex data is generated in the vertex shader, no need to bind a vertex buffer
    // Draw call - 6 vertices for the two-triangle grid (as indicated by the gridPlane array in the vertex shader)
    vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
}

RenderSystem::Input GridRenderSystem::fillInput(VkRenderPass renderPass) const {
    return RenderSystem::Input{.renderPass            = renderPass,
                               .vertShaderPath        = rayx::ResourceHandler::getInstance().getResourcePath("Shaders/grid_shader_vert.spv").string(),
                               .fragShaderPath        = rayx::ResourceHandler::getInstance().getResourcePath("Shaders/grid_shader_frag.spv").string(),
                               .bindingDescriptions   = std::vector<VkVertexInputBindingDescription>{},
                               .attributeDescriptions = std::vector<VkVertexInputAttributeDescription>{},
                               .topology              = std::nullopt,
                               .polygonMode           = std::nullopt,
                               .depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL,
                               .lineWidth             = std::nullopt};
}