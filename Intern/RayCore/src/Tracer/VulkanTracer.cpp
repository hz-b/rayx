#include "VulkanTracer.h"

#include <Material/Material.h>

#include <chrono>
#include <cmath>

#include "Debug.h"
#include "Debug/Instrumentor.h"
#include "PathResolver.h"

#ifdef RAYX_PLATFORM_MSVC
#ifdef USE_NSIGHT_AFTERMATH
#include "GFSDK_Aftermath.h"
#endif
#endif

#define SHADERPATH "comp.spv"
#ifndef SHADERPATH
#define SHADERPATH "comp_inter.spv"
#endif
#ifndef SHADERPATH
#define SHADERPATH "comp_dyn.spv"
#endif
#ifndef SHADERPATH
//#define SHADERPATH "comp_dyn_inter.spv"
#endif
#ifndef SHADERPATH
#define SHADERPATH "comp_dyn_multi.spv"
#endif
#ifndef SHADERPATH
#define SHADERPATH "comp_new_all.spv"
#endif

// Memory leak detection in windows (debug mode)
#if defined(NDEBUG) && defined(RAYX_PLATFORM_MSVC)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DBG_NEW new
#endif

/**
 * Compute:
 * buffers[0] = ray buffer
 * buffers[1] = output buffer
 * buffers[2] = quadric buffer
 * buffers[3] = xyznull buffer
 * buffers[4] = material index table
 * buffers[5] = material table
 * buffers[6] = debug buffer
 *
 * Staging:
 * buffers[0] = In/Output Staigng buffer
 * buffers[1] = Debug Staging buffer
 **/

const char* COMPUTE_BUFFER_NAMES[7] = {"ray buffer",           "output buffer",
                                       "quadric buffer",       "xyznull buffer",
                                       "material index table", "material table",
                                       "debug buffer"};
const char* STAGING_BUFFER_NAMES[2] = {"In/Out staging buffer",
                                       "Debug staging buffer"};

namespace RAYX {
VulkanTracer::VulkanTracer() {
    // Set buffer settings (DEBUG OR RELEASE)
    RAYX_LOG << "Initializing Vulkan Tracer..";
    setSettings();

    // until they are added, all materials are considered to be irrelevant.
    // Compute Buffers (I/O Storage)
    m_engine.m_compute.m_BufferSizes.resize(m_settings.m_computeBuffersCount);
    m_engine.m_compute.m_Buffers.resize(m_settings.m_computeBuffersCount);
    m_engine.m_compute.m_BufferMemories.resize(
        m_settings.m_computeBuffersCount);
    // Staging Buffers (COPY)
    m_engine.m_staging.m_BufferSizes.resize(m_settings.m_stagingBuffersCount);
    m_engine.m_staging.m_Buffers.resize(m_settings.m_stagingBuffersCount);
    m_engine.m_staging.m_BufferMemories.resize(
        m_settings.m_stagingBuffersCount);

    // Vulkan is initialized (from scratch)
    // PS: To Save time and performance, Vulkan is initialized only once
    // throughout the whole trace process. If any of Vulkan resources are bound
    // to change, Vulkan might need to "be prepared again" (potential TODO)

    /*
    Here we specify a descriptor set layout. This allows us to bind our
    descriptors to resources in the shader.
    */

    /*
    Here we specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to
    the binding point 0. This binds to layout(std140, binding = 0) buffer
    ibuf (input) and layout(std140, binding = 1) buffer obuf (output) etc.. in
    the compute shader.
    */
    // bindings 0, 1, 2, 3, 4, 5, 6 are used right now
    m_engine.init({{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                    VK_SHADER_STAGE_COMPUTE_BIT, nullptr},
                   {1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                    VK_SHADER_STAGE_COMPUTE_BIT, nullptr},
                   {2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                    VK_SHADER_STAGE_COMPUTE_BIT, nullptr},
                   {3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                    VK_SHADER_STAGE_COMPUTE_BIT, nullptr},
                   {4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                    VK_SHADER_STAGE_COMPUTE_BIT, nullptr},
                   {5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                    VK_SHADER_STAGE_COMPUTE_BIT, nullptr},
#ifdef RAYX_DEBUG_MODE
                   {6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                    VK_SHADER_STAGE_COMPUTE_BIT, nullptr}
#endif
    });

    // beamline.resize(0);
}

VulkanTracer::~VulkanTracer() { cleanup(); }

RayList VulkanTracer::trace(const Beamline& beamline) {
    m_RayList = beamline.getInputRays();

    setBeamlineParameters(1, beamline.m_OpticalElements.size(),
                          m_RayList.rayAmount());

    for (const auto& e : beamline.m_OpticalElements) {
        addArrays(e->getSurfaceParams(), glmToArray16(e->getInMatrix()),
                  glmToArray16(e->getOutMatrix()), e->getObjectParameters(),
                  e->getElementParameters());
    }

    m_MaterialTables = beamline.calcMinimalMaterialTables();

    run();

    RayList out = m_OutputRays;
    m_OutputRays = {};

    cleanTracer();

    return out;
}

//	This function destroys the debug messenger
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
    RAYX_PROFILE_FUNCTION();
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

/** Function is used to start the Vulkan tracer
 */
void VulkanTracer::run() {
    RAYX_PROFILE_FUNCTION();
    const clock_t begin_time = clock();
    RAYX_LOG << "Starting Vulkan Tracer..";

    // the sizes of the input and output buffers are set. The buffers need to be
    // the size numberOfRays * size of a Ray * the size of a double (a ray
    // consists of 16 values in double precision, x,y,z for the position and s0,
    // s1, s2, s3; Weight, direction, energy, stokes, pathLEngth, order,
    // lastElement and extraParam.
    // IMPORTANT: The shader size of the buffer needs to be multiples of 32
    // bytes!

    RAYX_LOG << "Setting compute buffers:";

    // Prepare size of compute storage buffers
    m_engine.m_compute.m_BufferSizes[0] = (uint64_t)m_numberOfRays *
                                          VULKANTRACER_RAY_DOUBLE_AMOUNT *
                                          sizeof(double);
    m_engine.m_compute.m_BufferSizes[1] = (uint64_t)m_numberOfRays *
                                          VULKANTRACER_RAY_DOUBLE_AMOUNT *
                                          sizeof(double);
    m_engine.m_compute.m_BufferSizes[2] =
        (VULKANTRACER_QUADRIC_PARAM_DOUBLE_AMOUNT * sizeof(double)) +
        (m_beamlineData.size() * sizeof(double));  // 4 doubles for parameters

    m_engine.m_compute.m_BufferSizes[3] =
        (uint64_t)m_numberOfRays * 4 * sizeof(double);
    m_engine.m_compute.m_BufferSizes[4] =
        m_MaterialTables.indexTable.size() * sizeof(int);
    m_engine.m_compute.m_BufferSizes[5] =
        m_MaterialTables.materialTable.size() * sizeof(double);
    if (isDebug())
        m_engine.m_compute.m_BufferSizes[6] =
            (uint64_t)m_numberOfRays * sizeof(m_debug);

    for (uint32_t i = 0; i < m_engine.m_compute.m_BufferSizes.size(); i++) {
        RAYX_LOG << "Compute Buffer \"" << COMPUTE_BUFFER_NAMES[i]
                 << "\" of size: " << m_engine.m_compute.m_BufferSizes[i]
                 << " Bytes";
    }

    // Prepare size of staging buffers
    m_engine.m_staging.m_BufferSizes[0] =
        std::min((uint64_t)GPU_MAX_STAGING_SIZE,
                 (uint64_t)m_numberOfRays * VULKANTRACER_RAY_DOUBLE_AMOUNT *
                     sizeof(double));  // maximum of 128MB
    if (isDebug())
        m_engine.m_staging.m_BufferSizes[1] = std::min(
            (uint64_t)GPU_MAX_STAGING_SIZE,
            (uint64_t)m_numberOfRays * sizeof(m_debug));  // maximum of 128MB

    for (uint32_t i = 0; i < m_engine.m_staging.m_BufferSizes.size(); i++) {
        RAYX_LOG << "Staging Buffer \"" << STAGING_BUFFER_NAMES[i]
                 << "\" of size: " << m_engine.m_staging.m_BufferSizes[i]
                 << " Bytes";
    }
    // initVulkan();
    RAYX_LOG << "Buffer sizes initiliazed. Run-time: "
             << float(clock() - begin_time) / CLOCKS_PER_SEC * 1000 << " ms";

    prepareBuffers();

    const clock_t begin_time_getRays = clock();

    const clock_t begin_time_cmdbuf = clock();
    runCommandBuffer();
    RAYX_LOG << "CommandBuffer, run time: "
             << float(clock() - begin_time_cmdbuf) / CLOCKS_PER_SEC * 1000
             << " ms";

    getRays();

    RAYX_LOG << "Got Rays. Run-time: "
             << float(clock() - begin_time_getRays) / CLOCKS_PER_SEC * 1000
             << " ms";

#ifdef RAYX_DEBUG_MODE
    getDebugBuffer();
#endif
}

void VulkanTracer::prepareBuffers() {
    // creates buffers to transfer data to and from the shader
    createBuffers();
    const clock_t begin_time_fillBuffer = clock();
    fillRayBuffer();
    RAYX_LOG << "RayBuffer filled, run time: "
             << float(clock() - begin_time_fillBuffer) / CLOCKS_PER_SEC * 1000
             << " ms";
    fillQuadricBuffer();
    fillMaterialBuffer();
    RAYX_LOG << "All buffers filled.";

    createDescriptorSet();

    // a compute pipeline needs to be created
    // the descriptors created earlier will be submitted here
    createComputePipeline();
    createCommandBuffer();
}

/** Cleans and deletes the whole tracer instance. Do this only if you do not
 * want to reuse the instance anymore
 * CALL CLEANTRACER FIRST BEFORE CALLING THIS ONE
 */
void VulkanTracer::cleanup() {
    RAYX_PROFILE_FUNCTION();
    vkDestroyDescriptorSetLayout(m_engine.m_Device,
                                 m_engine.m_DescriptorSetLayout, nullptr);
    vkDestroyCommandPool(m_engine.m_Device, m_engine.m_CommandPool, nullptr);
    {
        RAYX_PROFILE_SCOPE("vkDestroyDevice");
        vkDestroyDevice(m_engine.m_Device, nullptr);
    }
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(m_engine.m_Instance,
                                      m_engine.m_DebugMessenger, nullptr);
    }
    {
        RAYX_PROFILE_SCOPE("vkDestroyInstance");
        vkDestroyInstance(m_engine.m_Instance, nullptr);
    }
}

/**
 * Use this function if you want to reuse the tracer instance with a new
 * beamline and new rays etc but do not want to initialize everything again
 */
void VulkanTracer::cleanTracer() {
    m_RayList.clean();
    m_beamlineData.clear();
    m_OutputRays.clean();
    vkFreeCommandBuffers(m_engine.m_Device, m_engine.m_CommandPool, 1,
                         &m_engine.m_CommandBuffer);
    vkDestroyPipeline(m_engine.m_Device, m_engine.m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_engine.m_Device, m_engine.m_PipelineLayout,
                            nullptr);
    vkDestroyDescriptorPool(m_engine.m_Device, m_engine.m_DescriptorPool,
                            nullptr);

    for (uint32_t i = 0; i < m_engine.m_compute.m_Buffers.size(); i++) {
        vkDestroyBuffer(m_engine.m_Device, m_engine.m_compute.m_Buffers[i],
                        nullptr);
        vkFreeMemory(m_engine.m_Device, m_engine.m_compute.m_BufferMemories[i],
                     nullptr);
    }
    for (uint32_t i = 0; i < m_engine.m_staging.m_Buffers.size(); i++) {
        vkDestroyBuffer(m_engine.m_Device, m_engine.m_staging.m_Buffers[i],
                        nullptr);
        vkFreeMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[i],
                     nullptr);
    }
    vkDestroyShaderModule(m_engine.m_Device, m_engine.m_ComputeShaderModule,
                          nullptr);
}

// find memory type with desired properties.
uint32_t VulkanTracer::findMemoryType(uint32_t memoryTypeBits,
                                      VkMemoryPropertyFlags properties) {
    RAYX_PROFILE_FUNCTION();
    VkPhysicalDeviceMemoryProperties memoryProperties;

    vkGetPhysicalDeviceMemoryProperties(m_engine.m_PhysicalDevice,
                                        &memoryProperties);

    /*
    How does this search work?
    See the documentation of VkPhysicalDeviceMemoryProperties for a detailed
    description.
    */
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((memoryTypeBits & (1 << i)) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & properties) ==
             properties))
            return i;
    }
    return -1;
}

void VulkanTracer::createBuffers() {
    RAYX_PROFILE_FUNCTION();
    // ----COMPUTE
    // Ray Buffer
    createBuffer(
        m_engine.m_compute.m_BufferSizes[0],
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_engine.m_compute.m_Buffers[0],
        m_engine.m_compute.m_BufferMemories[0]);

    // output Buffer
    createBuffer(
        m_engine.m_compute.m_BufferSizes[1],
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_engine.m_compute.m_Buffers[1],
        m_engine.m_compute.m_BufferMemories[1]);

    // Quadric Buffer
    createBuffer(m_engine.m_compute.m_BufferSizes[2],
                 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                 m_engine.m_compute.m_Buffers[2],
                 m_engine.m_compute.m_BufferMemories[2]);

    // buffer for xyznull
    createBuffer(
        m_engine.m_compute.m_BufferSizes[3], VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_engine.m_compute.m_Buffers[3],
        m_engine.m_compute.m_BufferMemories[3]);
    // buffer for material index table
    createBuffer(m_engine.m_compute.m_BufferSizes[4],
                 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                 m_engine.m_compute.m_Buffers[4],
                 m_engine.m_compute.m_BufferMemories[4]);
    // buffer for material table
    createBuffer(m_engine.m_compute.m_BufferSizes[5],
                 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                 m_engine.m_compute.m_Buffers[5],
                 m_engine.m_compute.m_BufferMemories[5]);

    // Buffer for debug
    if (isDebug())
        createBuffer(m_engine.m_compute.m_BufferSizes[6],
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     m_engine.m_compute.m_Buffers[6],
                     m_engine.m_compute.m_BufferMemories[6]);

    // ----STAGING
    // staging buffer for rays
    createBuffer(m_engine.m_staging.m_BufferSizes[0],
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                 m_engine.m_staging.m_Buffers[0],
                 m_engine.m_staging.m_BufferMemories[0]);
    // staging buffer for debug
    if (isDebug())
        createBuffer(m_engine.m_staging.m_BufferSizes[1],
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                     m_engine.m_staging.m_Buffers[1],
                     m_engine.m_staging.m_BufferMemories[1]);
    RAYX_LOG << "All buffers created!";
}

// Creates a buffer to each given object with a given size.
// This also allocates memory to the buffer according the requirements of the
// Physical Device. Sharing is kept to exclusive.
//
// More at
// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkBufferCreateInfo.html
void VulkanTracer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer& buffer,
                                VkDeviceMemory& bufferMemory) {
    RAYX_PROFILE_FUNCTION();
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;  // buffer is used as a storage buffer.
    bufferCreateInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;  // buffer is exclusive to a single
                                    // queue family at a time.
    VK_CHECK_RESULT(
        vkCreateBuffer(m_engine.m_Device, &bufferCreateInfo, nullptr, &buffer));
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_engine.m_Device, buffer,
                                  &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize =
        memoryRequirements.size;  // specify required memory.
    allocateInfo.memoryTypeIndex =
        findMemoryType(memoryRequirements.memoryTypeBits, properties);
    VK_CHECK_RESULT(
        vkAllocateMemory(m_engine.m_Device, &allocateInfo, NULL,
                         &bufferMemory));  // allocate memory on device.

    // Now associate that allocated memory with the buffer. With that, the
    // buffer is backed by actual memory.
    VK_CHECK_RESULT(
        vkBindBufferMemory(m_engine.m_Device, buffer, bufferMemory, 0));
}

void VulkanTracer::fillRayBuffer() {
    RAYX_PROFILE_FUNCTION();

    uint32_t bytesNeeded =
        m_numberOfRays * VULKANTRACER_RAY_DOUBLE_AMOUNT * sizeof(double);
    uint32_t numberOfStagingBuffers = std::ceil(
        (double)bytesNeeded /
        (double)m_engine.m_staging.m_BufferSizes[0]);  // bufferSizes[0] = 128MB
    RAYX_LOG << "Number of staging Buffers: " << numberOfStagingBuffers
             << ", (Bytes needed): " << bytesNeeded << " Bytes";
    auto raySetIterator = m_RayList.getData().begin();
    RAYX_LOG << "Staging...";
    size_t vectorsPerStagingBuffer =
        std::floor(GPU_MAX_STAGING_SIZE / RAY_VECTOR_SIZE);

    for (uint32_t i = 0; i < numberOfStagingBuffers - 1; i++) {
        fillStagingBuffer(i, raySetIterator, vectorsPerStagingBuffer);
        std::advance(raySetIterator, vectorsPerStagingBuffer);
        copyToRayBuffer(i * GPU_MAX_STAGING_SIZE, GPU_MAX_STAGING_SIZE);
        RAYX_LOG << "Debug Info: more than 128MB of rays!";
        bytesNeeded = bytesNeeded - GPU_MAX_STAGING_SIZE;
    }

    fillStagingBuffer((numberOfStagingBuffers - 1) * GPU_MAX_STAGING_SIZE,
                      raySetIterator,
                      std::ceil((double)bytesNeeded / RAY_VECTOR_SIZE));

    copyToRayBuffer((numberOfStagingBuffers - 1) * GPU_MAX_STAGING_SIZE,
                    ((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1);
    RAYX_LOG << "Done.";
}

// the input buffer is filled with the ray data
void VulkanTracer::fillStagingBuffer(
    [[maybe_unused]] uint32_t offset,
    std::list<std::vector<Ray>>::const_iterator raySetIterator,
    size_t vectorsPerStagingBuffer)  // TODO is it okay that offset is unused?
{
    RAYX_PROFILE_FUNCTION();
    // data is copied to the buffer
    void* data;
    vkMapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[0], 0,
                m_engine.m_staging.m_BufferSizes[0], 0, &data);

    if ((*raySetIterator).size() > GPU_MAX_STAGING_SIZE) {
        RAYX_ERR << "(*raySetIterator).size() > GPU_MAX_STAGING_SIZE)!";
    }
    vectorsPerStagingBuffer =
        std::min(m_RayList.getData().size(), vectorsPerStagingBuffer);
    RAYX_LOG << "Vectors per StagingBuffer: " << vectorsPerStagingBuffer;
    for (uint32_t i = 0; i < vectorsPerStagingBuffer; i++) {
        memcpy(((char*)data) + i * RAY_VECTOR_SIZE, (*raySetIterator).data(),
               std::min((*raySetIterator).size() *
                            VULKANTRACER_RAY_DOUBLE_AMOUNT * sizeof(double),
                        (size_t)GPU_MAX_STAGING_SIZE));
        raySetIterator++;
    }
    auto* temp = (double*)data;
    RAYX_LOG << "Debug Info: value: " << temp[0];
    vkUnmapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[0]);
    RAYX_LOG << "Vector in StagingBuffer insterted! [RayList→StagingBuffer]";
}

void VulkanTracer::copyToRayBuffer(uint32_t offset,
                                   uint32_t numberOfBytesToCopy) {
    RAYX_PROFILE_FUNCTION();
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_engine.m_CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_engine.m_Device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = offset;
    copyRegion.size = numberOfBytesToCopy;
    RAYX_LOG << "Copying [StagingBuffer→RayBuffer]: offset: " << offset
             << " size: " << numberOfBytesToCopy << " Bytes";
    vkCmdCopyBuffer(commandBuffer, m_engine.m_staging.m_Buffers[0],
                    m_engine.m_compute.m_Buffers[0], 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_engine.m_ComputeQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_engine.m_ComputeQueue);

    vkFreeCommandBuffers(m_engine.m_Device, m_engine.m_CommandPool, 1,
                         &commandBuffer);
    RAYX_LOG << "Done.";
}
void VulkanTracer::copyToOutputBuffer(uint32_t offset,
                                      uint32_t numberOfBytesToCopy) {
    RAYX_PROFILE_FUNCTION();
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_engine.m_CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_engine.m_Device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    RAYX_LOG << "Copying [OutputBuffer→StagingBuffer]: offset: " << offset
             << " size: " << numberOfBytesToCopy;
    copyRegion.srcOffset = offset;
    copyRegion.size = numberOfBytesToCopy;
    vkCmdCopyBuffer(commandBuffer, m_engine.m_compute.m_Buffers[1],
                    m_engine.m_staging.m_Buffers[0], 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_engine.m_ComputeQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_engine.m_ComputeQueue);

    vkFreeCommandBuffers(m_engine.m_Device, m_engine.m_CommandPool, 1,
                         &commandBuffer);
}

void VulkanTracer::copyFromDebugBuffer(uint32_t offset,
                                       uint32_t numberOfBytesToCopy) {
    RAYX_PROFILE_FUNCTION();
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_engine.m_CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_engine.m_Device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    RAYX_LOG << "Copying [DebugBuffer→StagingBuffer]: offset: " << offset
             << " size:" << numberOfBytesToCopy;
    copyRegion.srcOffset = offset;
    copyRegion.size = numberOfBytesToCopy;
    vkCmdCopyBuffer(commandBuffer, m_engine.m_compute.m_Buffers[6],
                    m_engine.m_staging.m_Buffers[1], 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_engine.m_ComputeQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_engine.m_ComputeQueue);

    vkFreeCommandBuffers(m_engine.m_Device, m_engine.m_CommandPool, 1,
                         &commandBuffer);
}

void VulkanTracer::getRays() {
    RAYX_PROFILE_FUNCTION();
    // reserve enough data for all the rays
    /*
    data.reserve((uint64_t)numberOfRays * VULKANTRACER_RAY_DOUBLE_AMOUNT);
    */

    uint32_t bytesNeeded =
        m_numberOfRays * VULKANTRACER_RAY_DOUBLE_AMOUNT * sizeof(double);
    uint32_t numberOfStagingBuffers = std::ceil(
        (double)bytesNeeded / (double)m_engine.m_staging.m_BufferSizes[0]);
    RAYX_LOG << "getRays: Number of staging Buffers: "
             << numberOfStagingBuffers;

    // TODO: ONLY FIRST STAGING BUFFER IS TRANSFERED
    // numberOfStagingBuffers = 1;

    for (uint32_t i = 0; i < numberOfStagingBuffers - 1; i++) {
        std::vector<Ray> data(GPU_MAX_STAGING_SIZE /
                              (RAY_DOUBLE_COUNT * sizeof(double)));
        copyToOutputBuffer(i * GPU_MAX_STAGING_SIZE, GPU_MAX_STAGING_SIZE);
        RAYX_LOG << "Debug Info: more than 128MB of rays";
        void* mappedMemory = nullptr;
        // Map the buffer memory, so that we can read from it on the CPU.
        vkMapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[0],
                    0, GPU_MAX_STAGING_SIZE, 0, &mappedMemory);
        memcpy(data.data(), mappedMemory, GPU_MAX_STAGING_SIZE);
        data.resize(GPU_MAX_STAGING_SIZE / (RAY_DOUBLE_COUNT * sizeof(double)));
        RAYX_LOG << "getRays: data.size(): " << data.size();
        m_OutputRays.insertVector(data);
        vkUnmapMemory(m_engine.m_Device,
                      m_engine.m_staging.m_BufferMemories[0]);
        bytesNeeded = bytesNeeded - GPU_MAX_STAGING_SIZE;
    }

    copyToOutputBuffer((numberOfStagingBuffers - 1) * GPU_MAX_STAGING_SIZE,
                       ((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1);

    void* mappedMemory = nullptr;
    // Map the buffer memory, so that we can read from it on the CPU.
    vkMapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[0], 0,
                ((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1, 0,
                &mappedMemory);
    std::vector<Ray> data((((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1) /
                          (RAY_DOUBLE_COUNT * sizeof(double)));

    RAYX_LOG << "data size: " << data.size()
             << ", bytes needed: " << bytesNeeded;
    memcpy(data.data(), mappedMemory,
           ((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1);
    data.resize(((((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1) /
                 (RAY_DOUBLE_COUNT * sizeof(double))));
    RAYX_LOG << "data size: " << data.size();
    m_OutputRays.insertVector(data);
    vkUnmapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[0]);

    RAYX_LOG << "Output Data size: "
             << m_OutputRays.getData().front().size() * RAY_DOUBLE_COUNT *
                    sizeof(double)
             << " Bytes";
    RAYX_LOG << "Done fetching [StagingBufer→OutputData].";
}

void VulkanTracer::getDebugBuffer() {
    RAYX_PROFILE_FUNCTION();
    uint32_t bytesNeeded = m_numberOfRays * sizeof(_debugBuf_t);
    uint32_t numberOfStagingBuffers = std::ceil(
        (double)bytesNeeded / (double)m_engine.m_staging.m_BufferSizes[1]);
    RAYX_LOG << "DebugBuffer: Enabled " << bytesNeeded;
    for (uint32_t i = 0; i < numberOfStagingBuffers - 1; i++) {
        std::vector<_debugBuf_t> data(GPU_MAX_STAGING_SIZE /
                                      sizeof(_debugBuf_t));
        copyFromDebugBuffer(i * GPU_MAX_STAGING_SIZE, GPU_MAX_STAGING_SIZE);
        RAYX_LOG << "Debug Info: more than 128MB of rays";
        void* debugMappedMemory = nullptr;
        // Map the buffer memory, so that we can read from it on the CPU.
        vkMapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[1],
                    0, GPU_MAX_STAGING_SIZE, 0, &debugMappedMemory);
        memcpy(data.data(), debugMappedMemory, GPU_MAX_STAGING_SIZE);
        data.resize(GPU_MAX_STAGING_SIZE / sizeof(_debugBuf_t));
        std::move(data.begin(), data.end(), std::back_inserter(m_debugBufList));
        vkUnmapMemory(m_engine.m_Device,
                      m_engine.m_staging.m_BufferMemories[1]);
        bytesNeeded = bytesNeeded - GPU_MAX_STAGING_SIZE;
    }
    void* debugMappedMemory = nullptr;
    copyFromDebugBuffer((numberOfStagingBuffers - 1) * GPU_MAX_STAGING_SIZE,
                        ((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1);
    vkMapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[1], 0,
                ((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1, 0,
                &debugMappedMemory);
    std::vector<_debugBuf_t> data(
        (((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1) / sizeof(_debugBuf_t));
    memcpy(data.data(), debugMappedMemory,
           ((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1);
    data.resize(((((bytesNeeded - 1) % GPU_MAX_STAGING_SIZE) + 1) /
                 sizeof(_debugBuf_t)));
    std::move(data.begin(), data.end(), std::back_inserter(m_debugBufList));
    vkUnmapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[1]);
    RAYX_LOG << "Done.[StagingBufer→DebugData]";
}

// the quad buffer is filled with the quadric data
void VulkanTracer::fillQuadricBuffer() {
    RAYX_PROFILE_FUNCTION();
    RAYX_LOG << "Filling QuadricBuffer..";
    // data is copied to the buffer
    void* data;
    vkMapMemory(m_engine.m_Device, m_engine.m_compute.m_BufferMemories[2], 0,
                m_engine.m_compute.m_BufferSizes[2], 0, &data);
    memcpy(data, m_beamlineData.data(), m_engine.m_compute.m_BufferSizes[2]);
    RAYX_LOG << "Done!";
    vkUnmapMemory(m_engine.m_Device, m_engine.m_compute.m_BufferMemories[2]);
}

void VulkanTracer::fillMaterialBuffer() {
    RAYX_LOG << "Filling MaterialBuffer.. (size = "
             << m_MaterialTables.materialTable.size() << " doubles)";

    // material index buffer
    {
        // data is copied to the buffer
        void* data;
        vkMapMemory(m_engine.m_Device, m_engine.m_compute.m_BufferMemories[4],
                    0, m_engine.m_compute.m_BufferSizes[4], 0, &data);
        memcpy(data, m_MaterialTables.indexTable.data(),
               m_engine.m_compute.m_BufferSizes[4]);
        vkUnmapMemory(m_engine.m_Device,
                      m_engine.m_compute.m_BufferMemories[4]);
    }

    // material buffer
    {
        void* data;
        vkMapMemory(m_engine.m_Device, m_engine.m_compute.m_BufferMemories[5],
                    0, m_engine.m_compute.m_BufferSizes[5], 0, &data);
        memcpy(data, m_MaterialTables.materialTable.data(),
               m_engine.m_compute.m_BufferSizes[5]);
        vkUnmapMemory(m_engine.m_Device,
                      m_engine.m_compute.m_BufferMemories[5]);
    }
    RAYX_LOG << "Done!";
}

/* Descriptor sets need a pool to allocate from, which is crated here. */
void VulkanTracer::createDescriptorSet() {
    RAYX_PROFILE_FUNCTION();
    /*
    one descriptor for each buffer
    */
    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSize.descriptorCount = m_settings.m_computeBuffersCount;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets =
        1;  // we need to allocate one descriptor sets from the pool.
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

    // create descriptor pool.
    VK_CHECK_RESULT(vkCreateDescriptorPool(m_engine.m_Device,
                                           &descriptorPoolCreateInfo, nullptr,
                                           &m_engine.m_DescriptorPool));

    /*
    With the pool allocated, we can now allocate the descriptor set.
    */
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool =
        m_engine.m_DescriptorPool;  // pool to allocate from.
    descriptorSetAllocateInfo.descriptorSetCount =
        1;  // allocate a single descriptor set.
    descriptorSetAllocateInfo.pSetLayouts = &m_engine.m_DescriptorSetLayout;

    // allocate descriptor set.
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_engine.m_Device,
                                             &descriptorSetAllocateInfo,
                                             &m_engine.m_DescriptorSet));

    // Populate every Compute Buffer/Descriptor and configure it.
    for (uint32_t i = 0; i < m_engine.m_compute.m_Buffers.size(); i++) {
        // specify which buffer to use: input buffer
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = m_engine.m_compute.m_Buffers[i];
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = m_engine.m_compute.m_BufferSizes[i];

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet =
            m_engine.m_DescriptorSet;       // write to this descriptor set.
        writeDescriptorSet.dstBinding = i;  // write to the ist binding
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;  // update a single descriptor.
        writeDescriptorSet.descriptorType =
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // storage buffer.
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

        // perform the update of the descriptor set.
        vkUpdateDescriptorSets(m_engine.m_Device, 1, &writeDescriptorSet, 0,
                               nullptr);
    }
}

// Read file into array of bytes, and cast to uint32_t*, then return.
// The data has been padded, so that it fits into an array uint32_t.
uint32_t* VulkanTracer::readFile(uint32_t& length, const char* filename) {
    RAYX_PROFILE_FUNCTION();
    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr) {
        printf("Could not find or open file: %s\n", filename);
    }

    // get file size.
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    if (filesize == -1) {
        RAYX_D_ERR << "Could not get file size.";
        return nullptr;
    }
    fseek(fp, 0, SEEK_SET);

    uint32_t filesizepadded = uint32_t(ceil(filesize / 4.0)) * 4;

    // read file contents.
    char* str = DBG_NEW char[filesizepadded];
    uint32_t readCount = fread(str, sizeof(char), filesize, fp);
    if (readCount != (uint32_t)filesize) {
        RAYX_D_LOG << readCount << " != " << filesize << "...";
        RAYX_D_ERR << "Errors while reading file: " << filename;
    }
    fclose(fp);

    // data padding.
    for (uint32_t i = filesize; i < filesizepadded; i++) {
        str[i] = 0;
    }

    length = filesizepadded;
    return (uint32_t*)str;
}
/*
We create a compute pipeline here.
*/
void VulkanTracer::createComputePipeline() {
    RAYX_PROFILE_FUNCTION();

    /*
    Create a shader module. A shader module basically just encapsulates some
    shader code.
    */
    uint32_t filelength;
    // the code in comp.spv was created by running the command:
    // glslangValidator.exe -V shader.comp
    std::string path = resolvePath(std::string("build/bin/") + SHADERPATH);
    uint32_t* compShaderCode = readFile(filelength, path.c_str());
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = compShaderCode;
    createInfo.codeSize = filelength;

    RAYX_LOG << "Creating compute shader module..";

    VK_CHECK_RESULT(vkCreateShaderModule(m_engine.m_Device, &createInfo,
                                         nullptr,
                                         &m_engine.m_ComputeShaderModule));
    RAYX_LOG << "Shader module(s) created.";
    delete[] compShaderCode;

    RAYX_LOG << "Creating pipeline...";
    /*
    Now let us actually create the compute pipeline.
    It only consists of a single stage with a compute shader.
    So first we specify the compute shader stage, and it's entry
    point(main).
    */
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = m_engine.m_ComputeShaderModule;
    shaderStageCreateInfo.pName = "main";

    /*
    The pipeline layout allows the pipeline to access descriptor sets.
    So we just specify the descriptor set layout we created earlier.
    */
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_engine.m_DescriptorSetLayout;
    VK_CHECK_RESULT(vkCreatePipelineLayout(m_engine.m_Device,
                                           &pipelineLayoutCreateInfo, nullptr,
                                           &m_engine.m_PipelineLayout));

    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = shaderStageCreateInfo;
    pipelineCreateInfo.layout = m_engine.m_PipelineLayout;
    /*
    Now, we finally create the compute pipeline.
    */
    VK_CHECK_RESULT(vkCreateComputePipelines(m_engine.m_Device, VK_NULL_HANDLE,
                                             1, &pipelineCreateInfo, nullptr,
                                             &m_engine.m_Pipeline));
    RAYX_LOG << "Pipeline created.";
}

void VulkanTracer::createCommandBuffer() {
    RAYX_PROFILE_FUNCTION();
    RAYX_LOG << "Creating commandBuffer..";
    /*
    Allocate a command buffer from the previously creeated command pool.
    */
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool =
        m_engine.m_CommandPool;  // specify the command pool to allocate from.

    /* if the command buffer is primary, it can be directly submitted to
    / queues. A secondary buffer has to be called from some primary command
    / buffer, and cannot be directly submitted to a queue. To keep things
    / simple, we use a primary command buffer. */

    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount =
        1;  // allocate a single command buffer.
    VK_CHECK_RESULT(vkAllocateCommandBuffers(
        m_engine.m_Device, &commandBufferAllocateInfo,
        &m_engine.m_CommandBuffer));  // allocate command buffer.

    /*
    Now we shall start recording commands into the newly allocated command
    buffer.
    */
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags =
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;  // the buffer is only
                                                      // submitted and used
                                                      // once in this
                                                      // application.
    VK_CHECK_RESULT(vkBeginCommandBuffer(
        m_engine.m_CommandBuffer, &beginInfo));  // start recording commands.

    /*
    We need to bind a pipeline, AND a descriptor set before we dispatch.
    The validation layer will NOT give warnings if you forget these, so be
    very careful not to forget them.
    */
    vkCmdBindPipeline(m_engine.m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                      m_engine.m_Pipeline);
    vkCmdBindDescriptorSets(
        m_engine.m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
        m_engine.m_PipelineLayout, 0, 1, &m_engine.m_DescriptorSet, 0, nullptr);
    // vkCmdBindDescriptorSets(commandBuffer,
    // VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1,
    // &descriptorSets[1], 0, NULL);

    /*
    Calling vkCmdDispatch basically starts the compute pipeline, and
    executes the compute shader. The number of workgroups is specified in
    the arguments. If you are already familiar with compute shaders from
    OpenGL, this should be nothing new to you.
    */
    auto requiredLocalWorkGroupNo =
        (uint32_t)ceil(m_numberOfRays /
                       float(WORKGROUP_SIZE));  // number of local works groups

    // check if there are too many rays
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_engine.m_PhysicalDevice, &deviceProperties);

    auto limits = deviceProperties.limits;

    auto xgroups = limits.maxComputeWorkGroupCount[0];
    auto ygroups = limits.maxComputeWorkGroupCount[1];
    auto zgroups = limits.maxComputeWorkGroupCount[2];

    // if this limit is reached, you may start using
    // multidimensional local workgroups.
    if (requiredLocalWorkGroupNo > xgroups * ygroups * zgroups) {
        RAYX_ERR << "the given task requires " << requiredLocalWorkGroupNo
                 << " many local work groups, but the maximal number on this "
                    "machine is "
                 << xgroups * ygroups * zgroups;
    } else {
        RAYX_D_LOG << "your machine supports up to "
                   << xgroups * ygroups * zgroups * WORKGROUP_SIZE << " rays";
    }

    // decrease xgroups, ygroups, zgroups so that we get a small number of
    // workgroups stlil covering requiredLocalWorkGroupNo
    {
        while (xgroups * ygroups * (zgroups / 2) >= requiredLocalWorkGroupNo) {
            zgroups /= 2;
        }
        while (xgroups * ygroups * (zgroups - 1) >= requiredLocalWorkGroupNo) {
            zgroups--;
        }

        while (xgroups * (ygroups / 2) * zgroups >= requiredLocalWorkGroupNo) {
            ygroups /= 2;
        }
        while (xgroups * (ygroups - 1) * zgroups >= requiredLocalWorkGroupNo) {
            ygroups--;
        }

        while ((xgroups / 2) * ygroups * zgroups >= requiredLocalWorkGroupNo) {
            xgroups /= 2;
        }
        while ((xgroups - 1) * ygroups * zgroups >= requiredLocalWorkGroupNo) {
            xgroups--;
        }
    }

    RAYX_LOG << "Dispatching commandBuffer...";
    RAYX_D_LOG << "Sending "
               << "(" << xgroups << ", " << ygroups << ", " << zgroups
               << ") to the GPU";
    vkCmdDispatch(m_engine.m_CommandBuffer, xgroups, ygroups, zgroups);

    VK_CHECK_RESULT(vkEndCommandBuffer(
        m_engine.m_CommandBuffer));  // end recording commands.
}

void VulkanTracer::runCommandBuffer() {
    RAYX_PROFILE_FUNCTION();
    /*
    Now we shall finally submit the recorded command buffer to a queue.
    */

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;  // submit a single command buffer
    submitInfo.pCommandBuffers =
        &m_engine.m_CommandBuffer;  // the command buffer to submit.

    /*
        We create a fence.
    */
    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VK_CHECK_RESULT(
        vkCreateFence(m_engine.m_Device, &fenceCreateInfo, nullptr, &fence));

    /*
    We submit the command buffer on the queue, at the same time giving a
    fence. (Fences are like interrupts and used for async computations)
    */
    VK_CHECK_RESULT(
        vkQueueSubmit(m_engine.m_ComputeQueue, 1, &submitInfo, fence));
    /*
    The command will not have finished executing until the fence is
    signaled. So we wait here. Directly afer this, we read our buffer
    from the GPU. Fences give us a hint that the Command in the Queue is
    actually done executing.
    */
    VK_CHECK_RESULT(vkWaitForFences(m_engine.m_Device, 1, &fence, VK_TRUE,
                                    1000000000000000));

    vkDestroyFence(m_engine.m_Device, fence, nullptr);
}

void VulkanTracer::setBeamlineParameters(uint32_t inNumberOfBeamlines,
                                         uint32_t inNumberOfQuadricsPerBeamline,
                                         uint32_t inNumberOfRays) {
    RAYX_PROFILE_FUNCTION();
    RAYX_LOG << "Setting Beamline Parameters:";
    RAYX_LOG << "\tNumber of beamlines: " << inNumberOfBeamlines;
    RAYX_LOG << "\tNumber of Quadrics/Beamline: "
             << inNumberOfQuadricsPerBeamline;
    RAYX_LOG << "\tNumber of Rays: " << inNumberOfRays;
    m_numberOfBeamlines = inNumberOfBeamlines;
    m_numberOfQuadricsPerBeamline = inNumberOfQuadricsPerBeamline;
    m_numberOfRays = inNumberOfRays * inNumberOfBeamlines;
    m_numberOfRaysPerBeamline = inNumberOfRays;
    if (m_beamlineData.size() < 4) {
        m_beamlineData.resize(4);
    }
    m_beamlineData[0] = m_numberOfBeamlines;
    m_beamlineData[1] = m_numberOfQuadricsPerBeamline;
    m_beamlineData[2] = m_numberOfRays;
    m_beamlineData[3] = m_numberOfRaysPerBeamline;
}

void VulkanTracer::addRayVector(std::vector<Ray>&& inRayVector) {
    RAYX_PROFILE_FUNCTION();

    RAYX_LOG << "Inserting into rayList. rayList.rayAmount() before: "
             << m_RayList.rayAmount();
    RAYX_LOG << "Sent size: " << inRayVector.size();
    m_RayList.insertVector(inRayVector);
}

// adds quad to beamline
void VulkanTracer::addArrays(
    const std::array<double, 4 * 4>& surfaceParams,
    const std::array<double, 4 * 4>& inputInMatrix,
    const std::array<double, 4 * 4>& inputOutMatrix,
    const std::array<double, 4 * 4>& objectParameters,
    const std::array<double, 4 * 4>& elementParameters) {
    RAYX_PROFILE_FUNCTION();
    // beamline.resize(beamline.size()+1);

    m_beamlineData.insert(m_beamlineData.end(), surfaceParams.begin(),
                          surfaceParams.end());
    m_beamlineData.insert(m_beamlineData.end(), inputInMatrix.begin(),
                          inputInMatrix.end());
    m_beamlineData.insert(m_beamlineData.end(), inputOutMatrix.begin(),
                          inputOutMatrix.end());
    m_beamlineData.insert(m_beamlineData.end(), objectParameters.begin(),
                          objectParameters.end());
    m_beamlineData.insert(m_beamlineData.end(), elementParameters.begin(),
                          elementParameters.end());
}

uint32_t VulkanTracer::getNumberOfBuffers() const {
    return m_settings.m_buffersCount;
}
bool VulkanTracer::isDebug() const { return m_settings.m_isDebug; }

// Set Vulkan Tracer m_settings according to Release or Debug Mode
void VulkanTracer::setSettings() {
#ifdef RAYX_DEBUG_MODE
    RAYX_D_LOG << "VulkanTracer Debug: ON";
    m_settings.m_isDebug = true;
    m_settings.m_computeBuffersCount = 7;
    m_settings.m_stagingBuffersCount = 2;
#else
    m_settings.m_isDebug = false;
    m_settings.m_computeBuffersCount = 6;
    m_settings.m_stagingBuffersCount = 1;
#endif
    m_settings.m_buffersCount =
        m_settings.m_computeBuffersCount + m_settings.m_stagingBuffersCount;
}
}  // namespace RAYX
