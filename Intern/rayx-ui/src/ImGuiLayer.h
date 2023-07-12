#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <glm/gtx/transform.hpp>

#include "Device.h"
#include "Swapchain.h"

struct CameraSettings {
    float FOV = 90.0f;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, -2.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);  // Add Up vector
    float near = 0.1f;
    float far = 100.0f;

    glm::mat4 getViewMatrix() const { return glm::lookAt(position, target, up); }

    glm::mat4 getProjectionMatrix(float aspectRatio) const { return glm::perspective(glm::radians(FOV), aspectRatio, near, far); }
};

class ImGuiLayer {
  public:
    ImGuiLayer(const Window& window, const Device& device, const SwapChain& swapchain);
    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;
    ~ImGuiLayer();

    void updateImGui();
    VkCommandBuffer recordImGuiCommands(uint32_t currentImage, const VkFramebuffer framebuffer, const VkExtent2D& extent);

    VkCommandBuffer getCommandBuffer(uint32_t index) const { return m_CommandBuffers[index]; }
    VkClearValue getClearValue() const { return {m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]}; }
    CameraSettings getCameraSettings() const { return m_Camera; }

  private:
    const Window& m_Window;
    const Device& m_Device;
    const SwapChain& m_SwapChain;

    bool m_LayerEnabled = true;
    bool m_ShowDemoWindow = false;
    float m_ClearColor[4] = {0.01f, 0.01f, 0.01f, 1.00f};

    VkRenderPass m_RenderPass;
    VkDescriptorPool m_DescriptorPool;
    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    ImGuiIO m_IO;

    CameraSettings m_Camera;

    void createCommandPool();
    void createCommandBuffers(uint32_t cmdBufferCount);
};