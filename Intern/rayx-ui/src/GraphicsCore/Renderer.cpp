#include "Renderer.h"

#include "Buffer.h"
#include "Texture.h"

Renderer::Renderer(Window& window, Device& device) : m_Window(window), m_Device(device) {
    recreateSwapChain();
    createCommandBuffers();
    initializeOffscreenRendering();
}

Renderer::~Renderer() { freeCommandBuffers(); }

void Renderer::initRenderSystems(const DescriptorSetLayout& globalSetLayout) {
    m_GridRenderSystem = std::make_unique<GridRenderSystem>(m_Device, m_offscreenRenderPass, globalSetLayout.getDescriptorSetLayout());
}

void Renderer::recreateSwapChain() {
    auto extent = m_Window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = m_Window.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_Device.device());

    if (m_SwapChain == nullptr) {
        m_SwapChain = std::make_unique<SwapChain>(m_Device, extent);
    } else {
        std::shared_ptr<SwapChain> oldSwapChain = std::move(m_SwapChain);
        m_SwapChain = std::make_unique<SwapChain>(m_Device, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*m_SwapChain)) {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
}

void Renderer::createCommandBuffers() {
    m_commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_Device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void Renderer::freeCommandBuffers() {
    vkFreeCommandBuffers(m_Device.device(), m_Device.getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
    m_commandBuffers.clear();
}

VkCommandBuffer Renderer::beginFrame() {
    assert(!m_isFrameStarted && "Can't call beginFrame while already in progress");

    auto result = m_SwapChain->acquireNextImage(&m_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    m_isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
}

void Renderer::endFrame() {
    assert(m_isFrameStarted && "Can't call endFrame while frame is not in progress");

    VkCommandBuffer commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    std::vector<VkCommandBuffer> submitCommandBuffers = {commandBuffer};

    VkResult result = m_SwapChain->submitCommandBuffers(submitCommandBuffers, m_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized()) {
        m_Window.resetWindowResizedFlag();
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_isFrameStarted = false;
    m_currentFrameIndex = (m_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer, const VkClearValue& clearValue) {
    assert(m_isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_SwapChain->getRenderPass();
    renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(m_currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    VkExtent2D extent = m_SwapChain->getExtent();
    renderPassInfo.renderArea.extent = extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = clearValue.color;
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, extent};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(m_isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::initializeOffscreenRendering() {
    createOffscreenResources();
    createOffscreenRenderPass();
    createOffscreenFramebuffer();
}

void Renderer::renderOffscreen(FrameInfo& frameInfo) {
    destroyOffscreenResources();
    createOffscreenResources();
    createOffscreenFramebuffer();

    // Single time command buffer for offscreen rendering
    VkCommandBuffer commandBuffer = m_Device.beginSingleTimeCommands();

    // Begin the offscreen render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_offscreenRenderPass;
    renderPassInfo.framebuffer = m_offscreenFramebuffer;
    renderPassInfo.renderArea.extent.width = 1920;   // Full HD Width
    renderPassInfo.renderArea.extent.height = 1080;  // Full HD Height

    VkClearValue clearValues[2];
    clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Set dynamic viewport
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(1920);   // Full HD Width
    viewport.height = static_cast<float>(1080);  // Full HD Height
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Set dynamic scissor
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = {1920, 1080};  // Full HD Dimensions
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    frameInfo.commandBuffer = commandBuffer;
    m_GridRenderSystem->render(frameInfo);

    vkCmdEndRenderPass(commandBuffer);

    // End command buffer recording
    m_Device.endSingleTimeCommands(commandBuffer);
}

void Renderer::createOffscreenResources() {
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

    // Create the offscreen image
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent.width = 1920;   // Full HD Width
    imageInfo.extent.height = 1080;  // Full HD Height
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(m_Device.device(), &imageInfo, nullptr, &m_offscreenImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen image");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device.device(), m_offscreenImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_Device.device(), &allocInfo, nullptr, &m_offscreenImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate offscreen image memory");
    }

    vkBindImageMemory(m_Device.device(), m_offscreenImage, m_offscreenImageMemory, 0);

    // Create the offscreen image view
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_offscreenImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device.device(), &viewInfo, nullptr, &m_offscreenImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen image view");
    }

    // Create the offscreen depth image
    format = VK_FORMAT_D32_SFLOAT;

    // Create the offscreen image
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent.width = 1920;   // Full HD Width
    imageInfo.extent.height = 1080;  // Full HD Height
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(m_Device.device(), &imageInfo, nullptr, &m_offscreenDepthImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen image");
    }

    vkGetImageMemoryRequirements(m_Device.device(), m_offscreenDepthImage, &memRequirements);

    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_Device.device(), &allocInfo, nullptr, &m_offscreenDepthImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate offscreen image memory");
    }

    vkBindImageMemory(m_Device.device(), m_offscreenDepthImage, m_offscreenDepthImageMemory, 0);

    // Create the offscreen image view
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_offscreenDepthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device.device(), &viewInfo, nullptr, &m_offscreenDepthImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen image view");
    }
}

void Renderer::destroyOffscreenResources() {
    // Destroy the offscreen depth image view
    if (m_offscreenDepthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_Device.device(), m_offscreenDepthImageView, nullptr);
        m_offscreenDepthImageView = VK_NULL_HANDLE;
    }

    // Destroy the offscreen depth image
    if (m_offscreenDepthImage != VK_NULL_HANDLE) {
        vkDestroyImage(m_Device.device(), m_offscreenDepthImage, nullptr);
        m_offscreenDepthImage = VK_NULL_HANDLE;
    }

    // Destroy the offscreen image view
    if (m_offscreenImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_Device.device(), m_offscreenImageView, nullptr);
        m_offscreenImageView = VK_NULL_HANDLE;
    }

    // If there are other offscreen resources (like framebuffers or render passes), destroy them here as well
    if (m_offscreenFramebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_Device.device(), m_offscreenFramebuffer, nullptr);
        m_offscreenFramebuffer = VK_NULL_HANDLE;
    }
}

void Renderer::createOffscreenRenderPass() {
    // Define a simple render pass with a single color attachment for offscreen rendering
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;  // Same format as the offscreen image
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;    // Clear at the start
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;  // Store when done
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;           // Don't care about initial layout of the image
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;  // Image will be read from, so transition to transfer source layout

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(m_Device.device(), &renderPassInfo, nullptr, &m_offscreenRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen render pass");
    }
}

void Renderer::createOffscreenFramebuffer() {
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_offscreenRenderPass;  // The render pass we just created
    framebufferInfo.attachmentCount = 2;
    std::array<VkImageView, 2> attachments = {m_offscreenImageView, m_offscreenDepthImageView};
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = 1920;   // Full HD
    framebufferInfo.height = 1080;  // Full HD
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_Device.device(), &framebufferInfo, nullptr, &m_offscreenFramebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen framebuffer");
    }
}

Texture Renderer::getRenderedImage() const {  // TODO: Parameterize the width and height
    return Texture(m_Device, m_offscreenImage, m_offscreenImageMemory, VK_FORMAT_R8G8B8A8_UNORM);
}
