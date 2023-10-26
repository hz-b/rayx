#include "UIRenderSystem.h"

#include <ImGuiFileDialog.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <glm/gtx/quaternion.hpp>

#include "CanonicalizePath.h"

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
void UIRenderSystem::setupUI(UIParameters& uiParams) {
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
    showBeamlineOutlineWindow(uiParams);

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
    ImGui::SetNextWindowSize(ImVec2(450, 350), ImGuiCond_Once);

    ImGui::Begin("Properties Manager");

    // Check ImGui dialog open condition
    if (ImGui::Button("Open File Dialog")) {
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose Beamline (rml) File", ".rml\0", ".");
    }

    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Once);

    // Display file dialog
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string extension = ImGuiFileDialog::Instance()->GetCurrentFilter();

            uiParams.rmlPath = filePathName;
            uiParams.pathChanged = true;
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::Text("Background");
    ImGui::ColorEdit3("Color", (float*)&m_ClearColor);

    ImGui::Text("Camera");
    ImGui::SliderFloat("FOV", &uiParams.camController.m_config.m_FOV, 0.0f, 180.0f);
    ImGui::InputFloat3("Position", &uiParams.camController.m_position.x);
    ImGui::InputFloat3("Direction", &uiParams.camController.m_direction.x);

    if (ImGui::Button("Save Camera")) {
        SaveCameraControllerToFile(uiParams.camController, "camera_save.txt");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Camera")) {
        LoadCameraControllerFromFile(uiParams.camController, "camera_save.txt");
    }

    ImGui::Text("Application average %.6f ms/frame", uiParams.frameTime * 1000.0f);

    ImGui::End();
}

void UIRenderSystem::showSettingsWindow() {
    ImGui::SetNextWindowPos(ImVec2(0, 350), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);

    ImGui::Begin("Settings");

    if (ImGui::Button("Toggle Large Font")) {
        m_useLargeFont = !m_useLargeFont;
    }

    ImGui::End();
}
void renderImGuiTreeFromRML(const std::filesystem::path& filename) {
    // Check if file exists
    if (!std::filesystem::exists(filename)) {
        ImGui::Text("Choose a file to display the beamline outline.");
        return;
    }

    // Read and parse the RML file
    std::ifstream t(filename);
    if (!t.is_open()) {
        ImGui::Text("Error: Could not open file.");
        return;
    }

    std::stringstream buffer;
    buffer << t.rdbuf();
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
        ImGui::Text("Error: XML Parsing failed.");
        return;
    }

    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");
    if (xml_beamline == nullptr) {
        ImGui::Text("Error: <beamline> not found in XML.");
        return;
    }

    // Call recursive function to handle the object collection and render the ImGui tree
    renderImGuiTreeFromXMLNode(xml_beamline);
}

void renderImGuiTreeFromXMLNode(rapidxml::xml_node<>* node) {
    for (rapidxml::xml_node<>* object = node->first_node(); object; object = object->next_sibling()) {
        if (strcmp(object->name(), "object") == 0) {
            ImGui::Text("%s", object->first_attribute("name")->value());
        } else if (strcmp(object->name(), "group") == 0) {
            if (ImGui::TreeNode(object->first_attribute("name")->value())) {
                renderImGuiTreeFromXMLNode(object);
                ImGui::TreePop();
            }
        }
    }
}

void UIRenderSystem::showBeamlineOutlineWindow(UIParameters& uiParams) {
    ImGui::SetNextWindowPos(ImVec2(0, 450), ImGuiCond_Once);  // Position it below the Settings window
    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);

    ImGui::Begin("Beamline Outline");

    renderImGuiTreeFromRML(uiParams.rmlPath);

    ImGui::End();
}
