#pragma once

#include "Device.h"
#include "FrameInfo.h"
#include "GraphicsPipeline.h"

// std
#include <memory>
#include <vector>

class TriangleRenderSystem {
  public:
    TriangleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~TriangleRenderSystem();

    TriangleRenderSystem(const TriangleRenderSystem&) = delete;
    TriangleRenderSystem& operator=(const TriangleRenderSystem&) = delete;

    void renderGameObjects(FrameInfo& frameInfo);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout pipelineLayout;
};