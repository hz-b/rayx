#pragma once

#include <memory>
#include <vector>

#include "Device.h"
#include "Swapchain.h"
#include "Window.h"

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

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    Window& m_Window;
    Device& m_Device;

    std::unique_ptr<SwapChain> m_SwapChain;
    std::vector<VkCommandBuffer> m_commandBuffers;

    uint32_t m_currentImageIndex;     // Index of the current swap chain image.
    uint32_t m_currentFrameIndex{0};  // Index of the current frame.
    bool m_isFrameStarted{false};
};