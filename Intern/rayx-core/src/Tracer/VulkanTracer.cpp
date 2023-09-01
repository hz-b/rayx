#ifndef NO_VULKAN

#include "VulkanTracer.h"

#include <Material/Material.h>

#include <chrono>
#include <cmath>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "RAY-Core.h"
#include "Random.h"

#ifdef RAYX_PLATFORM_MSVC
#ifdef USE_NSIGHT_AFTERMATH
#include "GFSDK_Aftermath.h"
#endif
#endif

namespace RAYX {
void VulkanTracer::listPhysicalDevices() {
    // init, if not yet initialized.
    if (m_engine.state() == VulkanEngine::EngineStates::PREINIT) {
        initEngine();
    }
    auto deviceList = m_engine.getPhysicalDevices();
    unsigned int deviceIndex = 0;
    std::cout << "Listing Vulkan Devices:" << std::endl;
    for (const auto& device : deviceList) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        std::cout << "Device: " << deviceProperties.deviceName << std::endl;
        std::cout << "Device Index: " << deviceIndex++ << std::endl;
        std::cout << "VendorID: " << deviceProperties.vendorID << std::endl << std::endl;
    }
}

BundleHistory VulkanTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // init, if not yet initialized.
    if (m_engine.state() == VulkanEngine::EngineStates::PREINIT) {
        initEngine();
    }

    auto rayList = cfg.m_rays;
    const uint32_t numberOfRays = rayList.size();

    std::vector<double> beamlineData;
    beamlineData.reserve(cfg.m_elements.size());

    for (Element e : cfg.m_elements) {
        auto ptr = (double*)&e;
        const size_t len = sizeof(Element) / sizeof(double);
        // the number of doubles needs to be divisible by 16, otherwise it might introduce padding.
        static_assert(len % 16 == 0);
        for (unsigned int i = 0; i < len; i++) {
            beamlineData.push_back(ptr[i]);
        }
    }

    // Prepare rayMeta data in-between batch traces
    std::vector<RayMeta> rayMeta;
    rayMeta.reserve((size_t)cfg.m_numRays);
    const uint64_t MAX_UINT64 = ~(uint64_t(0));
    uint64_t workerCounterNum = MAX_UINT64 / uint64_t(cfg.m_numRays);
    for (auto i = 0; i < cfg.m_numRays; i++) {
        rayMeta.push_back({.ctr = ((uint64_t)cfg.m_rayIdStart + (uint64_t)i) * workerCounterNum + uint64_t(cfg.m_randomSeed * MAX_UINT64),
                           .nextElementId = -1,   // Intersection element unknown / does not exist
                           .finalized = false});  // Not started
    }

    auto materialTables = cfg.m_materialTables;

    auto bufferHandler = m_engine.getBufferHandler();
    // Create Buffers and bind them to Pass through Descriptors
    {
        RAYX_PROFILE_SCOPE_STDOUT("Buffer creation");
        std::string passName0 = "singleTracePass";
        std::string passName1 = "finalCollisionPass";
        std::string passName2 = "OldFullTracingPass";

        // Should return only compute now. Important when mixing different shader types to chose right stage flag!
        auto shaderFlag = m_engine.getComputePass(passName0)->getPipelines()[0]->getShaderStageFlagBits();

        // Bindings (Vulkan Buffer <- Descriptors (sets))
        // PS: You can also call addDescriptorSetPerPassBindings once!
        bufferHandler
            ->createBuffer<Ray>({"ray-buffer", VKBUFFER_INOUT}, rayList)  // Input/Output Ray Buffer TODO(OS): remove wait for async
            .addDescriptorSetPerPassBinding(passName0, 0, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 0, shaderFlag)
            .addDescriptorSetPerPassBinding(passName2, 0, shaderFlag);

        bufferHandler
            ->createBuffer({"output-buffer", VKBUFFER_OUT, numberOfRays * sizeof(Ray) * (size_t)cfg.m_maxEvents})  // OUTOUT DATA only for main.comp
            .addDescriptorSetPerPassBinding(passName2, 1, shaderFlag);

        bufferHandler
            ->createBuffer({"ray-meta-buffer", VKBUFFER_INOUT}, rayMeta)  // Meta Ray Buffer
            .addDescriptorSetPerPassBinding(passName0, 1, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 1, shaderFlag)
            .addDescriptorSetPerPassBinding(passName2, 2, shaderFlag);

        bufferHandler
            ->createBuffer<double>({"quadric-buffer", VKBUFFER_IN}, beamlineData)  // Beamline quadric info
            .addDescriptorSetPerPassBinding(passName0, 2, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 2, shaderFlag)
            .addDescriptorSetPerPassBinding(passName2, 3, shaderFlag);

        bufferHandler
            ->createBuffer({"xyznull-buffer", VKBUFFER_IN, 100})  // FIXME(OS): This buffer is not needed?
            .addDescriptorSetPerPassBinding(passName0, 3, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 3, shaderFlag)
            .addDescriptorSetPerPassBinding(passName2, 4, shaderFlag);

        bufferHandler
            ->createBuffer<int>({"material-index-table", VKBUFFER_IN}, materialTables.indexTable)  /// Material info
            .addDescriptorSetPerPassBinding(passName0, 4, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 4, shaderFlag)
            .addDescriptorSetPerPassBinding(passName2, 5, shaderFlag);

        bufferHandler
            ->createBuffer<double>({"material-table", VKBUFFER_IN}, materialTables.materialTable)  // Material info
            .addDescriptorSetPerPassBinding(passName0, 5, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 5, shaderFlag)
            .addDescriptorSetPerPassBinding(passName2, 6, shaderFlag);

#ifdef RAYX_DEBUG_MODE
        bufferHandler
            ->createBuffer({"debug-buffer", VKBUFFER_OUT, numberOfRays * sizeof(debugBuffer_t)})  // Debug Matrix Buffer
            .addDescriptorSetPerPassBinding(passName0, 6, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 6, shaderFlag)
            .addDescriptorSetPerPassBinding(passName2, 7, shaderFlag);
#endif
    }

    // FIXME: Push Constants still support only one shader code! (If two shaders require two structs then Tracer needs to also change to carry a
    // vector of structs or similar)
    m_engine.getComputePass("singleTracePass")
        ->updatePushConstant(0, m_engine.m_PushConstantHandler.getData(), m_engine.m_PushConstantHandler.getSize());
    m_engine.getComputePass("finalCollisionPass")
        ->updatePushConstant(0, m_engine.m_PushConstantHandler.getData(), m_engine.m_PushConstantHandler.getSize());
    m_engine.getComputePass("OldFullTracingPass")
        ->updatePushConstant(0, m_engine.m_PushConstantHandler.getData(), m_engine.m_PushConstantHandler.getSize());

    // Create Pipeline layouts and Descriptor Layouts. Everytime buffer formation (not data) changes, we need to prepare again
    m_engine.prepareComputePipelinePasses();

    auto out = m_engine.runTraceComputeTask({.m_numberOfInvocations = numberOfRays, .maxBounces = (int)cfg.m_elements.size()});

    m_engine.cleanup();

    return out;
}

void VulkanTracer::setPushConstants(const PushConstants* p) {
    if (sizeof(*p) > 128) RAYX_WARN << "Using pushConstants bigger than 128 Bytes might be unsupported on some GPUs. Check Compute Info";
    m_engine.m_PushConstantHandler.update((void*)p, sizeof(*p));
}

void VulkanTracer::initEngine() {
    // Set buffer settings (DEBUG OR RELEASE)
    RAYX_VERB << "Initializing Vulkan Tracer..";
    m_engine.init(m_deviceID);  // For now, we recreate everything

    std::vector<Pass::PipelineCreateInfo> splitShaderStages0 = {{.name = "TraceStage", .shaderPath = "build/bin/singleBounce.spv"}};

    std::vector<Pass::PipelineCreateInfo> splitShaderStages1 = {{.name = "FinalCollision", .shaderPath = "build/bin/finalCollision.spv"}};

    std::vector<Pass::PipelineCreateInfo> splitShaderStages2 = {{.name = "OldFullTracing", .shaderPath = "build/bin/main.spv"}};

    // Create Compute passes
    // This compute pass traces one ray bounce. It is called N times (bounces)
    // Each time an event (Checkpoint) is stored in RAM.
    m_engine.createComputePipelinePass({.passName = "singleTracePass", .pipelineCreateInfos = splitShaderStages0});
    m_engine.createComputePipelinePass({.passName = "finalCollisionPass", .pipelineCreateInfos = splitShaderStages1});
    m_engine.createComputePipelinePass({.passName = "OldFullTracingPass", .pipelineCreateInfos = splitShaderStages2});

    m_engine.printPasses();
}
}  // namespace RAYX

#endif
