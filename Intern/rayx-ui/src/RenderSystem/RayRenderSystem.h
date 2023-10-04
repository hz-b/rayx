#pragma once

#include <memory>
#include <optional>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"

struct Line;

class RayRenderSystem {
  public:
    RayRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~RayRenderSystem();

    RayRenderSystem(const RayRenderSystem&) = delete;
    RayRenderSystem& operator=(const RayRenderSystem&) = delete;

    void render(FrameInfo& frameInfo, const std::optional<RenderObject>& renderObj);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};