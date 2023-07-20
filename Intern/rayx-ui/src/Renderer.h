#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "Device.h"
#include "ImGuiLayer.h"
#include "Swapchain.h"
#include "Window.h"

class Renderer {
  public:
    Renderer(Window& window, Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    VkRenderPass getSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
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

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    Window& m_Window;
    Device& m_Device;
    // ImGuiLayer m_ImGuiLayer;
    std::unique_ptr<SwapChain> m_SwapChain;
    std::vector<VkCommandBuffer> m_commandBuffers;

    uint32_t m_currentImageIndex;
    uint32_t m_currentFrameIndex{0};
    bool m_isFrameStarted{false};
};