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
    setSettings();

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
    m_RayList = beamline.getInputRays();

    setBeamlineParameters(1, beamline.m_OpticalElements.size(),
                          m_RayList.rayAmount());

    for (const auto& e : beamline.m_OpticalElements) {
        addArrays(e->getSurfaceParams(), glmToArray16(e->getInMatrix()),
                  glmToArray16(e->getOutMatrix()), e->getObjectParameters(),
                  e->getElementParameters());
    }

    m_MaterialTables = beamline.calcMinimalMaterialTables();

    std::vector<Ray> rays_;
    for (auto r : m_RayList) {
        rays_.push_back(r);
    }
    m_engine.defineBufferByData<Ray>("ray-buffer", rays_);
    m_engine.defineBufferBySize("output-buffer", m_numberOfRays * sizeof(Ray));
    m_engine.defineBufferByData<double>("quadric-buffer", m_beamlineData);
    m_engine.defineBufferBySize("xyznull-buffer", 100);
    m_engine.defineBufferByData<int>("material-index-table",
                                     m_MaterialTables.indexTable);
    m_engine.defineBufferByData<double>("material-table",
                                        m_MaterialTables.materialTable);
#ifdef RAYX_DEBUG_MODE
    m_engine.defineBufferBySize("debug-buffer",
                                m_numberOfRays * sizeof(_debugBuf_t));
#endif

    m_engine.run({.m_numberOfInvocations = m_numberOfRays});

    std::vector<Ray> _rays = m_engine.readOutBuffer<Ray>("output-buffer");
    RayList outraylist;
    for (auto r : _rays) {
        outraylist.push(r);
    }

#ifdef RAYX_DEBUG_MODE
    m_debugBufList = m_engine.readOutBuffer<_debugBuf_t>("debug-buffer");
#endif

    m_beamlineData.clear();
	m_engine.cleanup();

    return outraylist;
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
