#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

// TODO: Move viewport and scissor to createInfo
struct GraphicsPipelineCreateInfo {
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    std::string vertShaderPath;
    std::string fragShaderPath;
    VkPrimitiveTopology topology;
    VkPolygonMode polygonMode;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
};

class GraphicsPipeline {
  public:
    GraphicsPipeline(Device& device, const GraphicsPipelineCreateInfo& createInfo);
    ~GraphicsPipeline() { vkDestroyPipeline(m_Device, m_Pipeline, nullptr); }

    VkPipeline getHandle() const { return m_Pipeline; }

  private:
    const VkDevice m_Device;
    VkPipeline m_Pipeline;
};