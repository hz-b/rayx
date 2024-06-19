#pragma once

#include <memory>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"
#include "RenderSystem.h"

/**
 * @brief The GridRenderSystem class handles the rendering of the grid in the scene.
 *
 * It manages the creation of pipelines and pipeline layouts, as well as the rendering process.
 */
class GridRenderSystem : public RenderSystem {
  public:
    GridRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts);

    /**
     * @brief Renders the grid.
     *
     * @param frameInfo Information about the current frame.
     */
    void render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects) override;

  private:
    RenderSystem::Input fillInput(VkRenderPass renderPass) const override;
};
