#pragma once

#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

class Window;

class ImGuiLayer {
  public:
    ImGuiLayer();
    ~ImGuiLayer();

    void init();
    void setupContext(ImGui_ImplVulkan_InitInfo* init_info, GLFWwindow* window);

  private:
    ImGui_ImplVulkanH_Window m_MainWindowData;
    int m_minImageCount;
    bool m_swapChainRebuild;
};