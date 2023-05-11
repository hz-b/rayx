#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

class ImGuiLayer {
  public:
    void init(GLFWwindow* window, ImGui_ImplVulkan_InitInfo&& initInfo, VkFormat format);
    void updateImGui();
    VkCommandBuffer recordImGuiCommands(uint32_t currentImage, const VkFramebuffer framebuffer, const VkExtent2D& extent);
    void cleanupImGui();

    VkCommandBuffer getCommandBuffer(uint32_t index) const { return m_CommandBuffers[index]; }

  private:
    GLFWwindow* m_Window;
    ImGui_ImplVulkan_InitInfo m_InitInfo;

    bool m_LayerEnabled = true;
    bool m_ShowDemoWindow = true;
    bool m_ShowAnotherWindow = false;
    float m_ClearColor[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    VkRenderPass m_RenderPass;
    VkDescriptorPool m_DescriptorPool;
    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    ImGuiIO m_IO;

    void createCommandPool();
    void createCommandBuffers(uint32_t cmdBufferCount);
};