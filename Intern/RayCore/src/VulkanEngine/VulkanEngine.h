#pragma once

#include <algorithm>
#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "RayCore.h"

namespace RAYX {

/// the argument type of `VulkanEngine::declareBuffer(_)`
struct DeclareBufferSpec {
    /// used to map the vulkan buffers to the shader buffers.
    /// Lines like `layout (binding = _)` declare buffers in the shader.
    uint32_t m_binding;

    /// expresses whether the buffer is allowed to be initialized with CPU-data.
    bool m_in;

    /// expresses whether the buffer is allowed to be read out to the CPU after the computation.
    bool m_out;
};

/// the argument type of `VulkanEngine::init(_)`
struct InitSpec {
    /// the name of the shaderfile, as relative path - relative to the root of the repository
    const char* m_shader;
};


/// the argument type of `VulkanEngine::run(_)`
struct RunSpec {
    uint32_t m_numberOfInvocations;
};

class RAYX_API VulkanEngine {
  public:
    VulkanEngine() = default;
    ~VulkanEngine();

    /// buffers need to be declared before init() is called.
    void declareBuffer(const char* bufname, DeclareBufferSpec);

    /// changes the state from PREINIT to PRERUN.
    void init(InitSpec);

    /// creates a buffer and fill it with the data given in vec.
    /// the buffer will have exactly the size to fit all elements of vec.
    template <typename T>
    inline void createBufferWithData(const char* bufname, const std::vector<T>& vec) {
        createBuffer(bufname, vec.size() * sizeof(T));
        writeBufferRaw(bufname, (char*)vec.data());
    }
    /// create a buffer with the given size, it's data is uninitialized.
    void createBuffer(const char* bufname, VkDeviceSize);

    /// changes the state from PRERUN to POSTRUN
    /// This function runs the shader.
    void run(RunSpec);

    /// after run(_) is finished (i.e. in POSTRUN state)
    /// we can read the contents of `m_out = true`-buffers.
    template <typename T>
    inline std::vector<T> readBuffer(const char* bufname) {
        std::vector<T> out(m_buffers[bufname].m_size / sizeof(T));
        readBufferRaw(bufname, (char*)out.data());
        return out;
    }

    /// changes the state from POSTRUN to PRERUN.
    /// after this all buffers are deleted and hence readBuffer will fail.
    void cleanup();

    /// There are 3 basic states for the VulkanEngine. Described below.
    /// the variable m_state stores that state.
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
        // legal functions: readBuffer(), cleanup().
        POSTRUN
    };

    inline EngineState state() { return m_state; }

    /// the internal representation of a buffer.
    /// m_in, m_out, m_binding are taken from DeclareBufferSpec.
    /// the other ones are initialized in createBuffer.
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
    /// stores the Buffers by name.
    std::map<std::string, Buffer> m_buffers;
    const char* m_shaderfile;
    uint32_t m_numberOfInvocations;

    /// there is only a single staging buffer. This one.
    VkBuffer m_stagingBuffer;
    /// the memory of the staging buffer above.
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

    // implementation details:

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

    /// note that there also is a public createBuffer to create a `Buffer`,
    /// and a private `createVkBuffer` which constructs an actual `VkBuffer`.
    /// This is merely a helper method for `createBuffer` and `createStagingBuffer`.
    void createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties, VkBuffer& buffer,
                        VkDeviceMemory& bufferMemory);
    // BufferIO:

    /// copies data from one buffer to the other with given offsets.
    /// note that gpuMemcpy copies from left to right, unlike the original memcpy.
    /// this is used for the buffer <-> staging buffer communication in {read,write}BufferRaw.
    void gpuMemcpy(VkBuffer& buffer_src, uint32_t offset_src,
                   VkBuffer& buffer_dst, uint32_t offset_dst, uint32_t bytes);

    /// reads a buffer and writes the data to `outdata`.
    /// the full buffer is read (the size is `m_buffers[bufname].m_size`)
    /// this function uses m_stagingBuffer to read the data in chunks of STAGING_SIZE.
    /// only allowed for buffers with `m_out = true`.
    void readBufferRaw(const char* bufname, char* outdata);

    /// writes the `indata` to the buffer.
    /// It will write the full `m_buffers[bufname].m_size` bytes.
    /// this function uses m_stagingBuffer to write the data in chunks of STAGING_SIZE.
    /// only allowed for buffers with `m_in = true`.
    void writeBufferRaw(const char* bufname, char* indata);

    /// loads `bytes` many bytes from the staging buffer into `outdata`.
    void loadFromStagingBuffer(char* outdata, uint32_t bytes);

    /// writes `bytes` many bytes from `indata` into the staging buffer.
    void storeToStagingBuffer(char* indata, uint32_t bytes);
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
