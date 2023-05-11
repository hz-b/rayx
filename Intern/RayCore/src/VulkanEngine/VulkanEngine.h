#ifndef NO_VULKAN

#pragma once

#include <vk_mem_alloc.h>

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/Buffer/BufferHandler.h"
#include "VulkanEngine/Common.h"
#include "VulkanEngine/Init/Initializers.h"
#include "VulkanEngine/Run/Pipeline.h"

namespace RAYX {

// the argument type of `VulkanEngine::declareBuffer(_)`
struct BufferDeclarationSpec_t {
    uint32_t binding;

    /// expresses whether the buffer is allowed to be initialized with CPU-data.
    bool isInput;

    /// expresses whether the buffer is allowed to be read out to the CPU after
    /// the computation.
    bool isOutput;
};

/// the argument type of `VulkanEngine::init(_)`
struct VulkanEngineInitSpec_t {
    /// the name of the shaderfile, as relative path - relative to the root of
    /// the repository
    const char* shaderFileName;
};

/// the argument type of `VulkanEngine::run(_)`
struct VulkanEngineRunSpec_t {
    uint32_t m_numberOfInvocations;
};

class RAYX_API VulkanEngine {
  public:
    VulkanEngine() = default;
    ~VulkanEngine();

    /// buffers need to be declared before init() is called.
    void declareBuffer(const char* bufname, BufferDeclarationSpec_t);

    /// changes the state from PREINIT to PRERUN.
    void init(VulkanEngineInitSpec_t);
    void newInit();

    // TODO (OS): Add the Vulkan state FSM Controls
    void initBufferHandler();
    void createComputePipelinePass(const ComputePassCreateInfo&);
    BufferHandler* getBufferHandler() const { return m_BufferHandler.get(); }

    /// create a buffer and fill it with the data given in vec.
    /// the buffer will have exactly the size to fit all elements of vec.
    /// only allowed for `m_in = true` buffers.
    template <typename T>
    inline void createBufferWithData(const char* bufname, const std::vector<T>& vec) {
        createBuffer(bufname, vec.size() * sizeof(T));
        writeBufferRaw(bufname, (char*)vec.data());
    }

    /// create a buffer with the given size, it's data is uninitialized.
    void createBuffer(const char* bufname, VkDeviceSize);

    /// changes the state from PRERUN to POSTRUN
    /// This function runs the shader.
    void run(VulkanEngineRunSpec_t);

    /// after run(_) is finished (i.e. in POSTRUN state)
    /// we can read the contents of `m_out = true`-buffers.
    template <typename T>
    inline std::vector<T> readBuffer(const char* bufname) {
        std::vector<T> out(m_buffers[bufname].size / sizeof(T));
        readBufferRaw(bufname, (char*)out.data());
        return out;
    }

    /// changes the state from POSTRUN to PRERUN.
    /// after this all buffers are deleted (and hence readBuffer will fail.)
    void cleanup();

    /// There are 3 basic states for the VulkanEngine. Described below.
    /// the variable m_state stores that state.
    enum class EngineStates_t {
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

    inline EngineStates_t state() { return m_state; }

    /// the internal representation of a buffer.
    /// m_in, m_out, m_binding are taken from DeclareBufferSpec.
    /// the other ones are initialized in createBuffer.
    struct Buffer_t {
        bool isInput;
        bool isOutput;
        uint32_t binding;
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDeviceSize size = 0;
        VmaAllocation alloca = nullptr;
        VmaAllocationInfo allocaInfo;
    };

    const VkDevice& getDevice() const { return m_Device; };
    const VkPhysicalDevice& getPhysicalDevice() const { return m_PhysicalDevice; };
    const VkPipeline& getPipeline() const { return m_Pipeline; };
    const VkQueue& getComputeQueue() const { return m_ComputeQueue; };
    const VkQueue& getTransferQueue() const { return m_TransferQueue; };

    // PushConstants are "constants" updated on each Dispatch Call (or similar) in the pipeline
    // Please pay attention to alignment rules
    // You can change this struct (also in shader)
    struct pushConstants_t {
        const void* pushConstPtr;
        size_t size;
    };
    pushConstants_t m_pushConstants;
    std::unique_ptr<BufferHandler> m_BufferHandler;  // new
    std::unique_ptr<ComputePass> m_ComputePass;      // New
    std::unique_ptr<Pass> m_Pass;                    // New

  private:
    EngineStates_t m_state = EngineStates_t::PREINIT;
    const char* m_shaderfile;
    uint32_t m_numberOfInvocations;

    /// stores the Buffers by name.
    std::map<std::string, Buffer_t> m_buffers;

    /// This is the only staging buffer of the VulkanEngine.
    /// It's size is STAGING_SIZE.
    Buffer_t m_stagingBuffer;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;

    VkPipeline m_Pipeline;
    VkPipelineCache m_PipelineCache;
    VkPipelineLayout m_PipelineLayout;
    VkShaderModule m_ComputeShaderModule;
    VkCommandBuffer m_ComputeCommandBuffer;
    VkCommandBuffer m_TransferCommandBuffer;
    std::vector<VkCommandBuffer> m_CommandBuffers = {};
    VkQueue m_ComputeQueue;
    VkQueue m_TransferQueue;
    VkDescriptorPool m_DescriptorPool;
    VkDescriptorSet m_DescriptorSet;
    VkDescriptorSetLayout m_DescriptorSetLayout;

    // implementation details:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device;
    uint32_t m_computeFamily;
    VkCommandPool m_GlobalCommandPool;
    VmaAllocator m_VmaAllocator;
    size_t STAGING_SIZE = 0;

    // Sync:
    struct {
        VkSemaphore computeSemaphore;
        VkSemaphore transferSemaphore;
    } m_Semaphores;

    std::vector<VkSemaphore> m_newSemaphores = {};
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
    void createCommandBuffers(int commandBuffersCount);
    void createShaderModule();
    void recordFullCommand();
    void createFences();
    void recordInComputeCommandBuffer();
    void createSemaphores();
    void newCreateSemaphores(int count);
    void createStagingBuffer();
    void prepareVma();

    void getAllMemories();
    VkDeviceSize getStagingBufferSize();

    // Run:
    void submitCommandBuffer();
    void updteDescriptorSets();
    void createComputePipeline();

    uint64_t m_runs = 0;

    VkCommandBuffer createOneTimeCommandBuffer();

    // CreateBuffer.cpp:

    /// note that there also is a public createBuffer to create a `Buffer`,
    /// and a private `createVkBuffer` which constructs an actual `VkBuffer`.
    /// This is merely a helper method for `createBuffer` and
    /// `createStagingBuffer`.
    void createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                        VkDeviceMemory& bufferMemory);
    // VMA Version of createVkBuffer
    void createVmaBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkBuffer& buffer, VmaAllocation& allocation,
                         VmaAllocationInfo* allocation_info, VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                         VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO, const std::vector<uint32_t>& queue_family_indices = {});
    // BufferIO:
    /// copies data from one buffer to the other with given offsets.
    /// this is used for the buffer <-> staging buffer communication in
    /// Careful : This is not an awaiting command so make sure to check the according fence transfer
    /// or Queue Idle before copying again
    /// {read,write}BufferRaw.
    void gpuMemcpy(VkBuffer& buffer_dst, size_t offset_dst, VkBuffer& buffer_src, size_t offset_src, size_t bytes);

    /// reads a buffer and writes the data to `outdata`.
    /// the full buffer is read (the size is `m_buffers[bufname].m_size`)
    /// this function uses m_stagingBuffer to read the data in chunks of
    /// STAGING_SIZE. only allowed for buffers with `m_out = true`.
    void readBufferRaw(const char* bufname, char* outdata);

    /// writes the `indata` to the buffer.
    /// It will write the full `m_buffers[bufname].m_size` bytes.
    /// this function uses m_stagingBuffer to write the data in chunks of
    /// STAGING_SIZE. only allowed for buffers with `m_in = true`.
    void writeBufferRaw(const char* bufname, char* indata);

    /// loads `bytes` many bytes from the staging buffer into `outdata`.
    void loadFromStagingBuffer(char* outdata, size_t bytes);

    /// writes `bytes` many bytes from `indata` into the staging buffer.
    void storeToStagingBuffer(char* indata, size_t bytes);

  protected:
};

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
