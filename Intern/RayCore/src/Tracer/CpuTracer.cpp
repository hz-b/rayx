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
//#include "shader/singleBounce.comp"
#include "shader/main.comp"

}  // namespace CPU_TRACER

CpuTracer::CpuTracer() { RAYX_VERB << "Initializing Cpu Tracer.."; }

CpuTracer::~CpuTracer() {}

bool _allFinalized(const std::vector<RayMeta>& vector) {
    for (const auto& element : vector) {
        if (!element.finalized) {
            return false;
        }
    }
    return true;
}

void _printRayStats(const std::vector<Ray>& rayOut) {
    int _hit = 0;
    int _abs = 0;
    int _fly = 0;
    int _unin = 0;
    int _not = 0;

    for (const auto& r : rayOut) {
        if ((int)r.m_weight == (int)W_JUST_HIT_ELEM) {
            _hit++;
        } else if ((int)r.m_weight == (int)W_ABSORBED) {
            _abs++;
        } else if ((int)r.m_weight == (int)W_FLY_OFF) {
            _fly++;
        } else if ((int)r.m_weight == (int)W_UNINIT) {
            _unin++;
        } else if ((int)r.m_weight == (int)W_NOT_ENOUGH_BOUNCES) {
            _not++;
        }
    }
    // std::erase_if(rayOut, [&](auto& r) { return r.m_weight == W_UNINIT; });
    double hit = (double)(_hit) / rayOut.size();
    double abs = (double)(_abs) / rayOut.size();
    double fly = (double)(_fly) / rayOut.size();
    double unin = (double)(_unin) / rayOut.size();
    double notx = (double)(_not) / rayOut.size();
    RAYX_D_LOG << "_hit: " << hit << " _abs: " << abs << " _fly: " << fly << " _unin: " << unin << " _not: " << notx;
    RAYX_D_LOG << "===============";
}

std::vector<Ray> CpuTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rayList = cfg.m_rays;

    // CFG meta passed through pushConstants

    CPU_TRACER::elements.data.clear();
    CPU_TRACER::xyznull.data.clear();
    CPU_TRACER::matIdx.data.clear();
    CPU_TRACER::mat.data.clear();
    CPU_TRACER::outputData.data.resize(rayList.size() * (size_t)cfg.m_maxSnapshots);

    // init rayData, rayMeta
    CPU_TRACER::rayData.data = rayList;

    for (size_t i = 0; i < rayList.size(); i++) {
        const uint64_t MAX_UINT64 = ~(uint64_t(0));
        uint64_t workerCounterNum = MAX_UINT64 / uint64_t(cfg.m_numRays);
        for (auto i = 0; i < cfg.m_numRays; i++) {
            CPU_TRACER::rayMetaData.data.push_back(
                {.ctr = ((uint64_t)cfg.m_rayIdStart + (uint64_t)i) * workerCounterNum + uint64_t(cfg.m_randomSeed * MAX_UINT64),
                 .nextElementId = 0,    // Intersection element unknown / does not exist
                 .finalized = false});  // Not started
        }
    }

    // init elements
    for (auto e : cfg.m_elements) {
        CPU_TRACER::elements.data.push_back(e);
    }

    auto materialTables = cfg.m_materialTables;
    CPU_TRACER::mat.data = materialTables.materialTable;
    CPU_TRACER::matIdx.data = materialTables.indexTable;

    const int maxBounces = CPU_TRACER::elements.data.size();
    std::vector<std::vector<Ray>> _checkpoints;

    // for (int b = 0; b < maxBounces; b++) {
    // Run the tracing by for all rays
    // CPU_TRACER::pushConstants.i_bounce = b;

    // SHADER START
    for (uint i = 0; i < rayList.size(); i++) {
        CPU_TRACER::gl_GlobalInvocationID = i;
        // CPU_TRACER::snapshotTaken = false;
        CPU_TRACER::main();
    }  // SHADER END

    auto rayOut = CPU_TRACER::outputData.data;
    auto rayMeta = CPU_TRACER::rayMetaData.data;

    //     _printRayStats(rayOut);

    //     // std::erase_if(rayOut, [](auto r) { return r.m_weight != W_UNINIT; });
    //     _checkpoints.push_back(rayOut);
    //     RAYX_DBG(rayOut.size());
    //     if (_allFinalized(rayMeta)) {  // Are all rays finished?
    //         RAYX_VERB << "All finalized";
    //         break;
    //     }
    // }

    return rayOut;
}

void CpuTracer::setPushConstants(const PushConstants_t* p) { std::memcpy(&CPU_TRACER::pushConstants, p, sizeof(CPU_TRACER::pushConstants)); }
}  // namespace RAYX
