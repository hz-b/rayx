#include "RayRenderSystem.h"

#include "RenderObject.h"

RayRenderSystem::RayRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

RayRenderSystem::~RayRenderSystem() { vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr); }

void RayRenderSystem::render(FrameInfo& frameInfo, std::vector<Line> rays) {
    if (rays.empty()) return;

    m_Pipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

    // Temporarily aggregate all vertices, then create a single RenderObject
    std::vector<Vertex> vertices(rays.size() * 2);
    std::vector<uint32_t> indices(rays.size() * 2);
    for (uint32_t i = 0; i < rays.size(); ++i) {
        vertices[i * 2] = rays[i].v1;
        vertices[i * 2 + 1] = rays[i].v2;
        indices[i * 2] = i * 2;
        indices[i * 2 + 1] = i * 2 + 1;
    }
    RenderObject renderObj(m_Device, glm::mat4(1.0f), vertices, indices);
    renderObj.bind(frameInfo.commandBuffer);
    renderObj.draw(frameInfo.commandBuffer);
}

void RayRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void RayRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
    pipelineConfig.rasterizationInfo.lineWidth = 2.0f;
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_Pipeline = std::make_unique<GraphicsPipeline>(m_Device, "../../../Intern/rayx-ui/src/Shaders/vert.spv",
                                                    "../../../Intern/rayx-ui/src/Shaders/frag.spv", pipelineConfig);
}