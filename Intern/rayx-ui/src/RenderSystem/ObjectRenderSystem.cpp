#include "ObjectRenderSystem.h"

#include <filesystem>

#include "CanonicalizePath.h"

// Keeping this here so it is easy to add push constants later
struct PushConstantData {
    glm::mat4 modelMatrix{1.f};
};

ObjectRenderSystem::ObjectRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts)
    : m_Device{device} {
    createPipelineLayout(setLayouts);
    createPipeline(renderPass);
}

ObjectRenderSystem::~ObjectRenderSystem() { vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr); }

void ObjectRenderSystem::rebuild(VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts) {
    vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    createPipelineLayout(setLayouts);
    createPipeline(renderPass);
}

void ObjectRenderSystem::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& setLayouts) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void ObjectRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig, GraphicsPipeline::VertexMode::TEXTURED);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    const std::string vertexShader = RAYX::canonicalizeRepositoryPath("build/bin/shader_vert.spv").string();
    const std::string fragmentShader = RAYX::canonicalizeRepositoryPath("build/bin/shader_frag.spv").string();
    m_Pipeline = std::make_unique<GraphicsPipeline>(m_Device, vertexShader, fragmentShader, pipelineConfig);
}

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