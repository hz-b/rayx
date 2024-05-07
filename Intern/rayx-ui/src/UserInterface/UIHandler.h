#pragma once

#include <imgui.h>

#include <string>

#include "Beamline/OpticalElement.h"
#include "BeamlineOutliner.h"
#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/Swapchain.h"
#include "Settings.h"

class UIHandler {
  public:
    UIHandler(const Window& window, const Device& device, VkFormat imageFormat, VkFormat depthFormat, uint32_t imageCount);
    UIHandler(const UIHandler&) = delete;
    UIHandler& operator=(const UIHandler&) = delete;
    ~UIHandler();

    void beginUIRender();
    void setupUI(UIParameters& uiParams, std::vector<RAYX::DesignElement>& elements, std::vector<glm::dvec3>& rSourcePositions);
    void endUIRender(VkCommandBuffer commandBuffer);

    VkClearValue getClearValue() const {
        VkClearValue v;
        for (int i = 0; i < 4; i++) {
            v.color.float32[i] = m_ClearColor[i];
        }
        return v;
    }

  private:
    const Window& m_Window;
    const Device& m_Device;

    float m_ClearColor[4] = {0.01f, 0.01f, 0.01f, 1.00f};

    float m_scale = 1.0f;
    float m_oldScale = 1.0f;
    std::vector<ImFont*> m_fonts;

    bool m_showRMLNotExistPopup = false;
    bool m_loadh5withRML = true;

    VkRenderPass m_RenderPass;
    VkDescriptorPool m_DescriptorPool;
    ImGuiIO m_IO;

    BeamlineOutliner m_BeamlineOutliner;

    void showSceneEditorWindow(UIParameters& uiParams);
    void showSettingsWindow();
    void showHotkeysWindow();
    void showMissingFilePopupWindow(UIParameters& uiParams);
    void showSimulationSettingsPopupWindow(UIParameters& uiParams);
};
