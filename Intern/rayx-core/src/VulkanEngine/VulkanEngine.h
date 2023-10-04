#ifndef NO_VULKAN

#pragma once

#include <vk_mem_alloc.h>

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "RAY-Core.h"

namespace RAYX {

// the argument type of `VulkanEngine::declareBuffer(_)`
struct BufferDeclarationSpec {
    /// used to map the vulkan buffers to the shader buffers.
    /// Lines like `layout (binding = _)` declare buffers in the shader.
    uint32_t binding;

    /// expresses whether the buffer is allowed to be initialized with CPU-data.
    bool isInput;

    /// expresses whether the buffer is allowed to be read out to the CPU after
    /// the computation.
    bool isOutput;
};

/// the argument type of `VulkanEngine::init(_)`
struct VulkanEngineInitSpec {
    /// the name of the shaderFile, as relative path - relative to the root of
    /// the repository
    const char* shaderFileName;
    int deviceID;
};

/// There are 3 basic states for the VulkanEngine. Described below.
/// the variable m_state stores that state.
enum class VulkanEngineStates {
    // the state before .init() is called.
    // legal functions: declareBuffer(), init().
    PREINIT,

    // PRERUN can be reached by either calling .init() from the PREINIT
    // state,
    // or .cleanup() from the POSTRUN state.
    // legal functions: createBuffer*, run().
    PRERUN,

    // the state after run() has been called.
    // legal functions: readBuffer(), cleanup().
    POSTRUN
};

/// the argument type of `VulkanEngine::run(_)`
struct VulkanEngineRunSpec {
    uint32_t m_numberOfInvocations;
};

class RAYX_API VulkanEngine {
  public:
    VulkanEngine() = default;
    ~VulkanEngine();

    /// buffers need to be declared before init() is called.
    void declareBuffer(const char* buffName, BufferDeclarationSpec);

    /// changes the state from PREINIT to PRERUN.
    void init(VulkanEngineInitSpec);

    /// create a buffer and fill it with the data given in vec.
    /// the buffer will have exactly the size to fit all elements of vec.
    /// only allowed for `m_in = true` buffers.
    template <typename T>
    inline void createBufferWithData(const char* buffName, const std::vector<T>& vec) {
        createBuffer(buffName, vec.size() * sizeof(T));
        writeBufferRaw(buffName, (char*)vec.data());
    }

    /// create a buffer with the given size, it's data is uninitialized.
    void createBuffer(const char* buffName, VkDeviceSize);

    /// changes the state from PRERUN to POSTRUN
    /// This function runs the shader.
    void run(VulkanEngineRunSpec);

    /// @brief  returns all physical devices of the current instance.
    /// @return a vector of VkPhysicalDevice
    std::vector<VkPhysicalDevice> getPhysicalDevices();

    /// after run(_) is finished (i.e. in POSTRUN state)
    /// we can read the contents of `m_out = true`-buffers.
    template <typename T>
    inline std::vector<T> readBuffer(const char* buffName) {
        std::vector<T> out(m_buffers[buffName].size / sizeof(T));
        readBufferRaw(buffName, (char*)out.data());
        return out;
    }

    /// changes the state from POSTRUN to PRERUN.
    /// after this all buffers are deleted and hence readBuffer will fail.
    void cleanup();

    inline VulkanEngineStates state() { return m_state; }

    /// the internal representation of a buffer.
    /// m_in, m_out, m_binding are taken from DeclareBufferSpec.
    /// the other ones are initialized in createBuffer.
    struct Buffer {
        bool isInput;
        bool isOutput;
        uint32_t binding;
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDeviceSize size = 0;
        VmaAllocation alloca = nullptr;
        VmaAllocationInfo allocaInfo;
    };

    // PushConstants are "constants" updated on each Dispatch Call (or similar) in the pipeline
    // Please pay attention to alignment rules
    // You can change this struct (also in shader)
    struct PushConstantsData {
        const void* pushConstPtr;
        size_t size;
    } m_pushConstantsData;

  private:
    VulkanEngineStates m_state = VulkanEngineStates::PREINIT;
    const char* m_shaderFile;
    int m_deviceID = -1;
    uint32_t m_numberOfInvocations;

    /// stores the Buffers by name.
    std::map<std::string, Buffer> m_buffers;

    /// This is the only staging buffer of the VulkanEngine.
    /// It's size is STAGING_SIZE.
    Buffer m_stagingBuffer;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device;
    uint32_t m_computeFamily;
    VkPipeline m_Pipeline;
    VkPipelineCache m_PipelineCache;
    VkPipelineLayout m_PipelineLayout;
    VkShaderModule m_ComputeShaderModule;
    VkCommandPool m_CommandPool;
    VkCommandBuffer m_ComputeCommandBuffer;
    VkCommandBuffer m_TransferCommandBuffer;
    VkQueue m_ComputeQueue;
    VkQueue m_TransferQueue;
    VkDescriptorPool m_DescriptorPool;
    VkDescriptorSet m_DescriptorSet;
    VkDescriptorSetLayout m_DescriptorSetLayout;
    VmaAllocator m_VmaAllocator;

    // implementation details:

    // Init:
    void createInstance();
    void createCache();
    void setupDebugMessenger();
    void pickDevice();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createDescriptorSetLayout();
    void createAllocateDescriptorPool(uint32_t);
    void createCommandPool();
    void createCommandBuffers();
    void createShaderModule();
    void recordFullCommand();
    void createFences();
    void recordInComputeCommandBuffer();
    void createSemaphores();
    void createStagingBuffer();
    void prepareVma();

    void getAllMemories();
    VkDeviceSize getStagingBufferSize();

    // Run:
    void submitCommandBuffer();
    void updateDescriptorSets();
    void createComputePipeline();

    // Sync:
    struct {
        VkSemaphore computeSemaphore;
        VkSemaphore transferSemaphore;
    } m_Semaphores;

    class Fence {
      public:
        Fence(VkDevice& device);
        ~Fence();
        VkFence* fence();
        VkResult wait();
        VkResult forceReset();

      private:
        VkFence f;
        VkDevice device;
    };

    struct {
        std::unique_ptr<Fence> transfer;
        std::unique_ptr<Fence> compute;
    } m_Fences;

    uint64_t m_runs = 0;

    VkCommandBuffer createOneTimeCommandBuffer();

    // CreateBuffer.cpp:

    /// VMA Version of createVkBuffer
    /// note that there also is a public createBuffer to create a `Buffer`,
    /// and a private `createVkBuffer` which constructs an actual `VkBuffer`.
    /// This is merely a helper method for `createBuffer` and
    /// `createStagingBuffer`.
    void createVmaBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkBuffer& buffer, VmaAllocation& allocation,
                         VmaAllocationInfo* allocationInfo, VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                         VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO, const std::vector<uint32_t>& queueFamilyIndices = {});
    // BufferIO:
    size_t STAGING_SIZE = 0;
    /// copies data from one buffer to the other with given offsets.
    /// this is used for the buffer <-> staging buffer communication in
    /// Careful : This is not an awaiting command so make sure to check the according fence transfer
    /// or Queue Idle before copying again
    /// {read,write}BufferRaw.
    void gpuMemcpy(VkBuffer& bufferDst, size_t offsetDst, VkBuffer& bufferSrc, size_t offsetSrc, size_t bytes);

    /// reads a buffer and writes the data to `outData`.
    /// the full buffer is read (the size is `m_buffers[buffName].m_size`)
    /// this function uses m_stagingBuffer to read the data in chunks of
    /// STAGING_SIZE. only allowed for buffers with `m_out = true`.
    void readBufferRaw(const char* buffName, char* outData);

    /// writes the `inData` to the buffer.
    /// It will write the full `m_buffers[buffName].m_size` bytes.
    /// this function uses m_stagingBuffer to write the data in chunks of
    /// STAGING_SIZE. only allowed for buffers with `m_in = true`.
    void writeBufferRaw(const char* buffName, char* inData);

    /// loads `bytes` many bytes from the staging buffer into `outdata`.
    void loadFromStagingBuffer(char* outData, size_t bytes);

    /// writes `bytes` many bytes from `inData` into the staging buffer.
    void storeToStagingBuffer(char* inData, size_t bytes);
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

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const int WORKGROUP_SIZE = 32;

/// size of the staging buffer in bytes, equal to 128MB.
const size_t DEFAULT_STAGING_SIZE = 134217728;  // = 128MB

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

}  // namespace RAYX

#endif
