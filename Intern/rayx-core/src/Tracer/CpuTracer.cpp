#include "CpuTracer.h"

#include <cmath>
#include <cstring>

#include "Beamline/OpticalElement.h"
#include "Material/Material.h"
#include "RAY-Core.h"
#include "KokkosUtils.h"
#include "Shader/DynamicElements.h"
#include "Shader/InvocationState.h"

using uint = unsigned int;

struct Kernel {
    Inv inv;

    KOKKOS_INLINE_FUNCTION
    void operator() (int gid) const {
        dynamicElements(gid, inv);
    }
};

namespace RAYX {

CpuTracer::CpuTracer() {
    RAYX_VERB << "Initializing Cpu Tracer..";
}

CpuTracer::~CpuTracer() = default;

std::vector<Ray> CpuTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // CFG meta passed through pushConstants

    auto numInputRays = cfg.m_rays.size();
    auto numOutputRays = numInputRays * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID);
    const auto& materialTables = cfg.m_materialTables;

    using ExecSpace = Kokkos::DefaultHostExecutionSpace;
    using MemSpace = ExecSpace::memory_space;
    using Util = KokkosUtils<MemSpace>;

    auto inv = Inv<MemSpace> {
        .globalInvocationId = {},
        .finalized = {},
        .ctr = {},
        .nextEventIndex = {},
        .rayData = Util::createView("CpuTracer_rayData", cfg.m_rays),
        .outputData = Util::createView<Ray>("CpuTracer_outputData", numOutputRays),
        .elements = Util::createView("CpuTracer_elements", cfg.m_elements),
        .xyznull = {},
        .matIdx = Util::createView("CpuTracer_matIdx", materialTables.indexTable),
        .mat = Util::createView("CpuTracer_mat", materialTables.materialTable),
        .pushConstants = m_pushConstants,
    };

    auto ex = ExecSpace();
    Kokkos::parallel_for(
        Kokkos::RangePolicy<ExecSpace>(
            ex,
            0,           // begin
            numInputRays // end
        ),
        Kernel { inv }
    );
    ex.fence();

    // Fetch Rays back from the Shader "container"
    return Util::createVector(inv.outputData);
}

void CpuTracer::setPushConstants(const PushConstants* p) {
    std::memcpy(&m_pushConstants, p, sizeof(PushConstants));
}

}  // namespace RAYX
