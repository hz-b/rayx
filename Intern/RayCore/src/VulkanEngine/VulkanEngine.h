#pragma once

#include "vulkan/vulkan.hpp"

namespace RAYX {

struct QueueFamilyIndices {
    uint32_t computeFamily;
    bool hasvalue;

    bool isComplete() { return hasvalue; }
};

class RAYX_API VulkanEngine {
  public:
    VulkanEngine() = default;
    ~VulkanEngine() = default;

    struct Compute {  // Possibilty to add CommandPool, Pipeline etc.. here
        std::vector<uint64_t> m_BufferSizes;
        std::vector<VkBuffer> m_Buffers;
        std::vector<VkDeviceMemory> m_BufferMemories;
    } m_compute;

    struct Staging {
        std::vector<uint64_t> m_BufferSizes;
        std::vector<VkBuffer> m_Buffers;
        std::vector<VkDeviceMemory> m_BufferMemories;
    } m_staging;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device;
    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
    VkShaderModule m_ComputeShaderModule;
    VkCommandPool m_CommandPool;
    VkCommandBuffer m_CommandBuffer;
    VkDescriptorPool m_DescriptorPool;
    VkDescriptorSet m_DescriptorSet;
    VkDescriptorSetLayout m_DescriptorSetLayout;
    VkQueue m_ComputeQueue;
    uint32_t m_QueueFamilyIndex;
    QueueFamilyIndices m_QueueFamily;
};

}  // namespace RAYX
