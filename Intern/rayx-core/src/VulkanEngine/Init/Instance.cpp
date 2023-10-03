#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace RAYX {

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

bool checkValidationLayerSupport();

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

std::vector<const char*> getRequiredExtensions();

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

void setupDebugMessenger(VkInstance& inst);

/* Create a new Vulkan library instance. (Validation layers included) */
void VulkanEngine::createInstance() {
    RAYX_PROFILE_FUNCTION();
    // validation layers are used for debugging
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    // Add description for instance
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "rayx-core";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 216);
    appInfo.pEngineName = "rayx-core Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 3, 216);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // pointer to description with layer count
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Validation Layer Debug Output "handler"
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    // create instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
    if (result != VK_SUCCESS) {
        RAYX_ERR << "Failed to create instance! Error Code " << result;
    }
}

void VulkanEngine::prepareVma() {
    // VmaVulkanFunctions vma_vulkan_func{};
    // vma_vulkan_func.vkAllocateMemory                    = vkAllocateMemory;
    // vma_vulkan_func.vkBindBufferMemory                  = vkBindBufferMemory;
    // vma_vulkan_func.vkBindImageMemory                   = vkBindImageMemory;
    // vma_vulkan_func.vkCreateBuffer                      = vkCreateBuffer;
    // vma_vulkan_func.vkCreateImage                       = vkCreateImage;
    // vma_vulkan_func.vkDestroyBuffer                     = vkDestroyBuffer;
    // vma_vulkan_func.vkDestroyImage                      = vkDestroyImage;
    // vma_vulkan_func.vkFlushMappedMemoryRanges           = vkFlushMappedMemoryRanges;
    // vma_vulkan_func.vkFreeMemory                        = vkFreeMemory;
    // vma_vulkan_func.vkGetBufferMemoryRequirements       = vkGetBufferMemoryRequirements;
    // vma_vulkan_func.vkGetImageMemoryRequirements        = vkGetImageMemoryRequirements;
    // vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    // vma_vulkan_func.vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties;
    // vma_vulkan_func.vkInvalidateMappedMemoryRanges      = vkInvalidateMappedMemoryRanges;
    // vma_vulkan_func.vkMapMemory                         = vkMapMemory;
    // vma_vulkan_func.vkUnmapMemory                       = vkUnmapMemory;
    // vma_vulkan_func.vkCmdCopyBuffer                     = vkCmdCopyBuffer;

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = m_PhysicalDevice;
    allocatorInfo.device = m_Device;
    allocatorInfo.instance = m_Instance;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    // allocatorInfo.pVulkanFunctions = &vma_vulkan_func;

    // VmaAllocator allocator;
    auto result = vmaCreateAllocator(&allocatorInfo, &m_VmaAllocator);

    if (result != VK_SUCCESS) {
        RAYX_ERR << "Cannot create VMA Allocator!";
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, [[maybe_unused]] void* pUserData) {
    RAYX_PROFILE_FUNCTION();

    // Only show Warnings or higher severity bits
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        RAYX_WARN << "(ValidationLayer warn): " << pCallbackData->pMessage;
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        RAYX_ERR << "(ValidationLayer error): " << pCallbackData->pMessage;
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        RAYX_VERB << "(ValidationLayer verb): " << pCallbackData->pMessage;
    }
    // TODO consider also showing INFO messages under some
    // circumstances.

    return VK_FALSE;  // Should return False
}

// Checks if all validation layers are supported.
bool checkValidationLayerSupport() {
    RAYX_PROFILE_FUNCTION();
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // check all validation layers
    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) return false;
    }
    return true;
}

//	This function creates a debug messenger
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    RAYX_PROFILE_FUNCTION();
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

std::vector<const char*> getRequiredExtensions() {
    RAYX_PROFILE_FUNCTION();
    std::vector<const char*> extensions;

    if (enableValidationLayers) {
#ifdef RAYX_DEBUG_MODE
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    }

    return extensions;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    RAYX_PROFILE_FUNCTION();
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanEngine::setupDebugMessenger() {
    RAYX_PROFILE_FUNCTION();
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

}  // namespace RAYX

#endif
