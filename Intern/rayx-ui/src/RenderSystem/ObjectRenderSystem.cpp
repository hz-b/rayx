#include "ObjectRenderSystem.h"

#include <filesystem>

#include "Data/Locate.h"
#include "Vertex.h"

ObjectRenderSystem::ObjectRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts)
    : RenderSystem(device, fillInput(renderPass), setLayouts) {}

void ObjectRenderSystem::render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects) {
    if (objects.empty()) return;

    m_Pipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

    for (const RenderObject& obj : objects) {
        VkDescriptorSet objDescrSet = obj.getDescriptorSet();
        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 1, 1, &objDescrSet, 0, nullptr);

        PushConstantData push{};
        push.modelMatrix = obj.getModelMatrix();

        vkCmdPushConstants(frameInfo.commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(PushConstantData), &push);

        obj.bind(frameInfo.commandBuffer);
        obj.draw(frameInfo.commandBuffer);
    }
}

RenderSystem::Input ObjectRenderSystem::fillInput(VkRenderPass renderPass) const {
    return RenderSystem::Input{.renderPass = renderPass,
                               .vertShaderPath = RAYX::ResourceHandler::getInstance().getResourcePath("Shaders/shader_vert.spv").string(),
                               .fragShaderPath = RAYX::ResourceHandler::getInstance().getResourcePath("Shaders/shader_frag.spv").string(),
                               .bindingDescriptions = TextureVertex::getBindingDescriptions(),
                               .attributeDescriptions = TextureVertex::getAttributeDescriptions(),
                               .topology = std::nullopt,
                               .polygonMode = std::nullopt,
                               .depthCompareOp = std::nullopt,
                               .lineWidth = std::nullopt};
}