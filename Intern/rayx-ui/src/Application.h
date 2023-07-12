#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <optional>
#include <vector>

#include "Beamline/Beamline.h"
#include "Descriptors.h"
#include "GraphicsPipeline.h"
#include "ImGuiLayer.h"
#include "Scene.h"
#include "Swapchain.h"
#include "Tracer/Tracer.h"
#include "VertexBuffer.h"
#include "Window.h"

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// struct QueueFamilyIndices {
//     std::optional<uint32_t> graphicsFamily;
//     std::optional<uint32_t> presentFamily;

//     bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
// };

// struct SwapChainSupportDetails {
//     VkSurfaceCapabilitiesKHR capabilities;
//     std::vector<VkSurfaceFormatKHR> formats;
//     std::vector<VkPresentModeKHR> presentModes;
// };

// struct SwapChain {
//     VkSwapchainKHR self;
//     std::vector<VkImage> images;
//     VkFormat ImageFormat;
//     VkExtent2D Extent;
//     std::vector<VkImageView> imageViews;
//     std::vector<VkFramebuffer> framebuffers;
// };

struct Camera {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(4) float n;
    alignas(4) float f;
};

struct FrameInfo {
    uint32_t frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet descriptorSet;
};

class Application {
  public:
    Application(uint32_t width, uint32_t height, const char* name);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

  private:
    // --- Order matters ---
    Window m_Window;
    Device m_Device;
    SwapChain m_SwapChain;
    ImGuiLayer m_ImGuiLayer;

    std::unique_ptr<DescriptorPool> m_DescriptorPool{nullptr};
    // ----------------------
    Scene m_Scene;

    uint32_t m_CurrentFrame = 0;

    // TODO: remove
    std::unique_ptr<DescriptorSetLayout> m_DescriptorSetLayout;

    VkPipelineLayout m_TrianglePipelineLayout;
    VkPipelineLayout m_LinePipelineLayout;
    VkPipelineLayout m_GridPipelineLayout;

    std::unique_ptr<GraphicsPipeline> m_TrianglePipeline;
    std::unique_ptr<GraphicsPipeline> m_LinePipeline;
    std::unique_ptr<GraphicsPipeline> m_GridPipeline;

    std::vector<VkCommandBuffer> m_CommandBuffers;

    std::vector<VkDescriptorSet> m_DescriptorSets;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;

    void initVulkan();

    void createGraphicsPipelineLayouts();
    void createGraphicsPipelines();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void drawFrame();
};