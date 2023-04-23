#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <memory>

#include "ImGuiLayer.h"
#include "Renderer.h"
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
    ImGuiLayer m_ImGuiLayer;
    Renderer m_Renderer;

    VkSurfaceKHR m_Surface;

    void initWindow();
    void initRenderer();
    void initImGuiLayer();

    // Helper functions for InitVulkan
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void selectGraphicsQueueFamily();
    void createLogicalDevice();
    void createDescriptorPool();
    void createSurface();
    void createFramebuffers();

    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    // Helper functions for InitVulkan
    void drawFrame();
};