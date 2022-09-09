#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

//	This function destroys the debug messenger
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
    RAYX_PROFILE_FUNCTION();
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}


VulkanEngine::~VulkanEngine() {
    vkDestroyBuffer(m_Device, m_stagingBuffer, nullptr);
    vkFreeMemory(m_Device, m_stagingMemory, nullptr);

    vkDestroyDescriptorSetLayout(m_Device,
                                 m_DescriptorSetLayout, nullptr);
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    {
        RAYX_PROFILE_SCOPE("vkDestroyDevice");
        vkDestroyDevice(m_Device, nullptr);
    }
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(m_Instance,
                                      m_DebugMessenger, nullptr);
    }
    {
        RAYX_PROFILE_SCOPE("vkDestroyInstance");
        vkDestroyInstance(m_Instance, nullptr);
    }
}

}
