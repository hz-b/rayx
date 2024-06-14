#include "UIHandler.h"

#undef APIENTRY
#include <portable-file-dialogs.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <fstream>
#include <rapidxml.hpp>

#include "Colors.h"
#include "CanonicalizePath.h"
#include "RayProcessing.h"

extern bool isSceneWindowHovered;

void checkVkResult(VkResult result, const char* message) {
    if (result != VK_SUCCESS) {
        printf("%s\n", message);
        exit(1);
    }
}

// ---- UIHandler ----
UIHandler::UIHandler(const Window& window, const Device& device, VkFormat imageFormat, VkFormat depthFormat, uint32_t imageCount)
    : m_Window(window), m_Device(device) {
    // Create descriptor pool for IMGUI
    VkDescriptorPoolSize poolSizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
    poolInfo.poolSizeCount = (uint32_t)std::size(poolSizes);
    poolInfo.pPoolSizes = poolSizes;

    if (vkCreateDescriptorPool(m_Device.device(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create imgui descriptor pool");
    }

    // Create render pass for IMGUI
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = imageFormat;  // same as in main render pass
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // same as in main render pass
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;    // same as in main render pass

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = depthFormat;  // same as in main render pass
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // same as in main render pass
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;  // same as in main render pass

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = static_cast<uint32_t>(attachments.size());
    info.pAttachments = attachments.data();
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    if (vkCreateRenderPass(m_Device.device(), &info, nullptr, &m_RenderPass) != VK_SUCCESS) {
        throw std::runtime_error("Could not create Dear ImGui's render pass");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_IO = ImGui::GetIO();

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = m_Device.instance();
    initInfo.PhysicalDevice = m_Device.physicalDevice();
    initInfo.Device = m_Device.device();
    initInfo.QueueFamily = m_Device.findPhysicalQueueFamilies().graphicsFamily;
    initInfo.Queue = m_Device.graphicsQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = m_DescriptorPool;
    initInfo.RenderPass = m_RenderPass;
    initInfo.Allocator = nullptr;
    initInfo.MinImageCount = imageCount;
    initInfo.ImageCount = imageCount;
    initInfo.CheckVkResultFn = nullptr;

    ImGui_ImplGlfw_InitForVulkan(m_Window.window(), true);
    ImGui_ImplVulkan_Init(&initInfo);

    // Upload fonts
    {
        // Setup style
        const std::filesystem::path fontPath = getExecutablePath() / "Assets/fonts/Roboto-Regular.ttf";
        m_fonts.push_back(m_IO.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 8.0f));
        m_fonts.push_back(m_IO.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16.0f));
        m_fonts.push_back(m_IO.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 32.0f));
        m_fonts.push_back(m_IO.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 48.0f));
        m_fonts.push_back(m_IO.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 64.0f));
        m_IO.Fonts->Build();

        ImGui_ImplVulkan_CreateFontsTexture();
    }
    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    applyDarkTheme();

    // Enable docking
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

UIHandler::~UIHandler() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(m_Device.device(), m_DescriptorPool, nullptr);
    vkDestroyRenderPass(m_Device.device(), m_RenderPass, nullptr);
}

void UIHandler::beginUIRender() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/**
 * @brief
 *
 * @param uiParams
 * @param rObjects
 */
void UIHandler::setupUI(UIParameters& uiParams, std::vector<RAYX::DesignElement>& elements, std::vector<glm::dvec3>& rSourcePositions) {
    ImFont* currentFont;
    float adjustedScale;
    if (m_oldScale != m_scale) {
        ImGuiStyle style = ImGui::GetStyle();
        style.ScaleAllSizes(m_scale / m_oldScale);
        style.ScrollbarSize = 15.0f;
        m_oldScale = m_scale;
    }
    if (m_scale <= 0.5f) {
        currentFont = m_fonts[0];
        adjustedScale = m_scale / 0.5f;
    } else if (m_scale <= 1.0f) {
        currentFont = m_fonts[1];
        adjustedScale = m_scale / 1.0f;
    } else if (m_scale <= 2.0f) {
        currentFont = m_fonts[2];
        adjustedScale = m_scale / 2.0f;
    } else if (m_scale <= 3.0f) {
        currentFont = m_fonts[3];
        adjustedScale = m_scale / 3.0f;
    } else if (m_scale <= 4.0f) {
        currentFont = m_fonts[4];
        adjustedScale = m_scale / 4.0f;
    } else {
        currentFont = m_fonts[4];
        adjustedScale = 1.0f;
    }
    ImGui::GetIO().FontGlobalScale = adjustedScale;
    ImGui::PushFont(currentFont);

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 0.0f));
    ImGui::Begin("Root", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // Dockspace
    auto& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("Root");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        static bool first_time = true;
        if (first_time) {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, nullptr, &dockspace_id);
            auto dock_id_right_top = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.5f, nullptr, &dock_id_right);
            auto dock_id_right_bottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.5f, nullptr, &dock_id_right);

            ImGui::DockBuilderDockWindow("Render View", dockspace_id);
            ImGui::DockBuilderDockWindow("Properties Manager", dock_id_right_top);
            ImGui::DockBuilderDockWindow("Settings", dock_id_right_top);
            ImGui::DockBuilderDockWindow("Beamline Outline", dock_id_right_bottom);
            ImGui::DockBuilderDockWindow("Hotkeys", dock_id_right_bottom);
            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    // Render View
    // Push zero padding for the window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("Render View");  // Window with no padding
    ImVec2 size = ImGui::GetContentRegionAvail();
    if (size.x > 0 && size.y > 0) {
        uiParams.sceneExtent = {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
        if (uiParams.sceneDescriptorSet != VK_NULL_HANDLE) {
            ImGui::Image((ImTextureID)uiParams.sceneDescriptorSet, ImVec2(size.x, size.y), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1),
                         ImVec4(0, 0, 0, 0));
        }
    }
    isSceneWindowHovered = ImGui::IsWindowHovered();
    ImGui::End();

    // Pop the style variable to restore default padding
    ImGui::PopStyleVar();

    showSceneEditorWindow(uiParams);
    showMissingFilePopupWindow(uiParams);
    showSimulationSettingsPopupWindow(uiParams);
    showSettingsWindow();
    m_BeamlineOutliner.showBeamlineOutlineWindow(uiParams, elements, rSourcePositions);
    showHotkeysWindow();
    ImGui::End();

    // setting focus to the beamline outline window
    static bool first_time = true;
    if (first_time) {
        first_time = false;
        ImGui::SetWindowFocus("Beamline Outline");
    }

    ImGui::PopFont();
}

void UIHandler::endUIRender(VkCommandBuffer commandBuffer) {
    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();

    // Avoid rendering when minimized
    if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f) {
        return;
    }

    // Create and submit command buffers
    ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
}

void UIHandler::showSceneEditorWindow(UIParameters& uiParams) {
    ImGui::Begin("Properties Manager");

    if (ImGui::Button("Open File Dialog")) {
        const std::vector<std::string> results =
            pfd::open_file("Open Beamline File", "", {"RML Beamline Files", "*.rml *.xml"}, pfd::opt::none).result();
        if (!results.empty()) {
            const std::string rmlPath = results[0];
#ifndef NO_H5
            std::string rayFilePathH5 = rmlPath.substr(0, rmlPath.size() - 4) + ".h5";
            uiParams.showH5NotExistPopup = !std::filesystem::exists(rayFilePathH5);
#else
            std::string rayFilePathCSV = rmlPath.substr(0, rmlPath.size() - 4) + ".csv";
            uiParams.showH5NotExistPopup = !std::filesystem::exists(rayFilePathCSV);
#endif
            m_showRMLNotExistPopup = rmlPath.substr(rmlPath.size() - 4, 4) != ".rml" || !std::filesystem::exists(rmlPath);

            if (m_showRMLNotExistPopup) {
                uiParams.rmlReady = false;
            } else {
                uiParams.h5Ready = !uiParams.showH5NotExistPopup && m_loadh5withRML;
                uiParams.rmlReady = true;
                uiParams.rmlPath = rmlPath;
            }
        } else {
            // Add specific handling when the file dialog is cancelled or an error occurs
            RAYX_LOG << ("User did not select a file or an error occurred.");
            // TODO: Popup
        }
    }
    if (uiParams.rmlPath != "") {
        ImGui::SameLine();
        if (ImGui::Button("Trace current file")) {
            uiParams.showH5NotExistPopup = false;
            m_showRMLNotExistPopup = false;
            uiParams.runSimulation = true;
        }
    } else {
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Button("Trace current file");
        ImGui::EndDisabled();
    }

    ImGui::Text("Background");
    ImGui::ColorEdit3("Color", (float*)&m_ClearColor);

    ImGui::Separator();
    uiParams.camController.displaySettings();
    ImGui::Separator();
    if (!uiParams.rmlPath.empty() && uiParams.rayInfo.raysLoaded) {
        size_t tempAmountOfRays = uiParams.rayInfo.amountOfRays;
        bool tempRenderAllRays = uiParams.rayInfo.renderAllRays;

        displayFilterSlider(uiParams.rayInfo.amountOfRays, uiParams.rayInfo.maxAmountOfRays, uiParams.rayInfo.displayRays,
                            uiParams.rayInfo.renderAllRays);

        if (tempAmountOfRays != uiParams.rayInfo.amountOfRays) {
            uiParams.rayInfo.raysChanged = true;
        }
        if (tempRenderAllRays != uiParams.rayInfo.renderAllRays) {
            uiParams.rayInfo.cacheChanged = true;
        }
    }
    ImGui::Text("Application average %.6f ms/frame", uiParams.frameTime * 1000.0f);

    ImGui::End();
}

void UIHandler::applyDarkTheme() {
    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);          // Brighter light grey text
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);  // Slightly brighter grey for disabled text

    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.45f, 0.47f, 1.00f);

    style->Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.80f, 1.00f);
    style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.22f, 0.66f, 1.00f);

    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.94f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.40f, 0.40f, 0.40f, 0.80f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.78f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.12f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);

    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.47f);

    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.53f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 0.85f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.85f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.35f, 0.90f, 1.00f, 0.80f);

    style->Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.25f, 0.25f, 0.64f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.28f, 0.28f, 0.88f, 1.00f);

    style->Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 0.86f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 0.15f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.67f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.95f);

    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.55f, 0.55f, 0.55f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.35f, 0.85f, 1.00f, 0.43f);
}

void UIHandler::applyLightTheme() {
    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_Text] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);          // Dark grey for high readability
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);  // Disabled text remains subdued

    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);

    style->Colors[ImGuiCol_Tab] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(0.85f, 0.85f, 1.00f, 1.00f);
    style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.87f, 0.87f, 1.00f, 1.00f);

    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);       // Very light grey background
    style->Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);       // White popups with slight transparency
    style->Colors[ImGuiCol_Border] = ImVec4(0.88f, 0.88f, 0.88f, 0.30f);        // Light grey border
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);  // No border shadow

    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);         // Frame background slightly lighter than window
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.96f, 0.96f, 0.96f, 0.78f);  // Light grey on hover
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);   // Active frame background

    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.96f, 0.96f, 0.96f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);

    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.96f, 0.96f, 0.96f, 0.47f);

    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.96f, 0.96f, 0.96f, 0.53f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.90f, 0.90f, 0.90f, 0.85f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 0.85f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);

    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 0.50f, 1.00f, 0.80f);  // Blue for check marks

    style->Colors[ImGuiCol_Button] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);          // Button background
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);  // Light hover effect
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);   // Active button effect

    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.8f, 0.8f, 0.8f, 0.64f);         // Slider handle
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.8f, 1.00f, 1.00f);  // Slider handle when active

    style->Colors[ImGuiCol_Header] = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.98f, 0.98f, 0.86f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.96f, 0.96f, 0.96f, 0.15f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.98f, 0.98f, 0.98f, 0.67f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);

    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.70f, 0.70f, 0.70f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.80f, 0.80f, 0.80f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 1.00f, 0.35f);  // Light blue for text selection
}

void UIHandler::showSettingsWindow() {
    ImGui::Begin("Settings");

    ImGui::SliderFloat("Scale", &m_scale, 0.1f, 4.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
    if (ImGui::Button("Toggle Theme")) {
        m_isDarkTheme = !m_isDarkTheme;  // Toggle theme
        if (m_isDarkTheme) {
            applyDarkTheme();
        } else {
            applyLightTheme();
        }
    }

    // checkbox
    ImGui::Checkbox("Load h5 with RML", &m_loadh5withRML);

    ImGui::End();
}

void UIHandler::showHotkeysWindow() {
    ImGui::Begin("Hotkeys");

    ImGui::Text("Keyboard Hotkeys:");
    ImGui::Separator();
    ImGui::Text("ESC: Close the window");
    ImGui::Text("F11: Fullscreen mode");
    ImGui::Text("F10: Windowed mode");
    ImGui::Separator();
    ImGui::Text("W, A, S, D: Camera movement (along plane)");
    ImGui::Text("Q: Move camera downward");
    ImGui::Text("E: Move camera upward");
    ImGui::Separator();
    ImGui::Text("Hold SHIFT for faster movement");

    ImGui::End();
}

void UIHandler::showMissingFilePopupWindow(UIParameters& uiParams) {
    if (m_showRMLNotExistPopup) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));

        ImGui::OpenPopup("File Not Found");
        if (ImGui::BeginPopupModal("File Not Found", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (m_showRMLNotExistPopup) {
                ImGui::Text("RML file does not exist or is not valid.");
            }

            if (ImGui::Button("Okay", ImVec2(120 * m_scale, 0))) {  // Make the button a bit larger
                uiParams.showH5NotExistPopup = false;
                m_showRMLNotExistPopup = false;
                uiParams.runSimulation = false;
                uiParams.rmlReady = false;   // Do not start the simulation
                ImGui::CloseCurrentPopup();  // Close the popup when an option is selected
            }

            ImGui::EndPopup();
        }
    }
}

void UIHandler::showSimulationSettingsPopupWindow(UIParameters& uiParams) {
    if (uiParams.runSimulation && !uiParams.simulationSettingsReady) {
        ImGui::OpenPopup("Simulation Settings");

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Simulation Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Checkbox("Sequential", &uiParams.simulationInfo.sequential);
            ImGui::InputScalar("Max Batch Size", ImGuiDataType_U32, &uiParams.simulationInfo.maxBatchSize);

            // Prepare device combo box
            std::vector<const char*> deviceItems;
            for (const auto& device : uiParams.simulationInfo.availableDevices) {
                deviceItems.push_back(device.c_str());
            }

            const char* tracerItems[] = {"CPU Tracer", "VULKAN Tracer"};
            ImGui::Combo("Tracer", reinterpret_cast<int*>(&uiParams.simulationInfo.tracer), tracerItems, IM_ARRAYSIZE(tracerItems));

            // Device selection combo box
            if (uiParams.simulationInfo.tracer == 1) {  // If not CPU Tracer, enable device selection
                ImGui::Combo("Device", reinterpret_cast<int*>(&uiParams.simulationInfo.deviceIndex), &deviceItems[0],
                             static_cast<int>(deviceItems.size()));
            } else {
                ImGui::BeginDisabled();  // Disable combo box if CPU Tracer is selected
                ImGui::Combo("Device", reinterpret_cast<int*>(&uiParams.simulationInfo.deviceIndex), &deviceItems[0],
                             static_cast<int>(deviceItems.size()));
                ImGui::EndDisabled();
            }
            // startEventID selection
            // ImGui::InputInt("Start Event ID", &uiParams.simulationInfo.startEventID);

            // maxEvents selection
            ImGui::InputScalar("Max Events", ImGuiDataType_U32, &uiParams.simulationInfo.maxEvents);

            if (!uiParams.simulationInfo.fixedSeed) {
                ImGui::BeginDisabled();
                ImGui::InputScalar("Seed", ImGuiDataType_U32, &uiParams.simulationInfo.seed);
                ImGui::EndDisabled();
            } else {
                ImGui::InputScalar("Seed", ImGuiDataType_U32, &uiParams.simulationInfo.seed);
            }
            ImGui::SameLine();
            ImGui::Checkbox("Fixed Seed", &uiParams.simulationInfo.fixedSeed);

            ImGui::Separator();

            // Push buttons to the bottom
            float totalSpace = ImGui::GetContentRegionAvail().y;
            float buttonHeight = 40.0f;
            ImGui::Dummy(ImVec2(0.0f, totalSpace - buttonHeight - ImGui::GetStyle().ItemSpacing.y * 2));

            // Centering buttons
            float windowWidth = ImGui::GetWindowSize().x;
            float buttonsWidth = /* 2* */ 120.0f + ImGui::GetStyle().ItemSpacing.x;  // Width of two buttons and spacing
            ImGui::SetCursorPosX((windowWidth - buttonsWidth) / 2.0f);

            if (uiParams.simulationInfo.deviceIndex >= static_cast<unsigned int>(uiParams.simulationInfo.availableDevices.size())) {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Start Simulation")) {
                uiParams.simulationSettingsReady = true;
                ImGui::CloseCurrentPopup();
            }

            if (uiParams.simulationInfo.deviceIndex >= static_cast<unsigned int>(uiParams.simulationInfo.availableDevices.size())) {
                ImGui::EndDisabled();
            }

            // ImGui::SameLine();

            // if (ImGui::Button("Cancel", ImVec2(120, buttonHeight))) {
            //     uiParams.runSimulation = false;
            //     ImGui::CloseCurrentPopup();
            // }

            ImGui::EndPopup();
        }
    }
}
