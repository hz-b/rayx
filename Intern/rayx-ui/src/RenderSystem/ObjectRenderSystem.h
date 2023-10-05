#pragma once

#include <memory>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"
/**
 * @brief The ObjectRenderSystem class handles the rendering of a collection of RenderObjects.
 *
 * It manages the creation of pipelines and pipeline layouts, as well as the rendering process.
 */
class ObjectRenderSystem {
  public:
    ObjectRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~ObjectRenderSystem();

    ObjectRenderSystem(const ObjectRenderSystem&) = delete;
    ObjectRenderSystem& operator=(const ObjectRenderSystem&) = delete;

    /**
     * @brief Renders a collection of RenderObjects.
     *
     * @param frameInfo Information about the current frame.
     * @param objects A vector of RenderObjects to render.
     */
    void render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};