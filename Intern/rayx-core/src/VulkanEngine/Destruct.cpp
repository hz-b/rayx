#ifndef NO_VULKAN

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
        checkVkResult(vkGetPipelineCacheData(device, cache, &size, nullptr));

        /* Get data of pipeline cache */
        std::vector<uint8_t> data(size);
        checkVkResult(vkGetPipelineCacheData(device, cache, &size, data.data()));

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
    RAYX_PROFILE_FUNCTION_STDOUT();
    for (auto& cmdBuf : m_CommandBuffers) {
        vkFreeCommandBuffers(m_Device, m_GlobalCommandPool, 1, &cmdBuf);
    }

    vkDestroyCommandPool(m_Device, m_GlobalCommandPool, nullptr);

    // Destroy Fences
    m_computeFence.reset();

    // Delete BufferHandler and compute Pass
    // TODO: Needs better interface
    delete m_BufferHandler;
    for (auto pass : m_computePasses) {
        delete pass;
    }

    vmaDestroyAllocator(m_VmaAllocator);
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

#endif
