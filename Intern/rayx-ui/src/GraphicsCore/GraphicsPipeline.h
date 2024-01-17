#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

struct PipelineConfigInfo {
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;

    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    VkPrimitiveTopology topology;
    VkPolygonMode polygonMode;
    uint32_t subpass = 0;
};

/**
 * @brief The GraphicsPipeline class encapsulates a Vulkan graphics pipeline.
 *
 * It handles the creation and destruction of the pipeline, including shader modules, etc.
 */
class GraphicsPipeline {
  public:
    enum class VertexMode { COLORED, TEXTURED };
    GraphicsPipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& createInfo);
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);

    /**
     * @brief Provides default configuration information for a graphics pipeline.
     *
     * @param configInfo The configuration information to be filled with default values.
     */
    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, VertexMode vertexMode = VertexMode::TEXTURED);
    VkPipeline getHandle() const { return m_Pipeline; }

  private:
    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& createInfo);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    const Device& m_Device;
    VkPipeline m_Pipeline;
    VkShaderModule m_VertShaderModule;
    VkShaderModule m_FragShaderModule;
};