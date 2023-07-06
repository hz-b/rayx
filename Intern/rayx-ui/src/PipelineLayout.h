#pragma once

#include <vulkan/vulkan.h>

class PipelineLayout {
  public:
    PipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);

    ~PipelineLayout() { vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr); }

    VkPipelineLayout getHandle() const { return m_PipelineLayout; }

  private:
    VkDevice m_Device;
    VkPipelineLayout m_PipelineLayout;
    VkDescriptorSetLayout m_DescriptorSetLayout;
};
