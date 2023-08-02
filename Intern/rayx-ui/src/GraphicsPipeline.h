#pragma once

#include <vulkan/vulkan.h>

#include "PipelineLayout.h"

class GraphicsPipeline {
  public:
    GraphicsPipeline(VkDevice device, VkRenderPass renderPass, PipelineLayout& pipelineLayout, const std::string& vertShaderPath,
                     const std::string& fragShaderPath, VkPrimitiveTopology topology, VkPolygonMode polygonMode,
                     VkPipelineVertexInputStateCreateInfo vertexInputInfo);
    ~GraphicsPipeline() { vkDestroyPipeline(m_Device, m_Pipeline, nullptr); }

    VkPipeline getHandle() const { return m_Pipeline; }

  private:
    const VkDevice m_Device;
    VkPipeline m_Pipeline;
};