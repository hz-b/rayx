#include <vk_mem_alloc.h>

#include "CanonicalizePath.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

//	This function destroys the debug messenger
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

/**
 * @brief Store and destroy Cache
 *
 * @param cache Cache Object
 * @param device Vulkan Logical device
 */
void inline storePipelineCache(VkDevice& device, VkPipelineCache& cache) {
    if (cache != VK_NULL_HANDLE) {
        /* Get size of pipeline cache */
        size_t size{};
        VK_CHECK_RESULT(vkGetPipelineCacheData(device, cache, &size, nullptr));

        /* Get data of pipeline cache */
        std::vector<uint8_t> data(size);
        VK_CHECK_RESULT(vkGetPipelineCacheData(device, cache, &size, data.data()));

        // Cache is stored in OS TEMP
        auto tmpDir = std::filesystem::temp_directory_path();
        try {
            /* Write pipeline cache data to a file in binary format */
            writeFile(data, (tmpDir / "pipeline_cache.data").string());
        } catch (std::runtime_error& ex) {
            RAYX_WARN << "No pipeline cache written.";
        }

        /* Destroy Vulkan pipeline cache */
        vkDestroyPipelineCache(device, cache, nullptr);
    }
}

VulkanEngine::~VulkanEngine() {
    if (m_state == VulkanEngineStates_t::PREINIT) {
        return; /* nothing to clean up! */
    }
    if (m_state == VulkanEngineStates_t::POSTRUN) {
        RAYX_WARN << ".cleanup() was not called after run!";
        cleanup();
    }

    // vkDestroyBuffer(m_Device, m_stagingBuffer, nullptr);
    // vkFreeMemory(m_Device, m_stagingMemory, nullptr);

    vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_ComputeCommandBuffer);
    vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_TransferCommandBuffer);

    vmaDestroyBuffer(m_VmaAllocator, m_stagingBuffer.buf, m_stagingBuffer.alloca);
    vmaDestroyAllocator(m_VmaAllocator);
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

    vkDestroySemaphore(m_Device, m_Semaphores.computeSemaphore, nullptr);
    vkDestroySemaphore(m_Device, m_Semaphores.transferSemaphore, nullptr);

    // Destroy Fences
    m_Fences.computeFence.reset();
    m_Fences.transferFence.reset();

    vkDestroyShaderModule(m_Device, m_ComputeShaderModule, nullptr);
    storePipelineCache(m_Device, m_PipelineCache);

    {
        RAYX_PROFILE_SCOPE_STDOUT("vkDestroyDevice");
        vkDestroyDevice(m_Device, nullptr);
    }
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
    {
        RAYX_PROFILE_SCOPE("vkDestroyInstance");
        vkDestroyInstance(m_Instance, nullptr);
    }
}

}  // namespace RAYX
