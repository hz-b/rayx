#pragma once

#include <imgui.h>

#include <filesystem>
#include <string>

#include "FrameInfo.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/Swapchain.h"
#include "RenderObject.h"

/**
 * UI Parameters such as toggles, paths, etc.
 */
struct UIRayInfo {
    bool displayRays;
    bool raysChanged;
    bool cacheChanged;
    bool renderAllRays;
    size_t amountOfRays;
    size_t maxAmountOfRays;
};
struct UIParameters {
    CameraController& camController;
    std::filesystem::path rmlPath;
    bool pathChanged;
    float frameTime;
    UIRayInfo rayInfo;
};

class UIRenderSystem {
  public:
    UIRenderSystem(const Window& window, const Device& device, VkFormat imageFormat, VkFormat depthFormat, uint32_t imageCount);
    UIRenderSystem(const UIRenderSystem&) = delete;
    UIRenderSystem& operator=(const UIRenderSystem&) = delete;
    ~UIRenderSystem();

    void setupUI(UIParameters& uiParams, std::vector<RenderObject>& rObjects, std::vector<glm::dvec3>& rSourcePositions);
    void render(VkCommandBuffer commandBuffer);

    VkClearValue getClearValue() const {
        VkClearValue v;
        for (int i = 0; i < 4; i++) {
            v.color.float32[i] = m_ClearColor[i];
        }
        return v;
    }

    // Simple TreeNode
    struct TreeNode {
        std::string name;
        std::string type;
        std::string category;  // Add this field for category
        int index = -1;
        std::vector<TreeNode> children;

        TreeNode(const std::string& name, const std::string& type = "", const std::string& category = "")
            : name(name), type(type), category(category) {}
    };

  private:
    const Window& m_Window;
    const Device& m_Device;

    float m_ClearColor[4] = {0.01f, 0.01f, 0.01f, 1.00f};

    bool m_useLargeFont = false;
    ImFont* m_smallFont;
    ImFont* m_largeFont;
    std::unique_ptr<TreeNode> m_pTreeRoot;

    VkRenderPass m_RenderPass;
    VkDescriptorPool m_DescriptorPool;
    ImGuiIO m_IO;

    int m_lightSourceIndex = 0;
    int m_opticalElementIndex = 0;

    void showSceneEditorWindow(UIParameters& uiParams);
    void showSettingsWindow();
    void showHotkeysWindow();

    void buildTreeFromXMLNode(rapidxml::xml_node<>* node, UIRenderSystem::TreeNode& treeNode);
    void renderImGuiTreeFromRML(const std::filesystem::path& filename, CameraController& camController, std::vector<RenderObject>& rObjects,
                                std::vector<glm::dvec3>& rSourcePositions);
    void showBeamlineOutlineWindow(UIParameters& uiParams, std::vector<RenderObject>& rObjects, std::vector<glm::dvec3>& rSourcePositions);
};

void renderImGuiTree(const UIRenderSystem::TreeNode& treeNode, CameraController& camController);
