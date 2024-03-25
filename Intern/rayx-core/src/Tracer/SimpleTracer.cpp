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

namespace {

struct Kernel {
    template <typename TAcc>
    RAYX_FUNC
    void operator() (const TAcc& acc, Inv inv) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < inv.rayData.size())
            dynamicElements(gid, inv);
    }
};

template <typename Acc>
auto pickFirstDevice() {
    const auto platform = alpaka::Platform<Acc>();
    const auto dev = alpaka::getDevByIdx(platform, 0);
    std::cout << "found " << alpaka::getDevCount(platform) << ""
        << " device(s) for platform '" << alpaka::getAccName<Acc>() << "'."
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

template <typename Acc>
constexpr auto getBlockSize() {
    using Idx = alpaka::Idx<Acc>;
    using Dim = alpaka::Dim<Acc>;

    auto impl = [] () -> Idx {
#if defined(ALPAKA_ACC_GPU_CUDA_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccGpuCudaRt<Dim, Idx>>) {
            return 128;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_SEQ_T_THREADS_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuThreads<Dim, Idx>>) {
            return 1;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_OMP2_T_SEQ_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuOmp2Blocks<Dim, Idx>>) {
            return 1;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_SEQ_T_OMP2_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuOmp2Threads<Dim, Idx>>) {
            return 1;
        }
#endif

#if defined(ALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED)
        if constexpr (std::is_same_v<Acc, alpaka::AccCpuSerial<Dim, Idx>>) {
            return 1;
        }
#endif

        return 0;
    };

    constexpr Idx blockSize = impl();
    static_assert(blockSize != static_cast<Idx>(0));

    return blockSize;
}

template <typename Acc>
auto getWorkDivForAcc(alpaka::Idx<Acc> numElements) {
    using Idx = alpaka::Idx<Acc>;
    using Dim = alpaka::Dim<Acc>;

    constexpr int blockSize = getBlockSize<Acc>();
    const int gridSize = (numElements - 1) / blockSize + 1;

    using Vec = alpaka::Vec<Dim, Idx>;
    return alpaka::WorkDivMembers<Dim, Idx> {
        Vec{gridSize},
        Vec{blockSize},
        Vec{1},
    };
}

template <typename Dim, typename Idx>
struct PlatformNotSupported;

    template <typename Dim, typename Idx>
#if defined(ALPAKA_ACC_GPU_CUDA_ENABLED) || defined(ALPAKA_ACC_GPU_HIP_ENABLED)
    using GpuAcc = alpaka::AccGpuCudaRt<Dim, Idx>;
#else
    using GpuAcc = PlatformNotSupported<Dim, Idx>;
#endif

    template <typename Dim, typename Idx>
#if defined(ALPAKA_ACC_CPU_B_OMP2_T_SEQ_ENABLED)
    using CpuAcc = alpaka::AccCpuOmp2Blocks<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_SEQ_T_OMP2_ENABLED)
    using CpuAcc = alpaka::AccCpuOmp2Threads<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_TBB_T_SEQ_ENABLED)
    using CpuAcc = alpaka::AccCpuTbbBlocks<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_SEQ_T_THREADS_ENABLED)
    using CpuAcc = alpaka::AccCpuThreads<Dim, Idx>;
#elif defined(ALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED)
    using CpuAcc = alpaka::AccCpuSerial<Dim, Idx>;
#else
    using CpuAcc = PlatformNotSupported<Dim, Idx>;
#endif

template <typename Dim, typename Idx>
constexpr bool isPlatformSupported(TracerPlatform platform) {
    if (platform == TracerPlatform::Cpu) {
        return !std::is_same_v<CpuAcc<Dim, Idx>, PlatformNotSupported<Dim, Idx>>;
    } else {
        return !std::is_same_v<GpuAcc<Dim, Idx>, PlatformNotSupported<Dim, Idx>>;
    }
}

template <typename Acc>
std::vector<Ray> traceWithAcc(const TraceRawConfig& cfg, const PushConstants& pushConstants) {
    static_assert(alpaka::isAccelerator<Acc>);

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

using Dim = alpaka::DimInt<1>;
using Idx = int;

} // unnamed namespace

namespace RAYX {

SimpleTracer::SimpleTracer(TracerPlatform platform) {
    RAYX_VERB << "Initializing Tracer..";
    m_platform = platform;

    switch (m_platform) {
    case TracerPlatform::Cpu:
        if constexpr (!isPlatformSupported<Dim, Idx>(TracerPlatform::Cpu))
            RAYX_ERR << "Cpu tracer was not enabled during build.";
        break;
    case TracerPlatform::Gpu:
        if constexpr (!isPlatformSupported<Dim, Idx>(TracerPlatform::Gpu))
            RAYX_ERR << "Gpu tracer was not enabled during build.";
        break;
    }
}

SimpleTracer::~SimpleTracer() = default;

std::vector<Ray> SimpleTracer::traceRaw(const TraceRawConfig& cfg) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    switch (m_platform) {
    case TracerPlatform::Cpu:
        if constexpr (isPlatformSupported<Dim, Idx>(TracerPlatform::Cpu))
            return traceWithAcc<CpuAcc<Dim, Idx>>(cfg, m_pushConstants);
        break;
    case TracerPlatform::Gpu:
        if constexpr (isPlatformSupported<Dim, Idx>(TracerPlatform::Gpu))
            return traceWithAcc<GpuAcc<Dim, Idx>>(cfg, m_pushConstants);
        break;
    }

    assert(false);
    return {}; // TODO(Sven): check unreachable
}

void SimpleTracer::setPushConstants(const PushConstants* p) {
    std::memcpy(&m_pushConstants, p, sizeof(PushConstants));
}

}  // namespace RAYX
