#ifndef NO_VULKAN

#include "VulkanTracer.h"

#include <Material/Material.h>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "RayCore.h"

#ifdef RAYX_PLATFORM_MSVC
#ifdef USE_NSIGHT_AFTERMATH
#include "GFSDK_Aftermath.h"
#endif
#endif

namespace RAYX {
std::vector<Ray> flatten(std::vector<std::vector<Ray>>& allRays) {
    std::vector<Ray> flattRays;
    flattRays.reserve(allRays.size() * allRays[0].size());

    for (const auto& row : allRays) {
        for (auto& r : row) {
            flattRays.push_back(r);
        }
    }
    return flattRays;
}
/**
 * @brief Trace Batch from cfg
 * @param cfg
 */
Rays VulkanTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    // Fetch CFG Data
    auto rayList = cfg.m_rays;
    const uint32_t numberOfRays = rayList.size();

    // Prepare beamline data
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

    // Prepare rayMeta data
    std::vector<RayMeta> rayMeta;
    rayMeta.reserve((size_t)cfg.m_numRays);
    {
        const uint64_t MAX_UINT64 = ~(uint64_t(0));
        uint64_t workerCounterNum = MAX_UINT64 / uint64_t(cfg.m_numRays);
        for (auto i = 0; i < cfg.m_numRays; i++) {
            rayMeta.push_back({.nextElementId = 0,  // Intersection element unknown
                               .ctr = ((uint64_t)cfg.m_rayIdStart + (uint64_t)i) * workerCounterNum + uint64_t(cfg.m_randomSeed * MAX_UINT64),
                               .finalized = false});  // Not started
        }
    }

    auto materialTables = cfg.m_materialTables;

    // Init Vulkan, if not yet initialized.
    // TODO (OS) : Only change buffers and not all pass!
    if (m_engine.state() == VulkanEngine::EngineStates_t::PREINIT) {
        m_engine.init();  // For now, we recreate everything

        std::vector<ShaderStageCreateInfo_t> splitShaderStages0 = {
            {.name = "TraceStage", .shaderPath = "build/bin/singleBounce.spv", .entryPoint = "main"}};
            
        std::vector<ShaderStageCreateInfo_t> splitShaderStages1 = {
            {.name = "FinalCollision", .shaderPath = "build/bin/finalCollision.spv", .entryPoint = "main"}};

        // Create Compute passes
        // This compute pass traces one ray bounce. It is called N times (bounces)
        // Each time an event (Checkpoint) is stored in RAM.
        m_engine.createComputePipelinePass({.passName = "singleTracePass", .shaderStagesCreateInfos = splitShaderStages0});
        m_engine.createComputePipelinePass({.passName = "finalCollisionPass", .shaderStagesCreateInfos = splitShaderStages1});

        m_engine.printPasses();
    }

    auto bufferHandler = m_engine.getBufferHandler();
    // Create Buffers and bind them to Pass through Descriptors
    {
        RAYX_PROFILE_SCOPE_STDOUT("Buffer creation");
        std::string passName0 = "singleTracePass";
        std::string passName1 = "finalCollisionPass";
        // Should return only compute now. Important when mixing different shader types to chose right stage flag!
        auto shaderFlag = m_engine.getComputePass(passName0)->getShaderStage(0).getShaderStageFlagBits();

        // Bindings (Vulkan Buffer <- Descriptors (sets))
        // PS: You can also call addDescriptorSetPerPassBindings once!
        bufferHandler
            ->createBuffer<Ray>({"ray-buffer", VKBUFFER_INOUT}, rayList)  // Input/Output Ray Buffer TODO(OS): remove wait for async
            .addDescriptorSetPerPassBinding(passName0, 0, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 0, shaderFlag);

        bufferHandler
            ->createBuffer({"ray-meta-buffer", VKBUFFER_INOUT}, rayMeta)  // Meta Ray Buffer
            .addDescriptorSetPerPassBinding(passName0, 1, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 1, shaderFlag);

        bufferHandler
            ->createBuffer<double>({"quadric-buffer", VKBUFFER_IN}, beamlineData)  // Beamline quadric info
            .addDescriptorSetPerPassBinding(passName0, 2, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 2, shaderFlag);

        bufferHandler
            ->createBuffer({"xyznull-buffer", VKBUFFER_IN, 100})  // FIXME(OS): This buffer is not needed?
            .addDescriptorSetPerPassBinding(passName0, 3, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 3, shaderFlag);

        bufferHandler
            ->createBuffer<int>({"material-index-table", VKBUFFER_IN}, materialTables.indexTable)  /// Material info
            .addDescriptorSetPerPassBinding(passName0, 4, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 4, shaderFlag);

        bufferHandler
            ->createBuffer<double>({"material-table", VKBUFFER_IN}, materialTables.materialTable)  // Material info
            .addDescriptorSetPerPassBinding(passName0, 5, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 5, shaderFlag);

#ifdef RAYX_DEBUG_MODE
        bufferHandler
            ->createBuffer({"debug-buffer", VKBUFFER_OUT, numberOfRays * sizeof(debugBuffer_t)})  // Debug Matrix Buffer
            .addDescriptorSetPerPassBinding(passName0, 6, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 6, shaderFlag);
#endif
    }

    // FIXME(OS): Weird pushConstant update
    m_engine.getComputePass("singleTracePass")->updatePushConstant(0, m_engine.m_pushConstants.pushConstPtr, m_engine.m_pushConstants.size);
    m_engine.getComputePass("finalCollisionPass")->updatePushConstant(0, m_engine.m_pushConstants.pushConstPtr, m_engine.m_pushConstants.size);

    // Create Pipeline layouts and Descriptor Layouts. Everytime buffer formation (not data) changes we need to prepare again
    m_engine.prepareComputePipelinePasses();
    // Run multiple bounces
    auto out = m_engine.run({.m_numberOfInvocations = numberOfRays, .maxBounces = (int)cfg.m_elements.size()});

#ifdef RAYX_DEBUG_MODE
    m_debugBufList = bufferHandler->readBuffer<debugBuffer_t>("debug-buffer", true);
#endif

    m_engine.cleanup();

    // Vector of vectors
    return out;
}

// Update Tracer pushConstant data
void VulkanTracer::setPushConstants(const PushConstants_t* p) {
    if (sizeof(*p) > 128) RAYX_WARN << "Using pushConstants bigger than 128 Bytes might be unsupported on some GPUs. Check Compute Info";

    m_engine.m_pushConstants.pushConstPtr = (void*)(PushConstants_t*)(p);
    m_engine.m_pushConstants.size = sizeof(*p);
}

}  // namespace RAYX

#endif
