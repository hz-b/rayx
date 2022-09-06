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
    std::map<std::string, BufferSpec> bs = {
        {"ray-buffer", {.binding = 0, .in = true, .out = false}},
        {"output-buffer", {.binding = 1, .in = false, .out = true}},
        {"quadric-buffer", {.binding = 2, .in = true, .out = false}},
        {"xyznull-buffer",
         {.binding = 3,
          .in = true,
          .out = false}},  // TODO what is this buffer?
        {"material-index-table", {.binding = 4, .in = true, .out = false}},
        {"material-table", {.binding = 5, .in = true, .out = false}},
#ifdef RAYX_DEBUG_MODE
        {"debug-buffer", {.binding = 6, .in = false, .out = true}},
#endif
    };
    m_engine.init({.shaderfile = "build/bin/comp.spv", .bufferSpecs = bs});
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

    // TODO fix workaround
    std::vector<Ray> rays;
    for (auto r : m_RayList) {
        rays.push_back(r);
    }
    std::map<std::string, GpuData> buffers = {
        {"ray-buffer", encode(rays)},
        {"quadric-buffer", encode(m_beamlineData)},
        {"material-index-table", encode(m_MaterialTables.indexTable)},
        {"material-table", encode(m_MaterialTables.materialTable)},
    };

    RunSpec r = {.numberOfInvocations = m_numberOfRays,
                 .computeBuffersCount = m_settings.m_computeBuffersCount,
                 .buffers = buffers};

    // creates buffers to transfer data to and from the shader
    m_engine.createBuffers(r);
    m_engine.fillBuffers(r);

    m_engine.run(r);

    const clock_t begin_time_getRays = clock();

    // getRays();

    RAYX_LOG << "Got Rays. Run-time: "
             << float(clock() - begin_time_getRays) / CLOCKS_PER_SEC * 1000
             << " ms";

#ifdef RAYX_DEBUG_MODE
    // getDebugBuffer();
#endif
}  // namespace RAYX

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
