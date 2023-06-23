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

        // Create first Shader Stage
        ShaderStageCreateInfo shaderCreateInfo = {.name = "FullTracer", .shaderPath = "build/bin/comp.spv", .entryPoint = "main"};

        // Merge all stages
        std::vector<ShaderStageCreateInfo> shaderStages = {shaderCreateInfo};

        // todo: Idea is to have Preloop-> first loop --loop(inCPU)--> until max bounces
        std::vector<ShaderStageCreateInfo> splitShaderStages0 = {
            {.name = "PreLoopStage", .shaderPath = "build/bin/main1.spv", .entryPoint = "main"},
            {.name = "LoopBodyStage", .shaderPath = "build/bin/main2.spv", .entryPoint = "main"},
        };
        //{.name = "PostLoopStage", .shaderPath = "build/bin/main3.spv", .entryPoint = "main"}};

        std::vector<ShaderStageCreateInfo> splitShaderStages1 = {
            {.name = "LoopBodyStage", .shaderPath = "build/bin/main2.spv", .entryPoint = "main"}};

        // Create Compute Pass
        m_engine.createComputePipelinePass({.passName = "BeamlineTracePass", .shaderStagesCreateInfos = shaderStages});

        // Create Compute passes
        m_engine.createComputePipelinePass({.passName = "InitTracePass", .shaderStagesCreateInfos = splitShaderStages0});
        m_engine.createComputePipelinePass({.passName = "TracePass", .shaderStagesCreateInfos = splitShaderStages1});
    }

    // Create Buffers and bind them to Pass through Descriptors
    {
        auto pass = m_engine.getComputePass("passName");                     // TODO : Fill
        auto shaderFlag = pass->getShaderStage(0).getShaderStageFlagBits();  // Should return only compute now
        auto passName = std::string(pass->getName());
        // Compute Buffers Meta
        // Bindings are *IN ORDER*
        m_engine.getBufferHandler()
            .createBuffer<Ray>({"ray-buffer", VKBUFFER_IN}, rayList)  // Input Ray Buffer
            .addDescriptorSetPerPassBinding(passName, 0, shaderFlag)
            .addDescriptorSetPerPassBinding(passName, 0, shaderFlag);

        m_engine.getBufferHandler()
            .createBuffer({"ray-meta-buffer", VKBUFFER_OUT, rayList.size()*sizeof(RayMeta)})  // Meta Ray Buffer
            .addDescriptorSetPerPassBinding(passName, 1, shaderFlag);

        m_engine.getBufferHandler()
            .createBuffer({"output-buffer", VKBUFFER_OUT, (numberOfRays * sizeof(Ray) * (int)cfg.m_maxSnapshots)})  // Output Ray Buffer
            .addDescriptorSetPerPassBinding(passName, 2, shaderFlag);

        m_engine.getBufferHandler()
            .createBuffer<double>({"quadric-buffer", VKBUFFER_IN}, beamlineData)  // Beamline quadric info
            .addDescriptorSetPerPassBinding(passName, 3, shaderFlag);

        m_engine.getBufferHandler()
            .createBuffer({"xyznull-buffer", VKBUFFER_IN, 100})  // FIXME(OS): This buffer is not needed?
            .addDescriptorSetPerPassBinding(passName, 4, shaderFlag);

        m_engine.getBufferHandler()
            .createBuffer<int>({"material-index-table", VKBUFFER_IN}, materialTables.indexTable)  /// Material info
            .addDescriptorSetPerPassBinding(passName, 5, shaderFlag);

        m_engine.getBufferHandler()
            .createBuffer<double>({"material-table", VKBUFFER_IN}, materialTables.materialTable)  // Material info
            .addDescriptorSetPerPassBinding(passName, 6, shaderFlag);
#ifdef RAYX_DEBUG_MODE
        m_engine.getBufferHandler()
            .createBuffer({"debug-buffer", VKBUFFER_OUT, numberOfRays * sizeof(debugBuffer_t)})  // Debug Matrix Buffer
            .addDescriptorSetPerPassBinding(passName, 7, shaderFlag);
#endif
    }
    // Optional
    // m_engine.getBufferHandler().waitTransferQueueIdle();

    // FIXME(OS): Weird pushconstant update
    m_engine.getComputePass("todo")->updatePushConstant(0, const_cast<void*>(m_engine.m_pushConstants.pushConstPtr), m_engine.m_pushConstants.size);

    // Create Pipeline layouts and Descriptor Layouts. Everytime buffer formation (not data) changes we need to prepare again
    m_engine.prepareComputePipelinePasses();  // TODO

    m_engine.run({.m_numberOfInvocations = numberOfRays});

    std::vector<Ray> out = m_engine.getBufferHandler().readBuffer<Ray>("output-buffer", true);

#ifdef RAYX_DEBUG_MODE
    m_debugBufList = m_engine.getBufferHandler().readBuffer<debugBuffer_t>("debug-buffer", true);
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
