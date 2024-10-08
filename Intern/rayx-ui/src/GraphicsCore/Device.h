#pragma once

#include <optional>
#include <vector>

#include "Window.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class Device {
  public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    explicit Device(Window& window, std::optional<int> deviceIndex = std::nullopt);
    ~Device();

    // Not copyable or movable
    Device(const Device&) = delete;
    void operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    VkCommandPool getCommandPool() const { return m_CommandPool; }
    VkDevice device() const { return m_Device; }
    VkPhysicalDevice physicalDevice() const { return m_PhysicalDevice; }
    VkInstance instance() const { return m_Instance; }
    VkSurfaceKHR surface() const { return m_Surface; }
    VkQueue graphicsQueue() const { return m_GraphicsQueue; }
    VkQueue presentQueue() const { return m_PresentQueue; }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
    SwapChainSupportDetails getSwapChainSupport() const { return querySwapChainSupport(m_PhysicalDevice); }
    QueueFamilyIndices findPhysicalQueueFamilies() const { return findQueueFamilies(m_PhysicalDevice); }

    // Buffer Helper Functions
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                      VkDeviceMemory& bufferMemory) const;
    VkCommandBuffer beginSingleTimeCommands() const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) const;

    void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;

  private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice(std::optional<uint32_t> deviceID);
    void createLogicalDevice();
    void createCommandPool();

    // helper functions
    bool isDeviceSuitable(VkPhysicalDevice device) const;
    std::vector<const char*> getRequiredExtensions() const;
    bool checkValidationLayerSupport() const;
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void hasGflwRequiredInstanceExtensions() const;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    Window& m_Window;
    VkCommandPool m_CommandPool;

    VkDevice m_Device;
    VkSurfaceKHR m_Surface;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;

    VkPhysicalDeviceProperties m_Properties;

    const std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};