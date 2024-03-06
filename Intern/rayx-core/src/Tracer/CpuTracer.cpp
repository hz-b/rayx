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

    using ExecSpace = Kokkos::DefaultHostExecutionSpace;
    using MemorySpace = Kokkos::SharedSpace;
    using Util = KokkosUtils<MemorySpace>;

    inv.rayData = Util::createView("CpuTracer_rayData", cfg.m_rays);
    inv.elements = Util::createView("CpuTracer_elements", cfg.m_elements);

    const auto& materialTables = cfg.m_materialTables;
    inv.mat = Util::createView("CpuTracer_mat", materialTables.materialTable);
    inv.matIdx = Util::createView("CpuTracer_matIdx", materialTables.indexTable);

    inv.outputData = Util::createView<Ray>("CpuTracer_outputData", numOutputRays);

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

void CpuTracer::setPushConstants(const PushConstants* p) { std::memcpy(&inv.pushConstants, p, sizeof(PushConstants)); }

}  // namespace RAYX
