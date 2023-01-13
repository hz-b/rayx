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
    if (m_engine.state() == VulkanEngine::EngineState::PREINIT) {
        // Set buffer settings (DEBUG OR RELEASE)
        RAYX_VERB << "Initializing Vulkan Tracer..";
        m_engine.declareBuffer("ray-buffer", {.m_binding = 0, .m_in = true, .m_out = false});
        m_engine.declareBuffer("output-buffer", {.m_binding = 1, .m_in = false, .m_out = true});
        m_engine.declareBuffer("quadric-buffer", {.m_binding = 2, .m_in = true, .m_out = false});
        m_engine.declareBuffer("xyznull-buffer", {.m_binding = 3, .m_in = false, .m_out = false});
        m_engine.declareBuffer("material-index-table", {.m_binding = 4, .m_in = true, .m_out = false});
        m_engine.declareBuffer("material-table", {.m_binding = 5, .m_in = true, .m_out = false});
#ifdef RAYX_DEBUG_MODE
        m_engine.declareBuffer("debug-buffer", {.m_binding = 6, .m_in = false, .m_out = true});
#endif
        m_engine.init({.m_shader = "build/bin/comp.spv"});
    }

    auto rayList = cfg.m_rays;
    const uint32_t numberOfRays = rayList.size();

    std::vector<double> beamlineData = {cfg.m_rayIdStart, cfg.m_numRays, cfg.m_randomSeed, cfg.m_maxSnapshots};

    for (const auto& e : cfg.m_OpticalElements) {
        std::vector<glm::dmat4x4> mats = {e->getSurfaceParams(), e->getInMatrix(), e->getOutMatrix(), e->getObjectParameters(),
                                          e->getElementParameters()};
        for (auto x : mats) {
            auto mat = glmToArray16(x);
            beamlineData.insert(beamlineData.end(), mat.begin(), mat.end());
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
    m_engine.createBuffer("debug-buffer", numberOfRays * sizeof(_debugBuf_t));
#endif

    m_engine.run({.m_numberOfInvocations = numberOfRays});

    std::vector<Ray> out = m_engine.readBuffer<Ray>("output-buffer");

#ifdef RAYX_DEBUG_MODE
    m_debugBufList = m_engine.readBuffer<_debugBuf_t>("debug-buffer");
#endif

    m_engine.cleanup();

    return out;
}

void VulkanTracer::setPushConstants(PushConstants* p) {
    if (sizeof(*p) > 128)
        RAYX_WARN << "Using pushConstants bigger than 128 Bytes might be unsupported on your GPU. Check Compute Info";
    m_engine.m_pushConstants.m_pushConstants_ptr = static_cast<PushConstants*>(p);
    m_engine.m_pushConstants.size = sizeof(*p);
}
}  // namespace RAYX
