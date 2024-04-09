#pragma once

#include <memory>
#include <vector>

#include "Device.h"
#include "FrameInfo.h"
#include "GraphicsCore/Descriptors.h"
#include "Swapchain.h"
#include "Window.h"

class Texture;

/**
 * @brief The Renderer class manages the rendering process, including the swapchain, command buffers, and ImGui integration.
 */
class Renderer {
  public:
    /**
     * @brief Constructs a Renderer object.
     * @param window The window associated with the renderer.
     * @param device The Vulkan device used for rendering.
     */
    Renderer(Window& window, Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    VkRenderPass getSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
    VkFormat getSwapChainImageFormat() const { return m_SwapChain->getImageFormat(); }
    VkFormat getSwapChainDepthFormat() const { return m_SwapChain->getDepthFormat(); }
    uint32_t getSwapChainImageCount() const { return m_SwapChain->getImageCount(); }
    float getAspectRatio() const { return m_SwapChain->extentAspectRatio(); }
    VkRenderPass getOffscreenRenderPass() const { return m_offscreenRenderPass; }

    bool isFrameInProgress() const { return m_isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {
        assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
        return m_commandBuffers[m_currentFrameIndex];
    }

    uint32_t getFrameIndex() const {
        assert(m_isFrameStarted && "Cannot get frame index when frame not in progress");
        return m_currentFrameIndex;
    }

    /**
     * @brief Begins a new frame, acquiring the next swap chain image.
     * @return VkCommandBuffer The Vulkan command buffer for the frame.
     */
    VkCommandBuffer beginFrame();

    /**
     * @brief Ends the current frame, submitting command buffers and presenting the swap chain image.
     */
    void endFrame();

    /**
     * @brief Begins the render pass for the swap chain.
     * @param commandBuffer The Vulkan command buffer for rendering.
     */
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer, const VkClearValue& clearValue);

    /**
     * @brief Ends the render pass for the swap chain.
     * @param commandBuffer The Vulkan command buffer for rendering.
     */
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    void beginOffscreenRenderPass(FrameInfo& frameInfo);
    void endOffscreenRenderPass(FrameInfo& frameInfo);
    std::shared_ptr<Texture> getOffscreenColorTexture() const { return m_offscreenColorTexture; }
    std::shared_ptr<Texture> getOffscreenDepthTexture() const { return m_offscreenDepthTexture; }

    void offscreenDescriptorSetUpdate(const DescriptorSetLayout& layout, const DescriptorPool& pool, VkDescriptorSet& descriptorSet);
    void resizeOffscreenResources(const VkExtent2D& extent);

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();
    void initializeOffscreenRendering();

    Window& m_Window;
    Device& m_Device;

    std::unique_ptr<SwapChain> m_SwapChain;
    std::vector<VkCommandBuffer> m_commandBuffers;

    // Off screen rendering
    VkExtent2D m_offscreenExtent = {1920, 1080};
    std::shared_ptr<Texture> m_offscreenColorTexture;
    std::shared_ptr<Texture> m_offscreenDepthTexture;
    VkFramebuffer m_offscreenFramebuffer;
    VkRenderPass m_offscreenRenderPass;

    void createOffscreenResources();
    void createOffscreenRenderPass();
    void createOffscreenFramebuffer();

    uint32_t m_currentImageIndex;     // Index of the current swap chain image.
    uint32_t m_currentFrameIndex{0};  // Index of the current frame.
    bool m_isFrameStarted{false};
};