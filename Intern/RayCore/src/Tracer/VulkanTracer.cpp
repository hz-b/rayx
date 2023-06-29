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
/**
 * @brief Trace Batch from cfg
 * @param cfg
 */
std::vector<Ray> VulkanTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    // Fetch CFG Data
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

    auto materialTables = cfg.m_materialTables;

    // Init Vulkan, if not yet initialized.
    if (m_engine.state() == VulkanEngine::EngineStates_t::PREINIT) {
        m_engine.init();
        // For now we recreate everything
        // TODO (OS) : Only change buffers and not all pass!

        // todo: Idea is to have Preloop-> first loop --loop(inCPU)--> until max bounces
        std::vector<ShaderStageCreateInfo> splitShaderStages0 = {
            {.name = "PreLoopStage", .shaderPath = "build/bin/main_1.spv", .entryPoint = "main"},
            {.name = "LoopBodyStage", .shaderPath = "build/bin/main_2.spv", .entryPoint = "main"},
        };

        std::vector<ShaderStageCreateInfo> splitShaderStages1 = {
            {.name = "LoopBodyStage", .shaderPath = "build/bin/main_2.spv", .entryPoint = "main"}};

        // Create Compute passes
        m_engine.createComputePipelinePass({.passName = "InitTracePass", .shaderStagesCreateInfos = splitShaderStages0});
        m_engine.createComputePipelinePass({.passName = "TracePass", .shaderStagesCreateInfos = splitShaderStages1});
    }

    m_engine.printPasses();
    auto bufferHandler = m_engine.getBufferHandler();
    // Create Buffers and bind them to Pass through Descriptors
    {
        // Should return only compute now. Important when mixing different shader types to chose right stage flag!
        auto shaderFlag = m_engine.getComputePass("InitTracePass")->getShaderStage(0).getShaderStageFlagBits();
        auto passName0 = std::string(m_engine.getComputePass("InitTracePass")->getName());
        auto passName1 = std::string(m_engine.getComputePass("TracePass")->getName());

        // Bindings are *IN ORDER*
        // PS: You can also call addDescriptorSetPerPassBindings once!
        bufferHandler
            ->createBuffer<Ray>({"ray-buffer", VKBUFFER_IN}, rayList)  // Input Ray Buffer
            .addDescriptorSetPerPassBinding(passName0, 0, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 0, shaderFlag);

        bufferHandler
            ->createBuffer({"ray-meta-buffer", VKBUFFER_OUT, rayList.size() * sizeof(RayMeta)})  // Meta Ray Buffer
            .addDescriptorSetPerPassBinding(passName0, 1, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 1, shaderFlag);

        bufferHandler
            ->createBuffer({"output-buffer", VKBUFFER_OUT, (numberOfRays * sizeof(Ray) * (int)cfg.m_maxSnapshots)})  // Output Ray Buffer
            .addDescriptorSetPerPassBinding(passName0, 2, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 2, shaderFlag);

        bufferHandler
            ->createBuffer<double>({"quadric-buffer", VKBUFFER_IN}, beamlineData)  // Beamline quadric info
            .addDescriptorSetPerPassBinding(passName0, 3, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 3, shaderFlag);

        bufferHandler
            ->createBuffer({"xyznull-buffer", VKBUFFER_IN, 100})  // FIXME(OS): This buffer is not needed?
            .addDescriptorSetPerPassBinding(passName0, 4, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 4, shaderFlag);

        bufferHandler
            ->createBuffer<int>({"material-index-table", VKBUFFER_IN}, materialTables.indexTable)  /// Material info
            .addDescriptorSetPerPassBinding(passName0, 5, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 5, shaderFlag);

        bufferHandler
            ->createBuffer<double>({"material-table", VKBUFFER_IN}, materialTables.materialTable)  // Material info
            .addDescriptorSetPerPassBinding(passName0, 6, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 6, shaderFlag);
#ifdef RAYX_DEBUG_MODE
        bufferHandler
            ->createBuffer({"debug-buffer", VKBUFFER_OUT, numberOfRays * sizeof(debugBuffer_t)})  // Debug Matrix Buffer
            .addDescriptorSetPerPassBinding(passName0, 7, shaderFlag)
            .addDescriptorSetPerPassBinding(passName1, 7, shaderFlag);
#endif
    }
    // Optional
    // m_engine.getBufferHandler().waitTransferQueueIdle();

    // FIXME(OS): Weird pushconstant update
    m_engine.getComputePass("InitTracePass")
        ->updatePushConstant(0, const_cast<void*>(m_engine.m_pushConstants.pushConstPtr), m_engine.m_pushConstants.size);
    m_engine.getComputePass("InitTracePass")
        ->updatePushConstant(1, const_cast<void*>(m_engine.m_pushConstants.pushConstPtr), m_engine.m_pushConstants.size);
    m_engine.getComputePass("TracePass")
        ->updatePushConstant(0, const_cast<void*>(m_engine.m_pushConstants.pushConstPtr), m_engine.m_pushConstants.size);

    // Create Pipeline layouts and Descriptor Layouts. Everytime buffer formation (not data) changes we need to prepare again
    m_engine.prepareComputePipelinePasses();

    m_engine.run({.m_numberOfInvocations = numberOfRays});

    std::vector<Ray> out = bufferHandler->readBuffer<Ray>("output-buffer", true);

#ifdef RAYX_DEBUG_MODE
    m_debugBufList = bufferHandler->readBuffer<debugBuffer_t>("debug-buffer", true);
#endif

    m_engine.cleanup();
    return out;
}

void VulkanTracer::setPushConstants(const PushConstants* p) {
    if (sizeof(*p) > 128) RAYX_WARN << "Using pushConstants bigger than 128 Bytes might be unsupported on some GPUs. Check Compute Info";
    m_engine.m_pushConstants.pushConstPtr = static_cast<const PushConstants*>(p);
    m_engine.m_pushConstants.size = sizeof(*p);

    // TODO (OS) Temp fix, pushconstants need to be updated deeper
    // m_engine.m_ComputePass->updatePushConstant(0, const_cast<PushConstants*>(p), sizeof(*p));
}

}  // namespace RAYX

#endif
