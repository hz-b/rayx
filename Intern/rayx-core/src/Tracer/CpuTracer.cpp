
#include "CpuTracer.h"

#include <chrono>
#include <cmath>
#include <cstring>

#include "Beamline/OpticalElement.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Material/Material.h"
#include "Random.h"

using uint = unsigned int;

namespace RAYX {

namespace CPU_TRACER {
#include "shader/common.comp"
#include "shader/finalCollision.comp"
#include "shader/singleBounce.comp"
}  // namespace CPU_TRACER

bool _allFinalized(const std::vector<RayMeta>& vector) {
    for (const auto& element : vector) {
        if (!element.finalized) {
            return false;
        }
    }
    return true;
}

CpuTracer::CpuTracer() { RAYX_VERB << "Initializing Cpu Tracer.."; }

CpuTracer::~CpuTracer() {}

BundleHistory CpuTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rayList = cfg.m_rays;

    // CFG meta passed through pushConstants

    CPU_TRACER::elements.data.clear();
    CPU_TRACER::xyznull.data.clear();
    CPU_TRACER::matIdx.data.clear();
    CPU_TRACER::mat.data.clear();

    // init rayData
    CPU_TRACER::rayData.data = rayList;

    // Prepare rayMeta data in-between batch traces
    CPU_TRACER::rayMetaData.data.clear();
    std::vector<RayMeta> rayMeta;
    rayMeta.reserve((size_t)cfg.m_numRays);
    const uint64_t MAX_UINT64 = ~(uint64_t(0));
    uint64_t workerCounterNum = MAX_UINT64 / uint64_t(cfg.m_numRays);
    for (auto i = 0; i < cfg.m_numRays; i++) {
        CPU_TRACER::rayMetaData.data.push_back(
            {.ctr = ((uint64_t)cfg.m_rayIdStart + (uint64_t)i) * workerCounterNum + uint64_t(cfg.m_randomSeed * MAX_UINT64),
             .nextElementId = -1,   // Intersection element unknown / does not exist
             .finalized = false});  // Not started
    }
    // init elements
    for (auto e : cfg.m_elements) {
        CPU_TRACER::elements.data.push_back(e);
    }

    auto materialTables = cfg.m_materialTables;
    CPU_TRACER::mat.data = materialTables.materialTable;
    CPU_TRACER::matIdx.data = materialTables.indexTable;
    const int maxBounces = CPU_TRACER::elements.data.size();

    BundleHistory events;

    // First stage: Single Trace until max Bounce
    // -------------------------------------------------------------------------------------------------------
    {
        RAYX_PROFILE_SCOPE_STDOUT("singleTracePassCPU");
        for (int b = 0; b < maxBounces; b++) {
            CPU_TRACER::pushConstants.i_bounce = b;

            // SHADER START
            for (uint i = 0; i < rayList.size(); i++) {
                CPU_TRACER::gl_GlobalInvocationID = i;
                CPU_TRACER::eventRecorded = false;
                CPU_TRACER::singleBounce_main();
            }  // SHADER END

            auto rayOut = CPU_TRACER::rayData.data;
            auto rayMeta = CPU_TRACER::rayMetaData.data;

            events.push_back(rayOut);
            if (_allFinalized(rayMeta)) {  // Are all rays finished?
                break;
            }
        }
    }

    // Second stage: Final collision only check
    // -------------------------------------------------------------------------------------------------------
    {
        RAYX_PROFILE_SCOPE_STDOUT("finalCollisionPassCPU");
        // SHADER START
        for (uint i = 0; i < rayList.size(); i++) {
            CPU_TRACER::gl_GlobalInvocationID = i;
            CPU_TRACER::eventRecorded = false;
            CPU_TRACER::finalCollision_main();
        }  // SHADER END

        auto rayOut = CPU_TRACER::rayData.data;  // Fetch Rays back from the Shader "container"
        events.push_back(rayOut);
    }
    return events;
}

void CpuTracer::setPushConstants(const PushConstants* p) { std::memcpy(&CPU_TRACER::pushConstants, p, sizeof(PushConstants)); }
}  // namespace RAYX
