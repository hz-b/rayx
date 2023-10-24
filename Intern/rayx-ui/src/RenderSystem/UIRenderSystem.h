#pragma once

#include <imgui.h>

#include <string>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/Swapchain.h"

class UIRenderSystem {
  public:
    UIRenderSystem(const Window& window, const Device& device, VkFormat imageFormat, VkFormat depthFormat, uint32_t imageCount);
    UIRenderSystem(const UIRenderSystem&) = delete;
    UIRenderSystem& operator=(const UIRenderSystem&) = delete;
    ~UIRenderSystem();

    void setupUI(CameraController& camController, FrameInfo& frameInfo);
    void render(VkCommandBuffer commandBuffer);

    VkClearValue getClearValue() const { return {m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]}; }

  private:
    const Window& m_Window;
    const Device& m_Device;

    float m_ClearColor[4] = {0.01f, 0.01f, 0.01f, 1.00f};

    bool m_useLargeFont = false;
    ImFont* m_smallFont;
    ImFont* m_largeFont;

    VkRenderPass m_RenderPass;
    VkDescriptorPool m_DescriptorPool;
    ImGuiIO m_IO;

    void showSceneEditorWindow(FrameInfo& frameInfo, CameraController& camController);
    void showSettingsWindow();
    void showBeamlineOutlineWindow(FrameInfo& frameInfo);
};