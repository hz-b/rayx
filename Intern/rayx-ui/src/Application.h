#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "GraphicsCore/Descriptors.h"
#include "GraphicsCore/Renderer.h"
#include "Scene.h"

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Window;
class Device;

class Application {
  public:
    Application(uint32_t width, uint32_t height, const char* name);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

  private:
    // --- Order matters ---
    Window m_Window;
    Device m_Device;
    Renderer m_Renderer;

    std::unique_ptr<DescriptorPool> m_DescriptorPool{nullptr};
    // ----------------------
    Scene m_Scene;
};