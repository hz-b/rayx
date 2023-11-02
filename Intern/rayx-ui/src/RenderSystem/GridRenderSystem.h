#pragma once

#include <memory>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"

/**
 * @brief The GridRenderSystem class handles the rendering of the grid in the scene.
 *
 * It manages the creation of pipelines and pipeline layouts, as well as the rendering process.
 */
class GridRenderSystem {
  public:
    GridRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~GridRenderSystem();

    GridRenderSystem(const GridRenderSystem&) = delete;
    GridRenderSystem& operator=(const GridRenderSystem&) = delete;

    /**
     * @brief Renders the grid.
     *
     * @param frameInfo Information about the current frame.
     */
    void render(FrameInfo& frameInfo);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};
