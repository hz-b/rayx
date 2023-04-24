#include "Application.h"

#include "utils.h"

Application::Application() {
    m_Window.init(WindowProps("RAY-X", 1920, 1080));
    initVulkan();
    m_ImGuiLayer.init();
}

Application::~Application() {}

void Application::initVulkan() {
    createInstance();
    setupDebugMessenger();
    m_Window.createSurface(m_Instance, m_Allocator, &m_Surface);
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void Application::run() {
    // while (!glfwWindowShouldClose(m_Window)) {
    //     glfwPollEvents();
    //     drawFrame();
    // }
    // vkDeviceWaitIdle(m_Device);
}

void Application::drawFrame() {}

void Application::createInstance() {
    VkResult err;
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    uint32_t extensions_count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    create_info.enabledExtensionCount = extensions_count;
    create_info.ppEnabledExtensionNames = extensions;
    // Create Vulkan Instance without any debug feature
    err = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
    check_vk_result(err);
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void Application::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void Application::pickPhysicalDevice() {
    uint32_t gpu_count;
    VkResult err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, NULL);
    check_vk_result(err);
    IM_ASSERT(gpu_count > 0);

    VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
    err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, gpus);
    check_vk_result(err);

    // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
    // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
    // dedicated GPUs) is out of scope of this sample.
    int use_gpu = 0;
    for (int i = 0; i < (int)gpu_count; i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(gpus[i], &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            use_gpu = i;
            break;
        }
    }

    m_PhysicalDevice = gpus[use_gpu];
    free(gpus);
}

void Application::createLogicalDevice() {
    // Select graphics queue family
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, NULL);
    VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queues);
    for (uint32_t i = 0; i < count; i++)
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_QueueFamily = i;
            break;
        }
    free(queues);
    IM_ASSERT(m_QueueFamily != (uint32_t)-1);

    // Create Logical Device (with 1 queue)
    int device_extension_count = 1;
    const char* device_extensions[] = {"VK_KHR_swapchain"};
    const float queue_priority[] = {1.0f};
    VkDeviceQueueCreateInfo queue_info[1] = {};
    queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[0].queueFamilyIndex = m_QueueFamily;
    queue_info[0].queueCount = 1;
    queue_info[0].pQueuePriorities = queue_priority;
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
    create_info.pQueueCreateInfos = queue_info;
    create_info.enabledExtensionCount = device_extension_count;
    create_info.ppEnabledExtensionNames = device_extensions;
    VkResult err = vkCreateDevice(m_PhysicalDevice, &create_info, m_Allocator, &m_Device);
    check_vk_result(err);
    vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);
}

void Application::createSwapChain() {}

void Application::createImageViews() {}

void Application::createRenderPass() {}

void Application::createGraphicsPipeline() {}

void Application::createFramebuffers() {}

void Application::createCommandPool() {}

void Application::createCommandBuffers() {}

void Application::createSyncObjects() {}
