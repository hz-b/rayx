#pragma once

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "Scene.h"

// std
#include <memory>
#include <vector>

class LineRenderSystem {
  public:
    LineRenderSystem(Device& device, Scene& scene, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~LineRenderSystem();

    LineRenderSystem(const LineRenderSystem&) = delete;
    LineRenderSystem& operator=(const LineRenderSystem&) = delete;

    void render(FrameInfo& frameInfo);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;
    Scene& m_Scene;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};