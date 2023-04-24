#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <memory>

#include "ImGuiLayer.h"
#include "Window.h"

// const char* VALIDATION_LAYERS = "VK_LAYER_KHRONOS_validation";
// const char* DEVICE_EXTENSIONS = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Application {
  public:
    Application();
    ~Application();

    void run();

  private:
    Window m_Window;
    VkAllocationCallbacks* m_Allocator;
    VkInstance m_Instance;
    VkSurfaceKHR m_Surface;
    VkDebugUtilsMessengerEXT m_DebugMessenger;


    ImGuiLayer m_ImGuiLayer;

    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    uint32_t m_QueueFamily;
    VkQueue m_Queue;
    VkDebugReportCallbackEXT m_DebugReport;
    VkPipelineCache m_PipelineCache;
    VkDescriptorPool m_DescriptorPool;

    void initVulkan();

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    void drawFrame();
};