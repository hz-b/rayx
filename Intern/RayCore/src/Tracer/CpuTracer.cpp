
#include "CpuTracer.h"

#include <chrono>
#include <cmath>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Material/Material.h"
#include "Model/Beamline/OpticalElement.h"
#include "Random.h"

using uint = unsigned int;

namespace RAYX {

namespace CPU_TRACER {
#include "shader/main.comp"
}  // namespace CPU_TRACER

CpuTracer::CpuTracer() { RAYX_VERB << "Initializing Cpu Tracer.."; }

CpuTracer::~CpuTracer() {}

std::vector<Ray> CpuTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rayList = cfg.m_rays;

    CPU_TRACER::rayIdStart = cfg.m_rayIdStart;
    CPU_TRACER::numRays = cfg.m_numRays;
    CPU_TRACER::randomSeed = cfg.m_randomSeed;
    CPU_TRACER::maxSnapshots = cfg.m_maxSnapshots;

    CPU_TRACER::elements.data.clear();
    CPU_TRACER::xyznull.data.clear();
    CPU_TRACER::matIdx.data.clear();
    CPU_TRACER::mat.data.clear();

    // init rayData, outputData
    CPU_TRACER::rayData.data = rayList;
    CPU_TRACER::outputData.data.resize(rayList.size() * cfg.m_maxSnapshots);

    // init elements
    for (auto e : cfg.m_elements) {
        CPU_TRACER::elements.data.push_back(e);
    }

    auto materialTables = cfg.m_materialTables;
    CPU_TRACER::mat.data = materialTables.materialTable;
    CPU_TRACER::matIdx.data = materialTables.indexTable;

    // Run the tracing by for all rays
    for (uint i = 0; i < rayList.size(); i++) {
        CPU_TRACER::gl_GlobalInvocationID = i;
        CPU_TRACER::main();
    }

    // Fetch Rays back from the Shader "container"
    return CPU_TRACER::outputData.data;
}

void CpuTracer::setPushConstants(PushConstants* p) { CPU_TRACER::pushConstants.pushMatrix = p->pushMatrix; }
}  // namespace RAYX
