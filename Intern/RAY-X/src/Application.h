#pragma once

#include <vulkan/vulkan.h>

#ifdef IMGUI_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
                                                   size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
                                                   void* pUserData) {
    (void)flags;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    (void)pLayerPrefix;  // Unused arguments
    fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}
#endif  // IMGUI_VULKAN_DEBUG_REPORT

class Window;

// TODO(Jannis): try to move ImGui out of the engine
class Application {
  public:
    Application(Window& window);
    ~Application();

    void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);

    // Getter
    bool getRebuildSwapChain() const { return m_rebuildSwapChain; }

  private:
    VkAllocationCallbacks* m_Allocator;
    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    uint32_t m_QueueFamily;
    VkQueue m_Queue;
    VkDebugReportCallbackEXT m_DebugReport;
    VkPipelineCache m_PipelineCache;
    VkDescriptorPool m_DescriptorPool;

    VkSurfaceKHR m_Surface;

    ImGui_ImplVulkanH_Window m_MainWindowData;
    int m_minImageCount = 2;
    bool m_rebuildSwapChain = false;

    void createWindowSurface(Window& window);
    void createVkInstance(const char** extensions, uint32_t extensions_count);
    void selectGPU();
    void selectQueueFamily();
    void createLogicalDevice();
    void createDescriptorPool();

    void cleanupVulkan();
    void cleanupVulkanWindow();
};