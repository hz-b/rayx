#include "TriangleRenderSystem.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};
};

TriangleRenderSystem::TriangleRenderSystem(Device& device, Scene& scene, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : m_Device{device}, m_Scene{scene} {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

TriangleRenderSystem::~TriangleRenderSystem() { vkDestroyPipelineLayout(m_Device.device(), pipelineLayout, nullptr); }

void TriangleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void TriangleRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    m_Pipeline = std::make_unique<GraphicsPipeline>(m_Device, "../../../Intern/rayx-ui/src/Shaders/vert.spv",
                                                    "../../../Intern/rayx-ui/src/Shaders/frag.spv", pipelineConfig);
}

void TriangleRenderSystem::render(FrameInfo& frameInfo) {
    m_Pipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

    m_Scene.bind(frameInfo.commandBuffer, Scene::Topography::TRIA_TOPOGRAPHY);
    m_Scene.draw(frameInfo.commandBuffer, Scene::Topography::TRIA_TOPOGRAPHY);
}