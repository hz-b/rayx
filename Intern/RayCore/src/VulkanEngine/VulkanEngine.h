#pragma once

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/GpuData.h"

namespace RAYX {

const int WORKGROUP_SIZE = 32;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

template <typename T>
using dict = std::map<std::string, T>;

struct BufferSpec {
    uint32_t binding;
    bool in;
    bool out;
};

struct InternalBuffer {
    VkBuffer m_Buffer;
    VkDeviceMemory m_Memory;
};

struct InitSpec {
    const char* shaderfile;
    dict<BufferSpec> bufferSpecs;
};
struct RunSpec {
    uint32_t numberOfInvocations;
    uint32_t computeBuffersCount;
    dict<GpuData> buffers;
};

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

    inline void init(InitSpec i) {
        initVk();
        initFromSpec(i);

        m_initSpec = i;
    }

    void createBuffers(RunSpec);
    void fillBuffers(RunSpec);
    void run(RunSpec r);

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

    VkBuffer m_stagingBuffer;
    VkDeviceMemory m_stagingMemory;

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
    std::map<std::string, InternalBuffer> m_internalBuffers;

    std::optional<InitSpec> m_initSpec;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // private implementation details - they should be kept at the bottom of
    // this file. don't bother reading them.
    ////////////////////////////////////////////////////////////////////////////////////////////

    // InitVk/InitVk.cpp
    void initVk();

    // InitVk/CreateInstance.cpp
    void createInstance();
    void setupDebugMessenger();

    // InitVk/PickDevice.cpp
    void pickDevice();
    void pickPhysicalDevice();
    void createLogicalDevice();

    // InitVk/CreateCommandPool.cpp
    void createCommandPool();

    // InitFromSpec.cpp
    void initFromSpec(InitSpec);

    // Run/Prepare.cpp
    void prepareRun(RunSpec);
    void createDescriptorSet(RunSpec);
    void createComputePipeline();
    void createCommandBuffer(RunSpec);

    // Run:
    void runCommandBuffer();

    void fillStagingBuffer(std::string bufname, std::vector<char> data);
};

// Used for validating return values of Vulkan API calls.
#define VK_CHECK_RESULT(f)                                               \
    {                                                                    \
        VkResult res = (f);                                              \
        if (res != VK_SUCCESS) {                                         \
            RAYX_WARN << "Fatal : VkResult fail!";                       \
            RAYX_ERR << "Error code: " << res                            \
                     << ", look up at "                                  \
                        "https://www.khronos.org/registry/vulkan/specs/" \
                        "1.3-extensions/man/html/VkResult.html";         \
        }                                                                \
    }

}  // namespace RAYX
