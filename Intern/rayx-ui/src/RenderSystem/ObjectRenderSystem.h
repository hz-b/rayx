#pragma once

#include <memory>
#include <vector>

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
    ObjectRenderSystem(Device& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts);
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

    /**
     * @brief Rebuilds the pipeline and pipeline layout.
     *
     * @param renderPass The render pass to use for the pipeline.
     * @param setLayouts The descriptor set layouts to use for the pipeline layout.
     */
    void rebuild(VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& setLayouts);

  private:
    void createPipelineLayout(const std::vector<VkDescriptorSetLayout>&);
    void createPipeline(VkRenderPass renderPass);

    Device& m_Device;

    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};