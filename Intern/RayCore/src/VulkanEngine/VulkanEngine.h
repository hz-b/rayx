#ifndef NO_VULKAN

#pragma once

#include <vk_mem_alloc.h>

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/Buffer/BufferHandler.h"
#include "VulkanEngine/Buffer/VulkanBuffer.h"
#include "VulkanEngine/Common.h"
#include "VulkanEngine/Init/Fence.h"
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

    /// changes the state from PREINIT to PRERUN.
    void newInit();

    // TODO (OS): Add the Vulkan state FSM Controls
    void initBufferHandler();
    void createComputePipelinePass(const ComputePassCreateInfo&);
    void prepareComputePipelinePass();
    BufferHandler& getBufferHandler() const { return *m_BufferHandler; }

    void newRun(VulkanEngineRunSpec_t);

    /// changes the state from POSTRUN to PRERUN.
    /// after this all buffers are deleted (and hence readBuffer will fail.)
    //void cleanup();
    void newCleanup();

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

    const VkDevice& getDevice() const { return m_Device; };
    const VkPhysicalDevice& getPhysicalDevice() const { return m_PhysicalDevice; };
    const VkQueue& getComputeQueue() const { return m_ComputeQueue; };
    const VkQueue& getTransferQueue() const { return m_TransferQueue; };

    // PushConstants are "constants" updated on each Dispatch Call (or similar) in the pipeline
    // Please pay attention to alignment rules
    // You can change this struct (also in shader)
    struct pushConstants_t {
        const void* pushConstPtr = nullptr;
        size_t size = 0;
    };
    pushConstants_t m_pushConstants;
    BufferHandler* m_BufferHandler;  // new
    ComputePass* m_ComputePass;      // New

  private:
    EngineStates_t m_state = EngineStates_t::PREINIT;
    uint32_t m_numberOfInvocations;
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkCommandBuffer m_ComputeCommandBuffer;
    VkCommandBuffer m_TransferCommandBuffer;
    std::vector<VkCommandBuffer> m_CommandBuffers = {};
    VkQueue m_ComputeQueue;
    VkQueue m_TransferQueue;
    VkDescriptorPool m_DescriptorPool;
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
    void createCommandPool();
    void createCommandBuffers();
    void createCommandBuffers(int commandBuffersCount);
    void recordFullCommand();
    void createFences() {
        m_Fences.compute = std::make_unique<Fence>(m_Device);
        m_Fences.transfer = std::make_unique<Fence>(m_Device);
    }

    void recordInComputeCommandBuffer();
    void newRecordInCommandBuffer(ComputePass& computePass);
    void createSemaphores();
    void newCreateSemaphores(int count);
    void prepareVma();

    void getAllMemories();
    VkDeviceSize getStagingBufferSize();

    // Run:
    void submitCommandBuffer();
    void updteDescriptorSets();
    void newUpdateDescriptorSets();
    void createComputePipeline();

    uint64_t m_runs = 0;

    VkCommandBuffer createOneTimeCommandBuffer();
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
