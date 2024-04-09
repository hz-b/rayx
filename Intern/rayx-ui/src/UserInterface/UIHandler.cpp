#include "UIHandler.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <nfd.h>

#include <fstream>
#include <rapidxml.hpp>

#include "CanonicalizePath.h"
#include "RayProcessing.h"

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
    poolInfo.maxSets = 1000;
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

    ImGui::StyleColorsDark();

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
        m_smallFont = m_IO.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16.0f);
        m_largeFont = m_IO.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 24.0f);

        ImGui_ImplVulkan_CreateFontsTexture();
    }
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
void UIHandler::setupUI(UIParameters& uiParams, std::vector<RAYX::OpticalElement>& elemets, std::vector<glm::dvec3>& rSourcePositions) {
    if (m_useLargeFont) {
        ImGui::PushFont(m_largeFont);
    } else {
        ImGui::PushFont(m_smallFont);
    }

    showSceneEditorWindow(uiParams);
    showMissingFilePopupWindow(uiParams);
    showSimulationSettingsPopupWindow(uiParams);
    showSettingsWindow();
    showHotkeysWindow();
    m_BeamlineOutliner.showBeamlineOutlineWindow(uiParams, elemets, rSourcePositions);

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
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(450, 450), ImGuiCond_Once);

    ImGui::Begin("Properties Manager");

    if (ImGui::Button("Open File Dialog")) {
        nfdchar_t* outPath;
        constexpr uint32_t filterCount = 1;
        nfdfilteritem_t filterItem[filterCount] = {{"RML Files", "rml, xml"}};
        nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, filterCount, NULL);
        if (result == NFD_OKAY) {
            std::string rmlPath = outPath;

#ifndef NO_H5
            std::string rayFilePathH5 = rmlPath.substr(0, rmlPath.size() - 4) + ".h5";
            m_showH5NotExistPopup = !std::filesystem::exists(rayFilePathH5);
#else
            std::string rayFilePathCSV = rmlPath.substr(0, rmlPath.size() - 4) + ".csv";
            m_showH5NotExistPopup = !std::filesystem::exists(rayFilePathCSV);
#endif
            m_showRMLNotExistPopup = rmlPath.substr(rmlPath.size() - 4, 4) != ".rml" || !std::filesystem::exists(rmlPath);

            if (m_showRMLNotExistPopup) {
                uiParams.rmlReady = false;
            } else {
                if (m_showH5NotExistPopup) {
                    uiParams.h5Ready = false;
                    m_pathValidState = false;
                } else {
                    uiParams.h5Ready = true;
                    m_pathValidState = true;
                    uiParams.rmlReady = true;
                }
                uiParams.rmlPath = outPath;
            }
        } else if (result == NFD_CANCEL) {
            puts("User pressed cancel.");
        } else {
            printf("Error: %s\n", NFD_GetError());
        }
    }
    if (uiParams.rmlPath != "") {
        ImGui::SameLine();
        if (ImGui::Button("Retrace current file")) {
            m_showH5NotExistPopup = false;
            m_showRMLNotExistPopup = false;
            uiParams.rmlReady = true;
            uiParams.runSimulation = true;
        }
    } else {
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Button("Retrace current file");
        ImGui::EndDisabled();
    }

    ImGui::Text("Background");
    ImGui::ColorEdit3("Color", (float*)&m_ClearColor);

    ImGui::Separator();
    uiParams.camController.displaySettings();
    ImGui::Separator();
    if (!uiParams.rmlPath.empty() && m_pathValidState) {
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

void UIHandler::showSettingsWindow() {
    ImGui::SetNextWindowPos(ImVec2(0, 450), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);

    ImGui::Begin("Settings");

    ImGui::Checkbox("Large Font", &m_useLargeFont);

    ImGui::End();
}

void UIHandler::showHotkeysWindow() {
    ImGui::SetNextWindowPos(ImVec2(0, 550), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(450, 210), ImGuiCond_Once);

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
    if (m_showH5NotExistPopup || m_showRMLNotExistPopup) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Always);  // Set size

        ImGui::OpenPopup("File Not Found");
        if (ImGui::BeginPopupModal("File Not Found", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            // Scale up font size
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

            if (m_showRMLNotExistPopup) {
                ImGui::Text("RML file does not exist or is not valid.");
            } else {
                ImGui::Text("The H5 file does not exist.");
            }
            ImGui::Spacing();
            ImGui::Text("Do you want to run the simulation now?");  // Ask the user if they want to run the simulation now
            ImGui::Spacing();

            // Calculate center position for buttons. Assuming 120 pixels width for each button and 10 pixels space between them
            constexpr float windowWidth = 400;         // Popup window width
            constexpr float buttonsWidth = 240;        // Total buttons width (120 * 2)
            constexpr float spaceBetweenButtons = 10;  // Space between buttons
            constexpr float buttonsStartPos = (windowWidth - buttonsWidth - spaceBetweenButtons) * 0.5f;

            ImGui::SetCursorPosX(buttonsStartPos);

            if (ImGui::Button("Yes", ImVec2(120, 40))) {  // Make the button a bit larger
                m_showH5NotExistPopup = false;
                m_showRMLNotExistPopup = false;
                uiParams.rmlReady = true;
                uiParams.runSimulation = true;
                ImGui::CloseCurrentPopup();  // Close the popup when an option is selected
            }

            ImGui::SameLine();  // Keep on the same line to ensure proper spacing

            ImGui::SetCursorPosX(buttonsStartPos + 120 + spaceBetweenButtons);  // Adjust for the next button

            if (ImGui::Button("No", ImVec2(120, 40))) {  // Make the button a bit larger
                m_showH5NotExistPopup = false;
                m_showRMLNotExistPopup = false;
                uiParams.runSimulation = false;  // Do not start the simulation
                ImGui::CloseCurrentPopup();      // Close the popup when an option is selected
            }

            // Revert to original font size
            ImGui::PopFont();

            ImGui::EndPopup();
        }
    }
}

void UIHandler::showSimulationSettingsPopupWindow(UIParameters& uiParams) {
    if (uiParams.runSimulation && !uiParams.simulationSettingsReady) {
        ImGui::OpenPopup("Simulation Settings");

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(410, 230), ImGuiCond_Always);

        if (ImGui::BeginPopupModal("Simulation Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Checkbox("Sequential", &uiParams.simulationInfo.sequential);
            ImGui::InputInt("Max Batch Size", &uiParams.simulationInfo.maxBatchSize);

            // Prepare device combo box
            std::vector<const char*> deviceItems;
            for (const auto& device : uiParams.simulationInfo.availableDevices) {
                deviceItems.push_back(device.c_str());
            }

            const char* tracerItems[] = {"CPU Tracer", "VULKAN Tracer"};
            ImGui::Combo("Tracer", &uiParams.simulationInfo.tracer, tracerItems, IM_ARRAYSIZE(tracerItems));

            // Device selection combo box
            if (uiParams.simulationInfo.tracer == 1) {  // If not CPU Tracer, enable device selection
                ImGui::Combo("Device", &uiParams.simulationInfo.deviceIndex, &deviceItems[0], static_cast<int>(deviceItems.size()));
            } else {
                ImGui::BeginDisabled();  // Disable combo box if CPU Tracer is selected
                ImGui::Combo("Device", &uiParams.simulationInfo.deviceIndex, &deviceItems[0], static_cast<int>(deviceItems.size()));
                ImGui::EndDisabled();
            }
            // startEventID selection
            ImGui::InputInt("Start Event ID", &uiParams.simulationInfo.startEventID);

            // maxEvents selection
            ImGui::InputInt("Max Events", &uiParams.simulationInfo.maxEvents);

            if (!uiParams.simulationInfo.fixedSeed) {
                ImGui::BeginDisabled();
                ImGui::InputInt("Seed", &uiParams.simulationInfo.seed);
                ImGui::EndDisabled();
            } else {
                ImGui::InputInt("Seed", &uiParams.simulationInfo.seed);
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

            if (uiParams.simulationInfo.deviceIndex >= static_cast<int>(uiParams.simulationInfo.availableDevices.size())) {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Start Simulation", ImVec2(120, buttonHeight))) {
                uiParams.simulationSettingsReady = true;
                ImGui::CloseCurrentPopup();
            }

            if (uiParams.simulationInfo.deviceIndex >= static_cast<int>(uiParams.simulationInfo.availableDevices.size())) {
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
