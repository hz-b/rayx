#include "ImGuiLayer.h"

void checkVkResult(VkResult result, const char* message) {
    if (result != VK_SUCCESS) {
        printf("%s\n", message);
        exit(1);
    }
}

ImGuiLayer::ImGuiLayer(const Window& window, const Device& device, const SwapChain& swapchain)
    : m_Window(window), m_Device(device), m_SwapChain(swapchain) {
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
    VkAttachmentDescription attachment = {};
    attachment.format = m_SwapChain.getImageFormat();
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
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
    initInfo.MinImageCount = (uint32_t)m_SwapChain.imageCount();
    initInfo.ImageCount = (uint32_t)m_SwapChain.imageCount();
    initInfo.CheckVkResultFn = nullptr;

    ImGui_ImplGlfw_InitForVulkan(m_Window.window(), true);
    ImGui_ImplVulkan_Init(&initInfo, m_RenderPass);

    createCommandPool();
    createCommandBuffers((uint32_t)m_SwapChain.imageCount());

    // Upload fonts
    {
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

void ImGuiLayer::updateImGui() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (m_ShowDemoWindow) ImGui::ShowDemoWindow(&m_ShowDemoWindow);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Properties Manager");  // Create a window called "Hello, world!" and append into it.

        ImGui::Text("Background");                          // Display some text (you can use a format strings too)
        ImGui::ColorEdit3("Color", (float*)&m_ClearColor);  // Edit 3 floats representing a color

        ImGui::Text("Camera");
        // ImGui::SliderFloat("FOV", &m_Camera.FOV, 0.0f, 180.0f);
        // ImGui::SliderFloat("Target X", &m_Camera.target.x, -10.0f, 10.0f);
        // ImGui::SliderFloat("Target Y", &m_Camera.target.y, -10.0f, 10.0f);
        // ImGui::SliderFloat("Target Z", &m_Camera.target.z, -10.0f, 10.0f);
        // ImGui::SliderFloat("Position X", &m_Camera.position.x, -10.0f, 10.0f);
        // ImGui::SliderFloat("Position Y", &m_Camera.position.y, -10.0f, 10.0f);
        // ImGui::SliderFloat("Position Z", &m_Camera.position.z, -10.0f, 10.0f);
        // ImGui::SliderFloat("Up X", &m_Camera.up.x, -1.0f, 1.0f);  // Slider for Up vector x-coordinate
        // ImGui::SliderFloat("Up Y", &m_Camera.up.y, -1.0f, 1.0f);  // Slider for Up vector y-coordinate
        // ImGui::SliderFloat("Up Z", &m_Camera.up.z, -1.0f, 1.0f);  // Slider for Up vector z-coordinate
        // ImGui::SliderFloat("Near", &m_Camera.near, 0.0f, 100.0f);
        // ImGui::SliderFloat("Far", &m_Camera.far, 0.0f, 10000.0f);

        if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_IO.Framerate, m_IO.Framerate);
        ImGui::End();
    }

    ImGui::Render();
}

VkCommandBuffer ImGuiLayer::recordImGuiCommands(uint32_t currentImage, const VkFramebuffer framebuffer, const VkExtent2D& extent) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(m_CommandBuffers[currentImage], &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }

    VkClearValue clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}};
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = m_RenderPass;
    info.framebuffer = framebuffer;
    info.renderArea.offset = {0, 0};
    info.renderArea.extent = extent;
    info.clearValueCount = 1;
    info.pClearValues = &clearValue;
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