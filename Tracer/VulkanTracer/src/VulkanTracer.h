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

const int WORKGROUP_SIZE = 32;
const int RAY_DOUBLE_AMOUNT = 8;

class VulkanTracer
{
public:
    VulkanTracer();
    ~VulkanTracer();
    void run();
    void addRay(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir, double weight);
    void addBeamLineObject(std::vector<double> inQuadric);
    std::vector<double> getRays();
    void cleanup();

private:
    //Member structs:
    struct Quadric{
        Quadric() : points(16) {}
        Quadric(std::vector<double> inQuadric){
            assert(inQuadric.size() == 16);
            points = inQuadric;
        }
        std::vector<double> points;
    };
    struct QueueFamilyIndices
    {
        uint32_t computeFamily;
        bool hasvalue;

        bool isComplete()
        {
            return hasvalue;
        }
    };
    //Member variables:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    std::vector<uint64_t> bufferSizes;
    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceMemory> bufferMemories;
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
    std::vector<Quadric> beamline;
    QueueFamilyIndices QueueFamily;
    
    //Member functions:
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
    void createBuffers();
    void createOutputBuffer();
    void createInputBuffer();
    void fillInputBuffer();
    void fillQuadricBuffer();
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