#pragma once

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "RayCore.h"

namespace RAYX {

struct DeclareBufferSpec {
    uint32_t m_binding;
    bool m_in;
    bool m_out;
};

struct InitSpec {
    const char* m_shader;
};

struct RunSpec {
    uint32_t m_numberOfInvocations;
};

class RAYX_API VulkanEngine {
  public:
    VulkanEngine() = default;
    ~VulkanEngine();

    void declareBuffer(const char* bufname, DeclareBufferSpec);
    void init(InitSpec);

    template <typename T>
    inline void createBufferWithData(const char* bufname, std::vector<T> vec) {
        createBuffer(bufname, vec.size() * sizeof(T));
        writeBufferRaw(bufname, (char*)vec.data());
    }
    void createBuffer(const char* bufname, VkDeviceSize);

    void run(RunSpec);

    template <typename T>
    inline std::vector<T> readBuffer(const char* bufname) {
        std::vector<T> out(m_buffers[bufname].m_size / sizeof(T));
        readBufferRaw(bufname, (char*)out.data());
        return out;
    }

    void cleanup();

    // TYPES

    enum class EngineState {
        // the state before .init() is called.
        // legal functions: declareBuffer(), init().
        PREINIT,

        // PRERUN can be reached by either calling .init() from the PREINIT
        // state,
        // or .cleanup() from the POSTRUN state.
        // legal functions: createBuffer*, run().
        PRERUN,

        // the state after run() has been called.
        // legal functions: readOutBuffer(), cleanup().
        POSTRUN
    };

    struct Buffer {
        bool m_in;
        bool m_out;
        uint32_t m_binding;
        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;
        VkDeviceSize m_size;
    };

  private:
    EngineState m_state = EngineState::PREINIT;
    std::map<std::string, Buffer> m_buffers;
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
    uint32_t m_computeFamily;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // private implementation details - they should be kept at the bottom of
    // this file. don't bother reading them.
    ////////////////////////////////////////////////////////////////////////////////////////////

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

    // CreateBuffer.cpp:
    void createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties, VkBuffer& buffer,
                        VkDeviceMemory& bufferMemory);
    // BufferIO:
    void storeToStagingBuffer(char* indata, uint32_t bytes);
    void loadFromStagingBuffer(char* outdata, uint32_t bytes);
    void gpuMemcpy(VkBuffer& buffer_src, uint32_t offset_src,
                   VkBuffer& buffer_dst, uint32_t offset_dst, uint32_t bytes);
    void readBufferRaw(const char* bufname, char* outdata);
    void writeBufferRaw(const char* bufname, char* indata);
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

// set debug generation information
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const int WORKGROUP_SIZE = 32;
const uint32_t STAGING_SIZE = 134217728;  // in bytes, equal to 128MB.

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

}  // namespace RAYX
