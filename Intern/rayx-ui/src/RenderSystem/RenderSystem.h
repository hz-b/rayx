#pragma once

#include <vulkan/vulkan.h>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/GraphicsPipeline.h"
#include "RenderObject.h"

class RenderSystem {
  public:
    struct PushConstantData {
        glm::mat4 modelMatrix{1.0f};
    };
    struct Input {  // Passed to createPipeline function
        VkRenderPass renderPass;
        std::string vertShaderPath;
        std::string fragShaderPath;

        std::optional<std::vector<VkVertexInputBindingDescription>> bindingDescriptions;
        std::optional<std::vector<VkVertexInputAttributeDescription>> attributeDescriptions;
        std::optional<VkPrimitiveTopology> topology;
        std::optional<VkPolygonMode> polygonMode;
        std::optional<VkCompareOp> depthCompareOp;
        std::optional<float> lineWidth;
    };

    RenderSystem(Device& device, const Input& input, const std::vector<VkDescriptorSetLayout>& setLayouts);
    virtual ~RenderSystem();

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    /**
     * @brief Renders a collection of RenderObjects.
     *
     * @param frameInfo Information about the current frame.
     * @param objects A vector of RenderObjects to render.
     */
    virtual void render(FrameInfo& frameInfo, const std::vector<RenderObject>& objects) = 0;

  protected:
    virtual Input fillInput(VkRenderPass) const = 0;
    std::unique_ptr<GraphicsPipeline> m_Pipeline;
    VkPipelineLayout m_PipelineLayout;

  private:
    void createPipelineLayout(const std::vector<VkDescriptorSetLayout>&);
    void createPipeline(const Input& input);

    Device& m_Device;
};
