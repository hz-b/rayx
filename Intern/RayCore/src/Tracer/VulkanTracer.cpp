#ifndef NO_VULKAN

#include "VulkanTracer.h"

#include <Material/Material.h>

#include <chrono>
#include <cmath>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "RayCore.h"

#ifdef RAYX_PLATFORM_MSVC
#ifdef USE_NSIGHT_AFTERMATH
#include "GFSDK_Aftermath.h"
#endif
#endif

namespace RAYX {
std::vector<Ray> VulkanTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // init, if not yet initialized.
    if (m_engine.state() == VulkanEngine::EngineStates_t::PREINIT) {
        // Set buffer settings (DEBUG OR RELEASE)
        RAYX_VERB << "Initializing Vulkan Tracer..";
        m_engine.declareBuffer("ray-buffer", {.binding = 0, .isInput = true, .isOutput = false});
        m_engine.declareBuffer("output-buffer", {.binding = 1, .isInput = false, .isOutput = true});
        m_engine.declareBuffer("quadric-buffer", {.binding = 2, .isInput = true, .isOutput = false});
        m_engine.declareBuffer("xyznull-buffer", {.binding = 3, .isInput = false, .isOutput = false});
        m_engine.declareBuffer("material-index-table", {.binding = 4, .isInput = true, .isOutput = false});
        m_engine.declareBuffer("material-table", {.binding = 5, .isInput = true, .isOutput = false});
#ifdef RAYX_DEBUG_MODE
        m_engine.declareBuffer("debug-buffer", {.binding = 6, .isInput = false, .isOutput = true});
#endif
        m_engine.init({.shaderFileName = "build/bin/comp.spv"});
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

    auto materialTables = cfg.m_materialTables;
    m_engine.createBufferWithData<Ray>("ray-buffer", rayList);
    m_engine.createBuffer("output-buffer", numberOfRays * sizeof(Ray) * cfg.m_maxSnapshots);
    m_engine.createBufferWithData<double>("quadric-buffer", beamlineData);
    m_engine.createBuffer("xyznull-buffer", 100);
    m_engine.createBufferWithData<int>("material-index-table", materialTables.indexTable);
    m_engine.createBufferWithData<double>("material-table", materialTables.materialTable);
#ifdef RAYX_DEBUG_MODE
    m_engine.createBuffer("debug-buffer", numberOfRays * sizeof(debugBuffer_t));
#endif

    m_engine.run({.m_numberOfInvocations = numberOfRays});

    std::vector<Ray> out = m_engine.readBuffer<Ray>("output-buffer");

#ifdef RAYX_DEBUG_MODE
    m_debugBufList = m_engine.readBuffer<debugBuffer_t>("debug-buffer");
#endif

    m_engine.cleanup();

    return out;
}

void VulkanTracer::setPushConstants(const PushConstants* p) {
    if (sizeof(*p) > 128) RAYX_WARN << "Using pushConstants bigger than 128 Bytes might be unsupported on some GPUs. Check Compute Info";
    m_engine.m_pushConstants.pushConstPtr = static_cast<const PushConstants*>(p);
    m_engine.m_pushConstants.size = sizeof(*p);
}
}  // namespace RAYX

#endif
