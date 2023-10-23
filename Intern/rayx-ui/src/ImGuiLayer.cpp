#include "ImGuiLayer.h"

#include <ImGuiFileDialog.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "CanonicalizePath.h"

void checkVkResult(VkResult result, const char* message) {
    if (result != VK_SUCCESS) {
        printf("%s\n", message);
        exit(1);
    }
}

// ---- ImGuiLayer ----
ImGuiLayer::ImGuiLayer(const Window& window, const Device& device, const SwapChain& swapchain) : m_Window(window), m_Device(device) {
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
    colorAttachment.format = swapchain.getImageFormat();  // same as in main render pass
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // same as in main render pass
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;    // same as in main render pass

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = swapchain.findDepthFormat();  // same as in main render pass
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
    initInfo.MinImageCount = (uint32_t)swapchain.imageCount();
    initInfo.ImageCount = (uint32_t)swapchain.imageCount();
    initInfo.CheckVkResultFn = nullptr;

    ImGui_ImplGlfw_InitForVulkan(m_Window.window(), true);
    ImGui_ImplVulkan_Init(&initInfo, m_RenderPass);

    createCommandPool();
    createCommandBuffers((uint32_t)swapchain.imageCount());

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

ImGuiLayer::~ImGuiLayer() {
    vkDestroyCommandPool(m_Device.device(), m_CommandPool, nullptr);
    vkDestroyRenderPass(m_Device.device(), m_RenderPass, nullptr);
    vkDestroyDescriptorPool(m_Device.device(), m_DescriptorPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::updateImGui(CameraController& camController, FrameInfo& frameInfo) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (m_useLargeFont) {
        ImGui::PushFont(m_largeFont);
    } else {
        ImGui::PushFont(m_smallFont);
    }

    ImGui::SetNextWindowPos(ImVec2(30, 30));
    ImGui::SetNextWindowSize(ImVec2(500, 600));
    // Main window
    {
        ImGui::Begin("Properties Manager");

        // Check ImGui dialog open condition
        if (ImGui::Button("Open File Dialog")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose Beamline (rml) File", ".rml\0", ".");
        }

        ImGui::SetNextWindowSize(ImVec2(800, 600));

        // Display file dialog
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string extension = ImGuiFileDialog::Instance()->GetCurrentFilter();

                frameInfo.filePath = filePathName;
                frameInfo.wasPathUpdated = true;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::Text("Background");
        ImGui::ColorEdit3("Color", (float*)&m_ClearColor);

        ImGui::Text("Camera");
        ImGui::SliderFloat("FOV", &camController.m_config.m_FOV, 0.0f, 180.0f);
        ImGui::SliderFloat("Up X", &camController.m_up.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Up Y", &camController.m_up.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Up Z", &camController.m_up.z, -1.0f, 1.0f);
        ImGui::SliderFloat("Near", &camController.m_config.m_near, 0.0f, 100.0f);
        ImGui::SliderFloat("Far", &camController.m_config.m_far, 0.0f, 10000.0f);

        auto isInputDigit = [](const char* str) { return !std::any_of(str, str + strlen(str), ::isspace); };

        static char posStrX[32];
        try {
            snprintf(posStrX, sizeof(posStrX), "%f", camController.m_position.x);
            if (ImGui::InputText("Position X", posStrX, sizeof(posStrX))) {
                if (isInputDigit(posStrX)) {
                    camController.m_position.x = std::stof(posStrX);
                }
            }
        } catch (const std::exception& e) {
        }

        static char posStrY[32];
        try {
            snprintf(posStrY, sizeof(posStrY), "%f", camController.m_position.y);
            if (ImGui::InputText("Position Y", posStrY, sizeof(posStrY))) {
                if (isInputDigit(posStrY)) {
                    camController.m_position.y = std::stof(posStrY);
                }
            }
        } catch (const std::exception& e) {
        }

        static char posStrZ[32];
        try {
            snprintf(posStrZ, sizeof(posStrZ), "%f", camController.m_position.z);
            if (ImGui::InputText("Position Z", posStrZ, sizeof(posStrZ))) {
                if (isInputDigit(posStrZ)) {
                    camController.m_position.z = std::stof(posStrZ);
                }
            }

        } catch (const std::exception& e) {
        }

        static char dirStrX[32];
        try {
            snprintf(dirStrX, sizeof(dirStrX), "%f", camController.m_direction.x);
            if (ImGui::InputText("Direction X", dirStrX, sizeof(dirStrX))) {
                if (isInputDigit(dirStrX)) {
                    camController.m_direction.x = std::stof(dirStrX);
                }
            }
        } catch (const std::exception& e) {
        }

        static char dirStrY[32];
        try {
            snprintf(dirStrY, sizeof(dirStrY), "%f", camController.m_direction.y);
            if (ImGui::InputText("Direction Y", dirStrY, sizeof(dirStrY))) {
                if (isInputDigit(dirStrY)) {
                    camController.m_direction.y = std::stof(dirStrY);
                }
            }
        } catch (const std::exception& e) {
        }

        static char dirStrZ[32];
        try {
            snprintf(dirStrZ, sizeof(dirStrZ), "%f", camController.m_direction.z);
            if (ImGui::InputText("Direction Z", dirStrZ, sizeof(dirStrZ))) {
                if (isInputDigit(dirStrZ)) {
                    camController.m_direction.z = std::stof(dirStrZ);
                }
            }
        } catch (const std::exception& e) {
        }

        if (ImGui::Button("Save Camera")) {
            SaveCameraControllerToFile(camController, "camera_save.txt");
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Camera")) {
            LoadCameraControllerFromFile(camController, "camera_save.txt");
        }

        ImGui::Text("Application average %.6f ms/frame", frameInfo.frameTime);

        ImGui::End();
    }

    ImGui::SetNextWindowPos(ImVec2(30, 630));
    ImGui::SetNextWindowSize(ImVec2(500, 300));
    // In your rendering loop
    {
        ImGui::Begin("Settings");

        if (ImGui::Button("Toggle Large Font")) {
            m_useLargeFont = !m_useLargeFont;
        }

        ImGui::End();
    }

    ImGui::PopFont();

    ImGui::Render();
}

VkCommandBuffer ImGuiLayer::recordImGuiCommands(uint32_t currentImage, const VkFramebuffer framebuffer, const VkExtent2D& extent) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(m_CommandBuffers[currentImage], &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }

    VkClearValue clearValues[2];
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};  // Clear color
    clearValues[1].depthStencil = {1.0f, 0};            // Clear depth and stencil values

    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = m_RenderPass;
    info.framebuffer = framebuffer;
    info.renderArea.offset = {0, 0};
    info.renderArea.extent = extent;
    info.clearValueCount = 2;
    info.pClearValues = clearValues;
    vkCmdBeginRenderPass(m_CommandBuffers[currentImage], &info, VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffers[currentImage]);
    vkCmdEndRenderPass(m_CommandBuffers[currentImage]);

    if (vkEndCommandBuffer(m_CommandBuffers[currentImage]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer");
    }

    return m_CommandBuffers[currentImage];
}

void ImGuiLayer::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = 0;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_Device.device(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
}

void ImGuiLayer::createCommandBuffers(uint32_t cmdBufferCount) {
    m_CommandBuffers.resize(cmdBufferCount);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = cmdBufferCount;

    if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
}
