#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <glm/gtx/transform.hpp>

struct CameraSettings {
    float FOV = 90.0f;
    float Pitch = 0.0f;
    float Yaw = 90.0f;
    float Roll = 0.0f;
    float Near = 0.1f;
    float Far = 100.0f;
    glm::vec3 Position = glm::vec3(0.0f, 0.0f, -2.0f);

    // Helper functions to calculate model, view and projection matrices
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(Roll), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        return model;
    }

    glm::mat4 getViewMatrix() const {
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, Position);
        view = glm::rotate(view, glm::radians(Roll), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::rotate(view, glm::radians(Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        return view;
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const { return glm::perspective(glm::radians(FOV), aspectRatio, Near, Far); }
};

class ImGuiLayer {
  public:
    void init(GLFWwindow* window, ImGui_ImplVulkan_InitInfo&& initInfo, VkFormat format);
    void updateImGui();
    VkCommandBuffer recordImGuiCommands(uint32_t currentImage, const VkFramebuffer framebuffer, const VkExtent2D& extent);
    void cleanupImGui();

    VkCommandBuffer getCommandBuffer(uint32_t index) const { return m_CommandBuffers[index]; }
    VkClearValue getClearValue() const { return {m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]}; }
    CameraSettings getCameraSettings() const { return m_Camera; }

  private:
    GLFWwindow* m_Window;
    ImGui_ImplVulkan_InitInfo m_InitInfo;

    bool m_LayerEnabled = true;
    bool m_ShowDemoWindow = false;
    float m_ClearColor[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    VkRenderPass m_RenderPass;
    VkDescriptorPool m_DescriptorPool;
    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    ImGuiIO m_IO;

    CameraSettings m_Camera;

    void createCommandPool();
    void createCommandBuffers(uint32_t cmdBufferCount);
};