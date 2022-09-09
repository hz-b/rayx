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

namespace RAYX {
VulkanTracer::VulkanTracer() {
    // Set buffer settings (DEBUG OR RELEASE)
    RAYX_LOG << "Initializing Vulkan Tracer..";
    m_engine.declareBuffer("ray-buffer",
                           {.m_binding = 0, .m_in = true, .m_out = false});
    m_engine.declareBuffer("output-buffer",
                           {.m_binding = 1, .m_in = false, .m_out = true});
    m_engine.declareBuffer("quadric-buffer",
                           {.m_binding = 2, .m_in = true, .m_out = false});
    m_engine.declareBuffer("xyznull-buffer",
                           {.m_binding = 3, .m_in = false, .m_out = false});
    m_engine.declareBuffer("material-index-table",
                           {.m_binding = 4, .m_in = true, .m_out = false});
    m_engine.declareBuffer("material-table",
                           {.m_binding = 5, .m_in = true, .m_out = false});
#ifdef RAYX_DEBUG_MODE
    m_engine.declareBuffer("debug-buffer",
                           {.m_binding = 6, .m_in = false, .m_out = true});
#endif
    m_engine.init({.m_shader = "build/bin/comp.spv"});
}

RayList VulkanTracer::trace(const Beamline& beamline) {
    auto rayList = beamline.getInputRays();

    uint32_t numberOfBeamlines = 1;
    uint32_t numberOfQuadricsPerBeamline = beamline.m_OpticalElements.size();
    uint32_t numberOfRays = rayList.rayAmount();
    uint32_t numberOfRaysPerBeamline = numberOfRays;
    std::vector<double> beamlineData = {
        (double)numberOfBeamlines, (double)numberOfQuadricsPerBeamline,
        (double)numberOfRays, (double)numberOfRaysPerBeamline};

    for (const auto& e : beamline.m_OpticalElements) {
        std::vector<std::array<double, 4 * 4>> mats = {
            e->getSurfaceParams(), glmToArray16(e->getInMatrix()),
            glmToArray16(e->getOutMatrix()), e->getObjectParameters(),
            e->getElementParameters()};
        for (auto x : mats) {
            beamlineData.insert(beamlineData.end(), x.begin(), x.end());
        }
    }

    auto materialTables = beamline.calcMinimalMaterialTables();

    std::vector<Ray> rays_;
    for (auto r : rayList) {
        rays_.push_back(r);
    }
    m_engine.defineBufferByData<Ray>("ray-buffer", rays_);
    m_engine.defineBufferBySize("output-buffer", numberOfRays * sizeof(Ray));
    m_engine.defineBufferByData<double>("quadric-buffer", beamlineData);
    m_engine.defineBufferBySize("xyznull-buffer", 100);
    m_engine.defineBufferByData<int>("material-index-table",
                                     materialTables.indexTable);
    m_engine.defineBufferByData<double>("material-table",
                                        materialTables.materialTable);
#ifdef RAYX_DEBUG_MODE
    m_engine.defineBufferBySize("debug-buffer",
                                numberOfRays * sizeof(_debugBuf_t));
#endif

    m_engine.run({.m_numberOfInvocations = numberOfRays});

    std::vector<Ray> _rays = m_engine.readOutBuffer<Ray>("output-buffer");
    RayList outraylist;
    for (auto r : _rays) {
        outraylist.push(r);
    }

#ifdef RAYX_DEBUG_MODE
    m_debugBufList = m_engine.readOutBuffer<_debugBuf_t>("debug-buffer");
#endif

    m_engine.cleanup();

    return outraylist;
}

}  // namespace RAYX
