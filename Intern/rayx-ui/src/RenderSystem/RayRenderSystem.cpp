#include "RayRenderSystem.h"

#include "CanonicalizePath.h"
#include "RenderObject.h"
#include "Vertex.h"

RayRenderSystem::RayRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts)
    : RenderSystem(device, fillInput(renderPass), setLayouts) {}

void RayRenderSystem::render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects) {
    if (objects.empty()) return;
    assert(objects.size() == 1 && "This render function expects the rays to be bundled into one object");  // ! Same as GrindRenderSystem issue

    m_Pipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

    objects[0].bind(frameInfo.commandBuffer);
    objects[0].draw(frameInfo.commandBuffer);
}

RenderSystem::Input RayRenderSystem::fillInput(VkRenderPass renderPass) const {
    return RenderSystem::Input{.renderPass = renderPass,
                               .vertShaderPath = RAYX::getExecutablePath().string() + "/Shaders/ray_shader_vert.spv",
                               .fragShaderPath = RAYX::getExecutablePath().string() + "/Shaders/ray_shader_frag.spv",
                               .bindingDescriptions = ColorVertex::getBindingDescriptions(),
                               .attributeDescriptions = ColorVertex::getAttributeDescriptions(),
                               .topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
                               .polygonMode = VK_POLYGON_MODE_LINE,
                               .depthCompareOp = std::nullopt,
                               .lineWidth = 2.0f};
}