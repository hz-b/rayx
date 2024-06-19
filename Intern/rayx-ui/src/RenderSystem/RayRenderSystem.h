#pragma once

#include <memory>
#include <optional>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"
#include "RenderSystem.h"

struct Line;

/**
 * @brief The RayRenderSystem class handles the rendering of a RenderObject representing rays.
 *
 * It manages the creation of pipelines and pipeline layouts, as well as the rendering process.
 */
class RayRenderSystem : public RenderSystem {
  public:
    RayRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts);

    /**
     * @brief Renders a RenderObject representing rays.
     *
     * @param frameInfo Information about the current frame.
     * @param renderObj An optional RenderObject representing rays.
     */
    void render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects) override;

  private:
    RenderSystem::Input fillInput(VkRenderPass renderPass) const override;
};