#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

class ImGuiLayer {
  public:
    void init(GLFWwindow* window, ImGui_ImplVulkan_InitInfo&& initInfo, VkRenderPass renderPass);
    void updateImGui();
    void drawImGui();
    void cleanupImGui();

    void createCommandPool();
    void createCommandBuffers();

  private:
    ImGui_ImplVulkan_InitInfo m_InitInfo;

    bool m_ImGuiEnabled = true;
    bool m_ImGuiShowDemoWindow = true;
    bool m_ImGuiShowAnotherWindow = false;
    float m_ImGuiClearColor[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    VkDescriptorPool m_DescriptorPool;
    VkCommandPool m_CommandPool;
    VkCommandBuffer m_CommandBuffer;
};