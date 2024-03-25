#include "SimpleTracer.h"

#include <cmath>
#include <cstring>

#include <alpaka/alpaka.hpp>
#include <alpaka/example/ExampleDefaultAcc.hpp>

#include "Beamline/OpticalElement.h"
#include "Material/Material.h"
#include "RAY-Core.h"

#include "Shader/DynamicElements.h"
#include "Shader/InvocationState.h"

#include "AlpakaUtil.h"

namespace {

struct Kernel {
    template <typename TAcc>
    RAYX_FUNC
    void operator() (const TAcc& acc, Inv inv) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < inv.pushConstants.numRays)
            dynamicElements(gid, inv);
    }
};

template <typename Acc>
std::vector<Ray> traceWithAcc(const TraceRawConfig& cfg, const PushConstants& pushConstants) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;

    using Cpu = alpaka::DevCpu;
    const auto [cpu_platform, cpu] = pickFirstDevice<Cpu>();
    const auto [d_platform, acc] = pickFirstDevice<Acc>();

    using QueueProperty = alpaka::NonBlocking;
    using AccQueue = alpaka::Queue<Acc, QueueProperty>;
    auto queue = AccQueue(acc);

    const auto numInputRays = cfg.m_rays.size();
    const auto numOutputRays = numInputRays * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID);
    const auto& materialTables = cfg.m_materialTables;

    using Vec = alpaka::Vec<Dim, Idx>;

    auto rayData = createBuffer<Idx, Vec, Ray>(queue, cfg.m_rays, cpu);
    auto outputRays = createBuffer<Idx, Ray>(queue, Vec{numOutputRays});
    auto elements = createBuffer<Idx, Vec, Element>(queue, cfg.m_elements, cpu);
    auto matIdx = createBuffer<Idx, Vec, int>(queue, materialTables.indexTable, cpu);
    auto mat = createBuffer<Idx, Vec, double>(queue, materialTables.materialTable, cpu);

    auto inv = Inv {
        // shader instance local variables
        .globalInvocationId = {},
        .finalized = {},
        .ctr = {},
        .nextEventIndex = {},

        // buffers
        .rayData = bufToSpan(rayData),
        .outputData = bufToSpan(outputRays),
        .elements = bufToSpan(elements),
        .xyznull = {},
        .matIdx = bufToSpan(matIdx),
        .mat = bufToSpan(mat),

#ifdef RAYX_DEBUG_MODE
        .d_struct = {},
#endif

        // CFG meta passed through pushConstants
        .pushConstants = pushConstants,
    };

    auto workDiv = getWorkDivForAcc<Acc>(numInputRays);
    alpaka::exec<Acc>(
        queue,
        workDiv,
        Kernel{},
        inv
    );

    auto output = std::vector<Ray>(numOutputRays);
    auto outputView = alpaka::createView(cpu, output);
    alpaka::memcpy(queue, outputView, outputRays, Vec{numOutputRays});

    alpaka::wait(queue);
    return output;
}

} // unnamed namespace

namespace RAYX {

SimpleTracer::SimpleTracer(Platform platform) {
    RAYX_VERB << "Initializing Tracer..";

    using Dim = alpaka::DimInt<1>;
    using Idx = int;

    switch (platform) {
        case Platform::Gpu: {
            using Acc = DefaultGpuAcc<Dim, Idx>;

            if constexpr (isAccAvailable<Acc>()) {
                m_traceFn = &traceWithAcc<Acc>;
                break;
            } else {
                RAYX_WARN
                    << "Gpu Tracer was disabled during build."
                    << "Falling back to Cpu Tracer."
                    << "Add '-x' flag on launch to use the Cpu Tracer directly"
                ;
                [[fallthrough]];
            }
        }
        case Platform::Cpu: {
            using Acc = DefaultCpuAcc<Dim, Idx>;
            m_traceFn = &traceWithAcc<Acc>;
        }
    }
}

SimpleTracer::~SimpleTracer() = default;

std::vector<Ray> SimpleTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    return m_traceFn(cfg, m_pushConstants);
}

void SimpleTracer::setPushConstants(const PushConstants* p) {
    std::memcpy(&m_pushConstants, p, sizeof(PushConstants));
}

}  // namespace RAYX
