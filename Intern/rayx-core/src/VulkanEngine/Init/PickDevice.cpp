#ifndef NO_VULKAN

#include <optional>

#include "VulkanEngine/VulkanEngine.h"
#define VULKAN_VERSION_3_OR_HIGHER (VK_HEADER_VERSION >= VK_API_VERSION_3_0)

namespace RAYX {

bool isDeviceSuitable(VkPhysicalDevice device);
int rateDevice(VkPhysicalDevice device);
std::vector<const char*> getRequiredDeviceExtensions();
std::optional<uint32_t> findQueueFamilies(VkPhysicalDevice device);

void VulkanEngine::pickDevice() {
    pickPhysicalDevice();
    createLogicalDevice();
    // Fetch best staging size
    STAGING_SIZE = getStagingBufferSize();
}

// scan for devices
std::vector<VkPhysicalDevice> VulkanEngine::getPhysicalDevices() {
    if (!m_Instance) {
        createInstance();
    }
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    if (deviceCount == 0) throw std::runtime_error("failed to find GPUs with Vulkan Support!");

    // create vector of devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());
    return devices;
}

// physical device for computation is chosen
void VulkanEngine::pickPhysicalDevice() {
    RAYX_PROFILE_FUNCTION();
    // search for devices
    auto devices = getPhysicalDevices();
    if (m_deviceID >= (int)devices.size() || m_deviceID < -1) {
        RAYX_ERR << "Device index out of range!";
    }
    if (m_deviceID == -1) {
        // pick fastest device
        m_PhysicalDevice = VK_NULL_HANDLE;
        int currentRating = -1;

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                int rating = rateDevice(device);
                if (rating > currentRating) {
                    m_PhysicalDevice = device;
                    currentRating = rating;
                }
            }
        }
        if (m_PhysicalDevice == VK_NULL_HANDLE) {
            RAYX_ERR << "failed to find a suitable GPU!";
        }
    } else {
        // pick device with given index
        m_PhysicalDevice = devices[m_deviceID];
        if (!isDeviceSuitable(m_PhysicalDevice)) {
            RAYX_ERR << "selected device not suitable!";
        }
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
    RAYX_VERB << "Chose GPU: " << deviceProperties.deviceName;
    RAYX_VERB << "==== Compute Info ====";
    RAYX_VERB << "Compute Max workgroup count: " << deviceProperties.limits.maxComputeWorkGroupCount[1];
    RAYX_VERB << "Compute Max workgroup invocations: " << deviceProperties.limits.maxComputeWorkGroupInvocations;
    RAYX_VERB << "Compute Max workgroup Group size :" << deviceProperties.limits.maxComputeWorkGroupSize[0];
    RAYX_VERB << "Compute Max shared memory size: " << deviceProperties.limits.maxComputeSharedMemorySize;
    RAYX_VERB << "Max pushConstants : " << deviceProperties.limits.maxPushConstantsSize;
    RAYX_VERB << "======================";
}

// checks if given device is suitable for computation
// can be extended later if more specific features are needed
bool isDeviceSuitable(VkPhysicalDevice device) {
    RAYX_PROFILE_FUNCTION();
    // get device properties
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    RAYX_VERB << "Found GPU:" << deviceProperties.deviceName;

    // get device features
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return findQueueFamilies(device).has_value();
}

// Giving scores to each Physical Device
int rateDevice(VkPhysicalDevice device) {
    RAYX_PROFILE_FUNCTION();
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    int score = 1;
    // discrete GPUs are usually faster and get a bonus
    // can be extended to choose the best discrete gpu if multiple are
    // available
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 100000;
    score += deviceProperties.limits.maxComputeSharedMemorySize;
    return score;
}

// Currently returns a  std::vector<const char*>
// Can be further enhanced for propoer Window output with glfw.
std::vector<const char*> getRequiredDeviceExtensions() {
    RAYX_PROFILE_FUNCTION();
    std::vector<const char*> extensions;

    if (enableValidationLayers) {
#if !VULKAN_VERSION_3_OR_HIGHER
        extensions.push_back("VK_KHR_shader_non_semantic_info");
#endif
    }

    // if (enableValidationLayers)
    // {
    // 	extensions.push_back("VK_EXT_descriptor_indexing");
    // }

    return extensions;
}

std::optional<uint32_t> findQueueFamilies(VkPhysicalDevice device) {
    RAYX_PROFILE_FUNCTION();
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            return i;
        }
    }
    return {};
}

/**
 * @brief Create Logical Device and extensions (also Compute Family and Queues)
 *
 */
void VulkanEngine::createLogicalDevice() {
    RAYX_PROFILE_FUNCTION();
    m_computeFamily = findQueueFamilies(m_PhysicalDevice).value();

    // create info about the device queues
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = m_computeFamily;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // create info about the device features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.shaderFloat64 = VK_TRUE;
    deviceFeatures.shaderInt64 = VK_TRUE;
    deviceFeatures.robustBufferAccess = VK_TRUE;

    VkPhysicalDeviceVulkan13Features vk13features{};
    vk13features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vk13features.maintenance4 = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.pNext = &vk13features;

    auto extensions = getRequiredDeviceExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // enable validation layers if possible
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
        RAYX_ERR << "failed to create logical device!";
    }

    vkGetDeviceQueue(m_Device, m_computeFamily, 0, &m_ComputeQueue);
    vkGetDeviceQueue(m_Device, m_computeFamily, 0, &m_TransferQueue);
}
// Create Semaphores for Queue synchronization between commands
void VulkanEngine::createSemaphores() {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Semaphores.computeSemaphore);
    vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Semaphores.transferSemaphore);
}

/**
 * @brief Use to print memories on device.
 * Only in DEBUG
 *
 */
void VulkanEngine::getAllMemories() {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

    RAYX_D_LOG << "Number of memory heaps: " << memoryProperties.memoryHeapCount;
    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++) {
        RAYX_D_LOG << "Memory heap " << i << ": "
                   << "Size: " << memoryProperties.memoryHeaps[i].size << " bytes "
                   << "Flags: " << memoryProperties.memoryHeaps[i].flags;
    }
    RAYX_D_LOG << "Number of memory types: " << memoryProperties.memoryTypeCount;
    for (uint32_t j = 0; j < memoryProperties.memoryTypeCount; j++) {
        RAYX_D_LOG << "Memory type " << j << ": "
                   << "Heap index: " << memoryProperties.memoryTypes[j].heapIndex << " "
                   << "Properties: " << memoryProperties.memoryTypes[j].propertyFlags;
    }
}
/**
 * @brief Get Max Staging Buffer size (Bytes), if not found return default STAGING_SIZE
 *
 * @return VkDeviceSize
 */
VkDeviceSize VulkanEngine::getStagingBufferSize() {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

    VkMemoryPropertyFlags stageMemoryType =
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    for (uint32_t j = 0; j < memoryProperties.memoryTypeCount; j++) {
        auto heapFlag = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[j].heapIndex].flags;
        auto typeProperties = memoryProperties.memoryTypes[j].propertyFlags;
        if (((heapFlag & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) == VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) &&
            ((typeProperties & stageMemoryType) == stageMemoryType)) {
            auto size = memoryProperties.memoryHeaps[memoryProperties.memoryTypes[j].heapIndex].size;
            if (size >= 512 * (1024 * 1024)) {
                size = 512 * (1024 * 1024);  // 512MB as max (for RADEON AMD Integrated)
            } else {
                size -= (1024 * 1024) * 16;  // 16 MB extra headroom removal
            }
            return size;
        }
    }
    // Stage default size
    return DEFAULT_STAGING_SIZE;
}

}  // namespace RAYX

#endif
