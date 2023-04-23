#pragma once 

#include <vulkan/vulkan.h>

class Renderer {
  public:
    Renderer();
    ~Renderer();

    void init();
  
    VkAllocationCallbacks* m_Allocator;
    VkInstance m_Instance;
  private:
    VkSurfaceKHR m_Surface;

    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    uint32_t m_QueueFamily;
    VkQueue m_Queue;
    VkDebugReportCallbackEXT m_DebugReport;
    VkPipelineCache m_PipelineCache;
    VkDescriptorPool m_DescriptorPool;

    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void selectGraphicsQueueFamily();
    void createLogicalDevice();
    void createDescriptorPool();

};