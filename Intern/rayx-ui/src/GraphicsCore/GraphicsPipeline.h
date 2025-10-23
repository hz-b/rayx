#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "Device.h"

struct PipelineConfigInfo {
    PipelineConfigInfo()                                     = default;
    PipelineConfigInfo(const PipelineConfigInfo&)            = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};      // Not set in default config
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};  // Not set in default config
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;

    VkPipelineLayout pipelineLayout = nullptr;                         // Not set in default config
    VkRenderPass renderPass         = nullptr;                         // Not set in default config
    VkPrimitiveTopology topology    = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;  // Not set in default config
    VkPolygonMode polygonMode       = VK_POLYGON_MODE_MAX_ENUM;        // Not set in default config
    uint32_t subpass                = 0;                               // Not set in default config
};

/**
 * @brief The GraphicsPipeline class encapsulates a Vulkan graphics pipeline.
 *
 * It handles the creation and destruction of the pipeline, including shader modules, etc.
 */
class GraphicsPipeline {
  public:
    GraphicsPipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& createInfo);
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&)            = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);

    /**
     * @brief Provides default configuration information for a graphics pipeline.
     *
     * @param configInfo The configuration information to be filled with default values.
     */
    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
    VkPipeline getHandle() const { return m_Pipeline; }

  private:
    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& createInfo);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    const Device& m_Device;
    VkPipeline m_Pipeline;
    VkShaderModule m_VertShaderModule;
    VkShaderModule m_FragShaderModule;
};
