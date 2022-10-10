#include <optional>

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

bool isDeviceSuitable(VkPhysicalDevice device);
int rateDevice(VkPhysicalDevice device);
std::vector<const char*> getRequiredDeviceExtensions();
std::optional<uint32_t> findQueueFamilies(VkPhysicalDevice device);

void VulkanEngine::pickDevice() {
    pickPhysicalDevice();
    createLogicalDevice();
}

// physical device for computation is chosen
void VulkanEngine::pickPhysicalDevice() {
    RAYX_PROFILE_FUNCTION();
    // search for devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    if (deviceCount == 0) throw std::runtime_error("failed to find GPUs with Vulkan Support!");

    // create vector of devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

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
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
    RAYX_VERB << "Chose GPU: " << deviceProperties.deviceName;
    RAYX_VERB << "==== Compute Info ====";
    RAYX_VERB << "Compute Max workgroup count: " << deviceProperties.limits.maxComputeWorkGroupCount[1];
    RAYX_VERB << "Compute Max workgroup invocations: " << deviceProperties.limits.maxComputeWorkGroupInvocations;
    RAYX_VERB << "Compute Max workgroup Group size :" << deviceProperties.limits.maxComputeWorkGroupSize[0];
    RAYX_VERB << "Compute Max shared memory size: " << deviceProperties.limits.maxComputeSharedMemorySize;
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
#ifdef RAYX_DEBUG_MODE
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

// creates a logical device to communicate with the physical device
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
    deviceFeatures.shaderFloat64 = true;
    deviceFeatures.shaderInt64 = true;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

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
        RAYX_WARN << "Failed to create instance!";
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_Device, m_computeFamily, 0, &m_ComputeQueue);
}

}  // namespace RAYX
