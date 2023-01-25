#include <vk_mem_alloc.h>

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
    vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

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
