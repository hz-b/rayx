#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <stdexcept>

#include "Device.h"

class SwapChain {
  public:
    SwapChain(Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous = nullptr);
    ~SwapChain();
    SwapChain(const SwapChain&) = delete;
    void operator=(const SwapChain&) = delete;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    VkFramebuffer getFrameBuffer(int index) const { return m_framebuffers[index]; }
    VkRenderPass getRenderPass() const { return m_RenderPass; }
    VkImageView getImageView(int index) const { return m_imageViews[index]; }
    uint32_t getImageCount() const { return (uint32_t)m_images.size(); }
    VkFormat getImageFormat() const { return m_ImageFormat; }
    VkFormat getDepthFormat() const { return m_DepthFormat; }
    VkExtent2D getExtent() const { return m_Extent; }

    float extentAspectRatio() const { return static_cast<float>(m_Extent.width) / static_cast<float>(m_Extent.height); }

    VkResult acquireNextImage(uint32_t* imageIndex) const;
    VkResult submitCommandBuffers(const std::vector<VkCommandBuffer>& cmdBuffers, uint32_t imageIndex);

    bool compareSwapFormats(const SwapChain& other) const { return other.m_DepthFormat == m_DepthFormat && other.m_ImageFormat == m_ImageFormat; }

  private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
    VkFormat findDepthFormat() const;

    VkFormat m_ImageFormat;
    VkFormat m_DepthFormat;
    VkExtent2D m_Extent;

    std::vector<VkFramebuffer> m_framebuffers;
    VkRenderPass m_RenderPass;

    std::vector<VkImage> m_depthImages;
    std::vector<VkDeviceMemory> m_depthImageMemorys;
    std::vector<VkImageView> m_depthImageViews;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    Device& m_Device;
    VkExtent2D m_WindowExtent;

    VkSwapchainKHR m_SwapChain;
    std::shared_ptr<SwapChain> m_oldSwapChain;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
    size_t m_currentFrame = 0;
};
