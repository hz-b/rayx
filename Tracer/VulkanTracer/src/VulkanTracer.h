#include "vulkan/vulkan.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <map>
#include <optional>
#include "Ray.h"
#include <assert.h>
#include <cmath>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

//set debug generation information
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

// Used for validating return values of Vulkan API calls.
#define VK_CHECK_RESULT(f)                                                                \
    {                                                                                     \
        VkResult res = (f);                                                               \
        if (res != VK_SUCCESS)                                                            \
        {                                                                                 \
            printf("Fatal : VkResult is %d in %s at line %d\n", res, __FILE__, __LINE__); \
            assert(res == VK_SUCCESS);                                                    \
        }                                                                                 \
    }
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

const int WORKGROUP_SIZE = 1;
const int RAY_DOUBLE_AMOUNT = 12;

class VulkanTracer
{
public:
    VulkanTracer();
    ~VulkanTracer();
    void run();
    void addRay(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir, double weight);
    std::vector<double> getRays();
    void cleanup();

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    uint32_t outputBufferSize;
    VkBuffer outputBuffer;
    VkDeviceMemory outputBufferMemory;
    uint32_t inputBufferSize;
    VkBuffer inputBuffer;
    VkDeviceMemory inputBufferMemory;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkShaderModule computeShaderModule;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;
    VkQueue computeQueue;
    uint32_t queueFamilyIndex;
    uint32_t rayAmount;
    std::vector<Ray> rayVector;
    struct QueueFamilyIndices
    {
        uint32_t computeFamily;
        bool hasvalue;

        bool isComplete()
        {
            return hasvalue;
        }
    };
    QueueFamilyIndices QueueFamily;

    void initVulkan();
    void mainLoop();
    void createInstance();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void setupDebugMessenger();
    std::vector<const char *> getRequiredExtensions();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
    bool checkValidationLayerSupport();
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    int rateDevice(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void createLogicalDevice();
    uint32_t findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
    void createOutputBuffer();
    void createInputBuffer();
    void fillInputBuffer();
    void createDescriptorSetLayout();
    void createDescriptorSet();
    uint32_t *readFile(uint32_t &length, const char *filename);
    void createComputePipeline();
    void createCommandBuffer();
    void runCommandBuffer();
    void setRayAmount(uint32_t inputRayAmount);
    void setRayAmount();
    void readDataFromOutputBuffer();
    void generateRays();

    int main();
};