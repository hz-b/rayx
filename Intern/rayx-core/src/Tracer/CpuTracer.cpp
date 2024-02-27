
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

    inv.elements.data.clear();
    inv.xyznull.data.clear();
    inv.matIdx.data.clear();
    inv.mat.data.clear();

    // init rayData, outputData
    inv.rayData.data = rayList;
    inv.outputData.data.resize(rayList.size() * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID));

    // init elements
    for (auto e : cfg.m_elements) {
        inv.elements.data.push_back(e);
    }

    auto materialTables = cfg.m_materialTables;
    inv.mat.data = materialTables.materialTable;
    inv.matIdx.data = materialTables.indexTable;

    // Run the tracing by for all rays
    for (uint i = 0; i < rayList.size(); i++) {
        gl_GlobalInvocationID = i;
        dynamicElements(inv);
    }

    // Fetch Rays back from the Shader "container"
    return inv.outputData.data;
}

void CpuTracer::setPushConstants(const PushConstants* p) { std::memcpy(&inv.pushConstants, p, sizeof(PushConstants)); }
}  // namespace RAYX
