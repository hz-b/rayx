#include "UIRenderSystem.h"

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

// ---- UIRenderSystem ----
UIRenderSystem::UIRenderSystem(const Window& window, const Device& device, VkFormat imageFormat, VkFormat depthFormat, uint32_t imageCount)
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
    initInfo.Allocator = nullptr;
    initInfo.MinImageCount = imageCount;
    initInfo.ImageCount = imageCount;
    initInfo.CheckVkResultFn = nullptr;

    ImGui_ImplGlfw_InitForVulkan(m_Window.window(), true);
    ImGui_ImplVulkan_Init(&initInfo, m_RenderPass);

    // Upload fonts
    {
        // Setup style
        m_smallFont =
            m_IO.Fonts->AddFontFromFileTTF(RAYX::canonicalizeRepositoryPath("./Intern/rayx-ui/res/fonts/Roboto-Regular.ttf").string().c_str(), 16.0f);
        m_largeFont =
            m_IO.Fonts->AddFontFromFileTTF(RAYX::canonicalizeRepositoryPath("./Intern/rayx-ui/res/fonts/Roboto-Regular.ttf").string().c_str(), 24.0f);

        auto tmpCommandBuffer = m_Device.beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(tmpCommandBuffer);
        m_Device.endSingleTimeCommands(tmpCommandBuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

UIRenderSystem::~UIRenderSystem() {
    vkDestroyRenderPass(m_Device.device(), m_RenderPass, nullptr);
    vkDestroyDescriptorPool(m_Device.device(), m_DescriptorPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
/**
 * @brief
 *
 * @param uiParams
 * @param rObjects
 */
void UIRenderSystem::setupUI(UIParameters& uiParams, std::vector<RenderObject>& rObjects) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (m_useLargeFont) {
        ImGui::PushFont(m_largeFont);
    } else {
        ImGui::PushFont(m_smallFont);
    }

    showSceneEditorWindow(uiParams);
    showSettingsWindow();
    showHotkeysWindow();
    showBeamlineOutlineWindow(uiParams, rObjects);

    ImGui::PopFont();
}

void UIRenderSystem::render(VkCommandBuffer commandBuffer) {
    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();

    // Avoid rendering when minimized
    if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f) {
        return;
    }

    // Create and submit command buffers
    ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
}

void UIRenderSystem::showSceneEditorWindow(UIParameters& uiParams) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(450, 450), ImGuiCond_Once);

    ImGui::Begin("Properties Manager");

    if (ImGui::Button("Open File Dialog")) {
        nfdchar_t* outPath;
        constexpr uint32_t filterCount = 1;
        nfdfilteritem_t filterItem[filterCount] = {{"RML Files", "rml, xml"}};
        nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, filterCount, NULL);
        if (result == NFD_OKAY) {
            uiParams.rmlPath = outPath;
            uiParams.pathChanged = true;
        } else if (result == NFD_CANCEL) {
            puts("User pressed cancel.");
        } else {
            printf("Error: %s\n", NFD_GetError());
        }
    }

    ImGui::Text("Background");
    ImGui::ColorEdit3("Color", (float*)&m_ClearColor);

    ImGui::Separator();
    uiParams.camController.displaySettings();
    ImGui::Separator();
    if (!uiParams.rmlPath.empty()) {
        int tempAmountOfRays = uiParams.rayInfo.amountOfRays;
        bool tempDisplayRays = uiParams.rayInfo.displayRays;
        bool tempRenderAllRays = uiParams.rayInfo.renderAllRays;
        displayFilterSlider(&uiParams.rayInfo.amountOfRays, uiParams.rayInfo.maxAmountOfRays, &uiParams.rayInfo.displayRays,
                            &uiParams.rayInfo.renderAllRays);
        if (tempAmountOfRays != uiParams.rayInfo.amountOfRays || tempDisplayRays != uiParams.rayInfo.displayRays ||
            tempRenderAllRays != uiParams.rayInfo.renderAllRays) {
            uiParams.rayInfo.raysChanged = true;
        }
        if (tempRenderAllRays != uiParams.rayInfo.renderAllRays) {
            uiParams.rayInfo.cacheChanged = true;
        }
    }
    ImGui::Text("Application average %.6f ms/frame", uiParams.frameTime * 1000.0f);

    ImGui::End();
}

void UIRenderSystem::showSettingsWindow() {
    ImGui::SetNextWindowPos(ImVec2(0, 450), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);

    ImGui::Begin("Settings");

    ImGui::Checkbox("Large Font", &m_useLargeFont);

    ImGui::End();
}

void UIRenderSystem::showHotkeysWindow() {
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

void renderImGuiTree(const UIRenderSystem::TreeNode& treeNode, CameraController& camController, std::vector<RenderObject>& rObjects) {
    for (auto& child : treeNode.children) {
        if (child.children.empty()) {
            std::string label = child.name;

            if (child.index != -1) {
                label;
            }

            if (ImGui::Selectable(label.c_str())) {
                // Handle selection logic here
                std::cout << "Selected object: " << child.name << " with index " << child.index << std::endl;
                if (child.category == "Optical Element") {
                    camController.lookAtPoint(rObjects[child.index].getTranslationVecor());
                }
            }
        } else {
            if (ImGui::TreeNode(child.name.c_str())) {
                renderImGuiTree(child, camController, rObjects);
                ImGui::TreePop();
            }
        }
    }
}

void UIRenderSystem::buildTreeFromXMLNode(rapidxml::xml_node<>* node, UIRenderSystem::TreeNode& treeNode) {
    for (rapidxml::xml_node<>* xmlChild = node->first_node(); xmlChild; xmlChild = xmlChild->next_sibling()) {
        rapidxml::xml_attribute<>* typeAttr = xmlChild->first_attribute("type");
        std::string type = typeAttr ? typeAttr->value() : "";
        std::string category;

        if (strcmp(xmlChild->name(), "object") == 0) {
            if (type == "Point Source" || type == "Matrix Source" || type == "Dipole" || type == "Dipole Source" || type == "Pixel Source" ||
                type == "Circle Source") {
                category = "Light Source";
                UIRenderSystem::TreeNode objectNode(xmlChild->first_attribute("name")->value(), type, category);
                objectNode.index = m_lightSourceIndex++;
                treeNode.children.emplace_back(objectNode);
            } else if (type == "ImagePlane" || type == "Plane Mirror" || type == "Toroid" || type == "Slit" || type == "Spherical Grating" ||
                       type == "Plane Grating" || type == "Sphere" || type == "Reflection Zoneplate" || type == "Ellipsoid" || type == "Cylinder" ||
                       type == "Cone") {
                category = "Optical Element";
                UIRenderSystem::TreeNode objectNode(xmlChild->first_attribute("name")->value(), type, category);
                objectNode.index = m_opticalElementIndex++;
                treeNode.children.emplace_back(objectNode);
            } else {
                UIRenderSystem::TreeNode objectNode(xmlChild->first_attribute("name")->value(), type, category);
                treeNode.children.emplace_back(objectNode);
            }
        } else if (strcmp(xmlChild->name(), "group") == 0) {
            category = "Group";
            UIRenderSystem::TreeNode groupNode(xmlChild->first_attribute("name")->value(), "", category);
            buildTreeFromXMLNode(xmlChild, groupNode);
            treeNode.children.push_back(groupNode);
        }
    }
}

void UIRenderSystem::renderImGuiTreeFromRML(const std::filesystem::path& filename, CameraController& camController,
                                            std::vector<RenderObject>& rObjects) {
    // Check if file exists
    if (!std::filesystem::exists(filename)) {
        ImGui::Text("Choose a file to display the beamline outline.");
        return;
    }

    // Read and parse the RML file
    std::ifstream fileContent(filename);
    if (!fileContent.is_open()) {
        ImGui::Text("Error: Could not open file.");
        return;
    }

    std::stringstream buffer;
    buffer << fileContent.rdbuf();
    std::string test = buffer.str();

    // Check if the file is empty
    if (test.empty()) {
        ImGui::Text("Error: File is empty.");
        return;
    }

    std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
    rapidxml::xml_document<> doc;

    try {
        doc.parse<0>(cstr.data());
    } catch (rapidxml::parse_error& e) {
        ImGui::Text("Error: XML Parsing failed:");
        ImGui::Text(e.what());
        return;
    }

    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");
    if (xml_beamline == nullptr) {
        ImGui::Text("Error: <beamline> not found in XML.");
        return;
    }

    // Call recursive function to handle the object collection and render the ImGui tree
    m_pTreeRoot = std::make_unique<UIRenderSystem::TreeNode>("Root");
    buildTreeFromXMLNode(xml_beamline, *m_pTreeRoot);
    renderImGuiTree(*m_pTreeRoot, camController, rObjects);
}

void UIRenderSystem::showBeamlineOutlineWindow(UIParameters& uiParams, std::vector<RenderObject>& rObjects) {
    ImGui::SetNextWindowPos(ImVec2(0, 760), ImGuiCond_Once);  // Position it below the Settings window
    ImGui::SetNextWindowSize(ImVec2(450, 300), ImGuiCond_Once);

    ImGui::Begin("Beamline Outline");

    if (uiParams.pathChanged) {
        // Create and render new Tree
        m_lightSourceIndex = 0;
        m_opticalElementIndex = 0;
        renderImGuiTreeFromRML(uiParams.rmlPath, uiParams.camController, rObjects);
    } else if (m_pTreeRoot == nullptr) {
        // Do nothing
        ImGui::Text("Choose a file to display the beamline outline.");
    } else {
        // Render same Tree
        renderImGuiTree(*m_pTreeRoot, uiParams.camController, rObjects);
    }

    ImGui::End();
}
