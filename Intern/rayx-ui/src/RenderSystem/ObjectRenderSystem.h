#pragma once

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"

// std
#include <memory>

class ObjectRenderSystem {
  public:
    ObjectRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~ObjectRenderSystem();

    ObjectRenderSystem(const ObjectRenderSystem&) = delete;
    ObjectRenderSystem& operator=(const ObjectRenderSystem&) = delete;

    void render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};