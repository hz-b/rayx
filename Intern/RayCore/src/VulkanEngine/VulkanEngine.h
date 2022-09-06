#pragma once

#include <vector>
#include <map>
#include <vulkan/vulkan.hpp>
#include <algorithm>

#include "RayCore.h"

namespace RAYX {

const int WORKGROUP_SIZE = 32;
const uint32_t STAGING_SIZE = 134217728; // in bytes, equal to 128MB.

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct BufferSpec {
    const char* name;
    uint32_t binding;
    bool in;
    bool out;
};

// the inner std::vector has size at most STAGING_SIZE.
using RawData = std::vector<std::vector<char>>;

struct Buffer {
    const char* name;
    RawData data;
};

struct InternalBuffer {
	VkBuffer m_Buffer;
	VkDeviceMemory m_Memory;
};

struct InitSpec {
    const char* shaderfile;
    std::vector<BufferSpec> bufferSpecs;
};
struct RunSpec {
    uint32_t numberOfInvocations;
    uint32_t computeBuffersCount;
    std::vector<Buffer> buffers;
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

// in order to send data to the VulkanEngine, it needs to be converted to raw bytes (i.e. chars).
// the performance of those encode/decode can definitely be improved, as a copy is not generally necessary.

// TODO optimize those functions!

template <typename T>
inline RawData encode(std::vector<T> in) {
	uint32_t remaining_bytes = in.size() * sizeof(T);
	char* ptr = (char*) in.data();

	RawData out;
	while (remaining_bytes > 0) {
		// number of bytes transferred in this for-loop
		int localbytes = std::min(STAGING_SIZE, remaining_bytes);

		std::vector<char> subdata(localbytes);
		memcpy(subdata.data(), ptr, localbytes);
		out.push_back(subdata);

		ptr += localbytes;
		remaining_bytes -= localbytes;
	}
	return out;
}

template <typename T>
inline std::vector<T> decode(RawData in) {
	std::vector<T> out;

	std::vector<char> tmp;
	while (!in.empty()) {
		std::vector<char> f = in[0];
		tmp.insert(tmp.end(), f.begin(), f.end());
		in.erase(in.begin());
		while (tmp.size() >= sizeof(T)) {
			T t;
			memcpy(&t, tmp.data(), sizeof(T));
			tmp.erase(tmp.begin(), tmp.begin() + sizeof(T));
			out.push_back(t);
		}
	}
	if (!tmp.empty()) {
		RAYX_ERR << "decode unsuccessful! remaining bytes are not enough to create another element!";
	}
	return out;
}

}  // namespace RAYX
