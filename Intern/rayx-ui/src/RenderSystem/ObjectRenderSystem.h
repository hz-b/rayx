#pragma once

#include <memory>
#include <vector>

#include "RenderSystem/RenderSystem.h"

/**
 * @brief The ObjectRenderSystem class handles the rendering of a collection of RenderObjects.
 *
 * It manages the creation of pipelines and pipeline layouts, as well as the rendering process.
 */
class ObjectRenderSystem : public RenderSystem {
  public:
    ObjectRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts);

    void render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects) override;

  private:
    RenderSystem::Input fillInput(VkRenderPass renderPass) const override;
};