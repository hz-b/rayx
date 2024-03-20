#include "CpuTracer.h"

#include <cmath>
#include <cstring>

#include "Beamline/OpticalElement.h"
#include "Material/Material.h"
#include "RAY-Core.h"
#include "Shader/DynamicElements.h"
#include "Shader/InvocationState.h"

namespace {

struct Kernel {
    template <typename TAcc>
    RAYX_FUNC
    void operator() (const TAcc& acc, Inv inv) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];
        dynamicElements(gid, inv);
    }
};

template <typename Dev>
auto pickFirstDevice() {
    const auto platform = alpaka::Platform<Dev>();
    const auto dev = alpaka::getDevByIdx(platform, 0);
    std::cout << "found " << alpaka::getDevCount(platform) << ""
        << " device(s) for platform '" << alpaka::getAccName<Dev>() << "'."
        << " picking '" << alpaka::getName(dev) << "'"
        << std::endl
    ;
    return std::make_tuple(platform, dev);
};

template <typename TIdx, typename T, typename TQueue, typename TExtent>
auto createBuffer(TQueue& queue, TExtent size) {
    return alpaka::allocAsyncBufIfSupported<T, TIdx>(queue, size);
}

template <typename TIdx, typename TExtent, typename T, typename TQueue, typename Cpu>
auto createBuffer(TQueue& queue, const std::vector<T>& data, const Cpu& cpu) {
    auto dataView = alpaka::createView(cpu, data);
    auto buf = alpaka::allocAsyncBufIfSupported<T, TIdx>(queue, TExtent{data.size()});
    alpaka::memcpy(queue, buf, dataView, TExtent{data.size()});
    return buf;
}

template <typename TBuf>
auto bufToSpan(TBuf& buf) {
    return std::span(alpaka::getPtrNative(buf), alpaka::getExtents(buf)[0]);
}

}

namespace RAYX {

CpuTracer::CpuTracer() {
    RAYX_VERB << "Initializing Cpu Tracer..";
}

CpuTracer::~CpuTracer() = default;

std::vector<Ray> CpuTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    using Dim = alpaka::DimInt<1>;
    using Idx = int;

    using Cpu = alpaka::DevCpu;
    const auto [cpu_platform, cpu] = pickFirstDevice<Cpu>();
    using Acc = alpaka::AccCpuSerial<Dim, Idx>;
    const auto [d_platform, acc] = pickFirstDevice<Acc>();

    using QueueProperty = alpaka::NonBlocking;
    using AccQueue = alpaka::Queue<Acc, QueueProperty>;
    auto queue = AccQueue(acc);

    const auto numInputRays = cfg.m_rays.size();
    const auto numOutputRays = numInputRays * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID);
    const auto& materialTables = cfg.m_materialTables;

    using Vec = alpaka::Vec<Dim, Idx>;
    const auto numShaderInstances = Vec{numInputRays};

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

        // CFG meta passed through pushConstants
        .pushConstants = m_pushConstants,
    };

    alpaka::exec<Acc>(
        queue,
        alpaka::getValidWorkDiv<Acc>(
            acc,
            numShaderInstances,
            Vec{1}
        ),
        Kernel{},
        inv
    );

    auto output = std::vector<Ray>(numOutputRays);
    auto outputView = alpaka::createView(cpu, output);
    alpaka::memcpy(queue, outputView, outputRays, Vec{numOutputRays});

    alpaka::wait(queue);

    return output;
}

void CpuTracer::setPushConstants(const PushConstants* p) {
    std::memcpy(&m_pushConstants, p, sizeof(PushConstants));
}

}  // namespace RAYX
