#include "Renderer.h"

#include <array>

#include "Buffer.h"
#include "Debug/Debug.h"
#include "Texture.h"

Renderer::Renderer(Window& window, Device& device) : m_Window(window), m_Device(device) {
    recreateSwapChain();
    createCommandBuffers();
    initializeOffscreenRendering();
}

Renderer::~Renderer() {
    vkDeviceWaitIdle(m_Device.device());

    if (m_offscreenFramebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_Device.device(), m_offscreenFramebuffer, nullptr);
        m_offscreenFramebuffer = VK_NULL_HANDLE;  // Optional, marks the resource as destroyed
    }

    if (m_offscreenRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_Device.device(), m_offscreenRenderPass, nullptr);
        m_offscreenRenderPass = VK_NULL_HANDLE;  // Optional, marks the resource as destroyed
    }

    freeCommandBuffers();
}

void Renderer::recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_Window.window(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_Window.window(), &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_Device.device());

    if (m_SwapChain == nullptr) {
        m_SwapChain = std::make_unique<SwapChain>(m_Device, VkExtent2D(width, height));
    } else {
        std::shared_ptr<SwapChain> oldSwapChain = std::move(m_SwapChain);
        m_SwapChain = std::make_unique<SwapChain>(m_Device, VkExtent2D(width, height), oldSwapChain);

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

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
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
    clearValues[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
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

void Renderer::beginOffscreenRenderPass(FrameInfo& frameInfo, const VkClearValue& clearValue) {
    // Single time command buffer for offscreen rendering
    frameInfo.commandBuffer = m_Device.beginSingleTimeCommands();

    // Begin the offscreen render pass
    VkClearValue clearValues[2];
    clearValues[0].color = clearValue.color;
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_offscreenRenderPass;
    renderPassInfo.framebuffer = m_offscreenFramebuffer;
    renderPassInfo.renderArea.extent.width = m_offscreenExtent.width;
    renderPassInfo.renderArea.extent.height = m_offscreenExtent.height;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(frameInfo.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Set dynamic viewport
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_offscreenExtent.width);
    viewport.height = static_cast<float>(m_offscreenExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(frameInfo.commandBuffer, 0, 1, &viewport);

    // Set dynamic scissor
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = m_offscreenExtent;
    vkCmdSetScissor(frameInfo.commandBuffer, 0, 1, &scissor);
}

void Renderer::endOffscreenRenderPass(FrameInfo& frameInfo) {
    vkCmdEndRenderPass(frameInfo.commandBuffer);
    m_Device.endSingleTimeCommands(frameInfo.commandBuffer);
}

void Renderer::offscreenDescriptorSetUpdate(const DescriptorSetLayout& layout, const DescriptorPool& pool, VkDescriptorSet& descriptorSet) {
    // Check if the descriptor set needs to be recreated
    if (descriptorSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(m_Device.device(), pool.getDescriptorPool(), 1, &descriptorSet);
        descriptorSet = VK_NULL_HANDLE;
    }

    VkDescriptorSetLayout layouts[] = {layout.getDescriptorSetLayout()};
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool.getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(m_Device.device(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = m_offscreenColorTexture->getView();
    imageInfo.sampler = m_offscreenColorTexture->getSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_Device.device(), 1, &descriptorWrite, 0, nullptr);
}

void Renderer::resizeOffscreenResources(const VkExtent2D& extent) {
    m_offscreenExtent = extent;

    m_offscreenColorTexture->resize(extent.width, extent.height);
    m_offscreenDepthTexture->resize(extent.width, extent.height);

    vkDeviceWaitIdle(m_Device.device());
    vkDestroyFramebuffer(m_Device.device(), m_offscreenFramebuffer, nullptr);
    createOffscreenFramebuffer();
}

void Renderer::createOffscreenResources() {
    RAYX_VERB << "Creating offscreen resources";
    m_offscreenColorTexture = std::make_shared<Texture>(
        m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT, m_offscreenExtent);
    m_offscreenColorTexture->transitionImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    m_offscreenDepthTexture = std::make_shared<Texture>(  //
        m_Device, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, m_offscreenExtent);
    m_offscreenDepthTexture->transitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void Renderer::createOffscreenRenderPass() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;  // Same format as the offscreen image
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
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
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_Device.device(), &renderPassInfo, nullptr, &m_offscreenRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen render pass");
    }
}

void Renderer::createOffscreenFramebuffer() {
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_offscreenRenderPass;  // The render pass we just created
    framebufferInfo.attachmentCount = 2;
    std::array<VkImageView, 2> attachments = {m_offscreenColorTexture->getView(), m_offscreenDepthTexture->getView()};
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = m_offscreenExtent.width;
    framebufferInfo.height = m_offscreenExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_Device.device(), &framebufferInfo, nullptr, &m_offscreenFramebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create offscreen framebuffer");
    }
}
