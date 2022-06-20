
#include "CpuTracer.h"

#include <chrono>
#include <cmath>

#include "Debug.h"
#include "Debug/Instrumentor.h"
#include "Material/Material.h"
#include "PathResolver.h"

namespace RAYX {

namespace CPP_TRACER {
#define CPP
#include "shader/main.comp"
}  // namespace CPP_TRACER

CpuTracer::CpuTracer() {
    // Set buffer settings (DEBUG OR RELEASE)
    RAYX_LOG << "Initializing Vulkan Tracer..";
    setSettings();
}

CpuTracer::~CpuTracer() {}

RayList CpuTracer::trace(const Beamline& beamline) {
    m_RayList = beamline.getInputRays();

    setBeamlineParameters(1, beamline.m_OpticalElements.size(),
                          m_RayList.rayAmount());

    for (auto e : beamline.m_OpticalElements) {
        addArrays(e->getSurfaceParams(), e->getInMatrix(), e->getOutMatrix(),
                  e->getObjectParameters(), e->getElementParameters());
    }

    run();

    RayList outRays = m_OutputRays;

    cleanTracer();

    return outRays;
}

/** Function is used to start the Vulkan tracer
 */
void CpuTracer::run() {
    RAYX_PROFILE_FUNCTION();
    const clock_t begin_time = clock();
    RAYX_LOG << "Starting Vulkan Tracer..";

    m_MaterialTables = loadMaterialTables(m_relevantMaterials);

    const clock_t begin_time_getRays = clock();

    mainLoop();

    getRays();

    RAYX_LOG << "Got Rays. Run-time: "
             << float(clock() - begin_time_getRays) / CLOCKS_PER_SEC * 1000
             << " ms";
}

glm::dvec3 convert(Ray::vec3 v) { return glm::dvec3(v.x, v.y, v.z); }
glm::dvec4 convert(Ray::vec4 v) { return glm::dvec4(v.x, v.y, v.z, v.w); }

CPP_TRACER::Ray convert(Ray r) {
    CPP_TRACER::Ray out;
    out.position = convert(r.m_position);
    out.weight = r.m_weight;
    out.direction = convert(r.m_direction);
    out.energy = r.m_energy;
    out.stokes = convert(r.m_stokes);
    out.pathLength = r.m_pathLength;
    out.order = r.m_order;
    out.lastElement = r.m_lastElement;
    out.extraParameter = r.m_extraParam;
    return out;
}

CPP_TRACER::Element convert(Element e) {
    return {e.surfaceParams, e.inTrans, e.outTrans, e.objectParameters,
            e.elementParameters};
}

void CpuTracer::mainLoop() {
    RAYX_PROFILE_FUNCTION();
    const clock_t begin_time = clock();

    CPP_TRACER::numberOfBeamlines = m_numberOfBeamlines;
    CPP_TRACER::numberOfElementsPerBeamline = m_numberOfQuadricsPerBeamline;
    CPP_TRACER::numberOfRays = m_numberOfRays;
    CPP_TRACER::numberOfRaysPerBeamLine = m_numberOfRaysPerBeamline;

    CPP_TRACER::rayData.data.clear();
    CPP_TRACER::outputData.data.clear();
    CPP_TRACER::quadricData.data.clear();
    CPP_TRACER::xyznull.data.clear();
    CPP_TRACER::matIdx.data.clear();
    CPP_TRACER::mat.data.clear();
    CPP_TRACER::d_struct.data.clear();

    // init rayData, outputData
    for (auto a : m_RayList) {
        for (auto r : a) {
            CPP_TRACER::rayData.data.push_back(convert(r));
            CPP_TRACER::outputData.data.push_back({});
        }
    }

    // init quadricData
    for (auto e : m_elementData) {
        CPP_TRACER::quadricData.data.push_back(convert(e));
    }

    CPP_TRACER::mat.data = m_MaterialTables.materialTable;
    CPP_TRACER::matIdx.data = m_MaterialTables.indexTable;

    // init debug buffer
    for (int i = 0; i < m_numberOfRays; i++) {
        CPP_TRACER::_debug_struct d;
        CPP_TRACER::d_struct.data.push_back(d);
    }

    for (int i = 0; i < m_numberOfRays; i++) {
        CPP_TRACER::gl_GlobalInvocationID = i;
        CPP_TRACER::main();
    }

    RAYX_LOG << "CommandBuffer, run time: "
             << float(clock() - begin_time) / CLOCKS_PER_SEC * 1000 << " ms";
}

/** Cleans and deletes the whole tracer instance. Do this only if you do not
 * want to reuse the instance anymore
 * CALL CLEANTRACER FIRST BEFORE CALLING THIS ONE
 */
void CpuTracer::cleanup() {}

/**
 * Use this function if you want to reuse the tracer instance with a new
 * beamline and new rays etc but do not want to initialize everything again
 */
void CpuTracer::cleanTracer() {
    m_relevantMaterials.fill(false);

    m_RayList.clean();
    m_elementData.clear();
    m_OutputRays.clean();
}

Ray::vec3 bconvert(glm::dvec3 v) { return Ray::vec3(v.x, v.y, v.z); }
Ray::vec4 bconvert(glm::dvec4 v) { return Ray::vec4(v.x, v.y, v.z, v.w); }

Ray bconvert(CPP_TRACER::Ray r) {
    Ray out;
    out.m_position = bconvert(r.position);
    out.m_weight = r.weight;
    out.m_direction = bconvert(r.direction);
    out.m_energy = r.energy;
    out.m_stokes = bconvert(r.stokes);
    out.m_pathLength = r.pathLength;
    out.m_order = r.order;
    out.m_lastElement = r.lastElement;
    out.m_extraParam = r.extraParameter;
    return out;
}

void CpuTracer::getRays() {
    m_OutputRays.clean();
    for (auto r : CPP_TRACER::outputData.data) {
        m_OutputRays.insertVector({bconvert(r)});
    }
}

void CpuTracer::setBeamlineParameters(uint32_t inNumberOfBeamlines,
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
}

void CpuTracer::addRayVector(std::vector<Ray>&& inRayVector) {
    RAYX_PROFILE_FUNCTION();

    RAYX_LOG << "Inserting into rayList. rayList.size() before: "
             << m_RayList.size();
    RAYX_LOG << "Sent size: " << inRayVector.size();
    m_RayList.insertVector(std::move(inRayVector));
    RAYX_LOG << "rayList ray count per vector: "
             << (*(m_RayList.begin())).size();
}

// adds quad to beamline
void CpuTracer::addArrays(const std::array<double, 4 * 4>& surfaceParams,
                          const std::array<double, 4 * 4>& inputInMatrix,
                          const std::array<double, 4 * 4>& inputOutMatrix,
                          const std::array<double, 4 * 4>& objectParameters,
                          const std::array<double, 4 * 4>& elementParameters) {
    RAYX_PROFILE_FUNCTION();
    // beamline.resize(beamline.size()+1);
    Element e;
    e.surfaceParams = arrayToGlm16(surfaceParams);
    e.inTrans = arrayToGlm16(inputInMatrix);
    e.outTrans = arrayToGlm16(inputOutMatrix);
    e.objectParameters = arrayToGlm16(objectParameters);
    e.elementParameters = arrayToGlm16(elementParameters);
    m_elementData.push_back(e);

    // if some material occurs in an OpticalElement, it needs to be added to
    // m_relevantMaterials so that the corresponding tables will be loaded.
    int material = surfaceParams[14];  // in [1, 92]
    if (1 <= material && material <= 92) {
        m_relevantMaterials[material - 1] = true;
    }

    // Possibility to use utils/movingAppend
}
void CpuTracer::divideAndSortRays() {
    RAYX_PROFILE_FUNCTION();
    for (auto i = m_RayList.begin(); i != m_RayList.end(); i++) {
    }
}
std::list<std::vector<Ray>>::const_iterator
CpuTracer::getOutputIteratorBegin() {
    return m_OutputRays.begin();
}
std::list<std::vector<Ray>>::const_iterator CpuTracer::getOutputIteratorEnd() {
    return m_OutputRays.end();
}

// Set Vulkan Tracer m_settings according to Release or Debug Mode
void CpuTracer::setSettings() {
#ifdef RAY_DEBUG_MODE
    RAYX_D_LOG << "CpuTracer Debug: ON";
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
