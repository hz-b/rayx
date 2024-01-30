
#include "CpuTracer.h"

#include <cmath>
#include <cstring>

#include "Beamline/OpticalElement.h"
#include "Material/Material.h"
#include "RAY-Core.h"
#include "Shader/DynamicElements.h"
#include "Shader/InvocationState.h"

using uint = unsigned int;

namespace RAYX {

CpuTracer::CpuTracer() { RAYX_VERB << "Initializing Cpu Tracer.."; }

CpuTracer::~CpuTracer() = default;

std::vector<Ray> CpuTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rayList = cfg.m_rays;

    // CFG meta passed through pushConstants

    inv_elements.data.clear();
    inv_xyznull.data.clear();
    inv_matIdx.data.clear();
    inv_mat.data.clear();

    // init rayData, outputData
    inv_rayData.data = rayList;
    inv_outputData.data.resize(rayList.size() * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID));

    // init elements
    for (auto e : cfg.m_elements) {
        inv_elements.data.push_back(e);
    }

    auto materialTables = cfg.m_materialTables;
    inv_mat.data = materialTables.materialTable;
    inv_matIdx.data = materialTables.indexTable;

    // Run the tracing by for all rays
    for (uint i = 0; i < rayList.size(); i++) {
        gl_GlobalInvocationID = i;
        dynamicElements();
    }

    // Fetch Rays back from the Shader "container"
    return inv_outputData.data;
}

void CpuTracer::setPushConstants(const PushConstants* p) { std::memcpy(&inv_pushConstants, p, sizeof(PushConstants)); }
}  // namespace RAYX
