#include <vk_mem_alloc.h>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

//	This function destroys the debug messenger
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    RAYX_PROFILE_FUNCTION();
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VulkanEngine::~VulkanEngine() {
    if (m_state == EngineState::PREINIT) {
        return; /* nothing to clean up! */
    }
    if (m_state == EngineState::POSTRUN) {
        RAYX_WARN << ".cleanup() was not called after run!";
        cleanup();
    }

    // vkDestroyBuffer(m_Device, m_stagingBuffer, nullptr);
    // vkFreeMemory(m_Device, m_stagingMemory, nullptr);

    vmaDestroyBuffer(m_VmaAllocator, m_stagingBuffer.m_Buffer, m_stagingBuffer.m_BufferAllocation);
    vmaDestroyAllocator(m_VmaAllocator);
    vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    {
        RAYX_PROFILE_SCOPE("vkDestroyDevice");
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
