#pragma once

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/GpuData.h"

namespace RAYX {

template <typename T>
using dict = std::map<std::string, T>;

struct DeclareBufferSpec {
    uint32_t m_binding;
    bool m_in;
    bool m_out;
};

const int WORKGROUP_SIZE = 32;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

enum class EngineState {
    PREINIT,  // the state before .init() is called. only declareBuffer() is
              // legal here.
    PRERUN,   // the state between the between the .init() and .run() calls
    POSTRUN
};

struct InitSpec {
    const char* m_shader;
};

struct RunSpec {
    uint32_t m_numberOfInvocations;
};

struct Buffer {
    bool m_in;
    bool m_out;
    uint32_t m_binding;
    VkBuffer m_Buffer;
    VkDeviceMemory m_Memory;
    VkDeviceSize m_size;
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
    ~VulkanEngine();

    void declareBuffer(const char* bufname, DeclareBufferSpec);
    void init(InitSpec);

    template <typename T>
    inline void defineBufferByData(const char* bufname, std::vector<T> vec) {
        defineBufferByDataRaw(bufname, encode<T>(vec));
    }
    void defineBufferBySize(const char* bufname, VkDeviceSize);

    void run(RunSpec);

    template <typename T>
    inline std::vector<T> readOutBuffer(const char* bufname) {
        return decode<T>(readOutBufferRaw(bufname));
    }

    void cleanup();

    EngineState m_state;
    const char* m_shaderfile;
    uint32_t m_numberOfInvocations;

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
    dict<Buffer> m_buffers;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // private implementation details - they should be kept at the bottom of
    // this file. don't bother reading them.
    ////////////////////////////////////////////////////////////////////////////////////////////

    // ReadOutBuffer:
    GpuData readOutBufferRaw(const char* bufname);

    // Init:
    void createInstance();
    void setupDebugMessenger();
    void pickDevice();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createDescriptorSetLayout();
    void createCommandPool();
    void createStagingBuffer();

    // Run:
    void runCommandBuffer();
    void createDescriptorSet();
    void createComputePipeline();
    void createCommandBuffer();

    // DefineBuffer:
    void defineBufferByDataRaw(const char* bufname, GpuData);
    void createBuffer(const char* bufname, VkDeviceSize size);
    void createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties, VkBuffer& buffer,
                        VkDeviceMemory& bufferMemory);
    void fillBuffer(const char* bufname, GpuData);

    // BufferIO:
    void storeToStagingBuffer(std::vector<char> data);
    std::vector<char> loadFromStagingBuffer(uint32_t bytes);
    void gpuMemcpy(VkBuffer& buffer_src, uint32_t offset_src,
                   VkBuffer& buffer_dst, uint32_t offset_dst, uint32_t bytes);
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
