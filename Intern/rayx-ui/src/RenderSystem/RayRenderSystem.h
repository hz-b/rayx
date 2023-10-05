#pragma once

#include <memory>
#include <optional>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"

struct Line;

/**
 * @brief The RayRenderSystem class handles the rendering of a RenderObject representing rays.
 *
 * It manages the creation of pipelines and pipeline layouts, as well as the rendering process.
 */
class RayRenderSystem {
  public:
    RayRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~RayRenderSystem();

    RayRenderSystem(const RayRenderSystem&) = delete;
    RayRenderSystem& operator=(const RayRenderSystem&) = delete;

    /**
     * @brief Renders a RenderObject representing rays.
     *
     * @param frameInfo Information about the current frame.
     * @param renderObj An optional RenderObject representing rays.
     */
    void render(FrameInfo& frameInfo, const std::optional<RenderObject>& renderObj);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};