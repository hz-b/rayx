#ifndef NO_VULKAN

#pragma once

#include <vk_mem_alloc.h>

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "VulkanEngine/Common.h"
#include "VulkanEngine/Init/Initializers.h"
#include "VulkanEngine/Run/Pass.h"

namespace RAYX {

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const int WORKGROUP_SIZE = 32;
/// size of the staging buffer in bytes, equal to 128MB.
const size_t DEFAULT_STAGING_SIZE = 134217728;  // = 128MB
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

/// the argument type of `VulkanEngine::run(_)`
struct ComputeRunSpec {
    uint32_t m_numberOfInvocations;
    int maxBounces;
};

class RAYX_API VulkanEngine {
  public:
    VulkanEngine() = default;
    ~VulkanEngine();

    /// changes the state from PREINIT to PRERUN.
    void init(int deviceID);
    void initBufferHandler();

    // TODO (OS): Add the Vulkan state FSM Controls
    void createComputePipelinePass(const ComputePassCreateInfo&);
    void prepareComputePipelinePass(int index);
    void prepareComputePipelinePass(std::string passName);
    void prepareComputePipelinePasses();
    void printPasses();

    BufferHandler* getBufferHandler() { return m_BufferHandler; }
    ComputePass* getComputePass(std::string passName);

    /// @brief  returns all physical devices of the current instance.
    /// @return a vector of VkPhysicalDevice
    std::vector<VkPhysicalDevice> getPhysicalDevices();

    std::vector<std::vector<Ray>> runTraceComputeTask(ComputeRunSpec spec);

    /// changes the state from POSTRUN to PRERUN.
    /// after this all buffers are deleted and hence readBuffer will fail.
    void cleanup();

    /// There are 3 basic states for the VulkanEngine. Described below.
    /// the variable m_state stores that state.
    enum class EngineStates {
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

    inline EngineStates state() { return m_state; }

    // PushConstants are "constants" updated on each Dispatch Call (or similar) in the pipeline
    // Please pay attention to alignment rules
    // You can change this struct (also in shader)
    PushConstantHandler m_PushConstantHandler;

  private:
    EngineStates m_state = EngineStates::PREINIT;

    int m_deviceID = -1;
    uint32_t m_numberOfInvocations;
    uint64_t m_runs = 0;
    size_t STAGING_SIZE = 0;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device;
    uint32_t m_computeFamily;
    VkCommandPool m_GlobalCommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers = {};
    VkQueue m_ComputeQueue;
    VkDescriptorPool m_DescriptorPool;  // TODO(OS) : Can this be deleted?
    VmaAllocator m_VmaAllocator;

    BufferHandler* m_BufferHandler;             // Buffer management
    std::vector<ComputePass*> m_computePasses;  // Passes for compute operations

    // Sync
    std::unique_ptr<Fence> m_computeFence;

    // Implementation details
    // -------------------------------------------------------------------------------------------------------
    // Init:
    void createInstance();
    // void createCache(); TODO: Try to implement later
    void setupDebugMessenger();
    void pickDevice();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();
    void createCommandBuffers(int commandBuffersCount);
    void createFences() { m_computeFence = std::make_unique<Fence>(m_Device); }
    void traceCommand(VkCommandBuffer& cmdBuffer);
    void recordSimpleTraceCommand(std::string passName, VkCommandBuffer& commandBuffer, int stage);
    void prepareVma();

    // Run:
    void submitCommandBuffer(int cmdBufIndex);
    void updateDescriptorSets(std::string passName);
    void updateAllDescriptorSets();
    VkCommandBuffer createOneTimeCommandBuffer();

    void getAllMemories();
    VkDeviceSize getStagingBufferSize();
};

}  // namespace RAYX

#endif
