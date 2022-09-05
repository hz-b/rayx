#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "RayCore.h"

namespace RAYX {

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// set debug generation information
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

struct QueueFamilyIndices {
    uint32_t computeFamily;
    bool hasvalue;

    bool isComplete() { return hasvalue; }
};

class RAYX_API VulkanEngine {
  public:
    VulkanEngine() = default;
    ~VulkanEngine() = default;

    void initVk();

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

    // private implementation details - they should be kept at the bottom of
    // this file. don't bother reading them.
    void createInstance();
    void setupDebugMessenger();
	void pickDevice();
	void pickPhysicalDevice();
	void createLogicalDevice();
};

}  // namespace RAYX
