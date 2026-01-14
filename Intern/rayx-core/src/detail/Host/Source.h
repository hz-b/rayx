#pragma once

namespace rayx::detail::device {

// one of : TagCpuOmp2Blocks, TagCpuOmp2Threads, TagCpuSerial, TagCpuTbbBlocks, TagCpuThreads, TagGpuSyclIntel,
template <typename T>
concept TagAccCpu =
    std::same_as<T, alpaka::TagAccCpuOmp2Blocks> || std::same_as<T, alpaka::TagAccCpuOmp2Threads> || std::same_as<T, alpaka::TagAccCpuSerial> ||
    std::same_as<T, alpaka::TagAccCpuTbbBlocks> || std::same_as<T, alpaka::TagAccCpuThreads>;

template <typename T>
concept TagAccGpuCudaRt = std::same_as<T, alpaka::TagAccGpuCudaRt>;

template <typename Acc>
concept AccCpu = TagAccCpu<alpaka::acctotag<Acc>>;

template <typename T>
concept AccGpuCuda = TagAccGpuCudaRt<typename T::TagAcc>;

template <typename Acc, typename T>
concept QueueCpu = AccCpu<typename alpaka::acc<alpaka::Queue<Acc, T>>::type>;

template <typename Acc, typename T>
concept QueueGpuCuda = AccGpuCuda<typename alpaka::acc<alpaka::Queue<Acc, T>>::type>;

}  // namespace rayx::detail::device

double toDouble(double value) { return value; }

double toDouble(Degrees degrees) { return degreesToRadians(degrees.value); }

double toDouble(Radians radians) { return radians.value; }

double toDouble(Angle angle) {
    return std::visit([](auto angle) { return toDouble(angle); });
}

template <typename T>
concept SimpleDistribution = std::same_as<T, SeparateValues> || std::same_as<T, WhiteNoiseDistribution> || std::same_as<T, GaussianDistribution>;

struct SimpleDistributionKernel {
    ALPAKA_FN_ACC
    void operator()(const auto& __restrict acc, double* __restrict dst, RandCounter* __restrict randCounters,
                    const SimpleDistribution& __restrict distribution, const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc);
        if (n <= gid) return;

        auto rand         = Rand(randCounters[gid]);
        dst[gid]          = process(distribution, rand);
        randCounters[gid] = rand.counter;
    }
};

Allocation<double> process(const auto& devAcc, const auto& q, const SimpleDistribution& distribution, Allocator& allocator, const int n,
                           RandCounter* randCounters) {}

template <Ctx_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const design::Distribution<double>& distribution, RandCounter* randCounters, int n) {
    return std::visit([&](const auto& dist) { return process(ctx, que, toHost(dist), randCounters, n); }, distribution);
}

template <Ctx_c TCtx, Que_c TQue>
Rays<Ctx::Acc> process(TCtx& ctx, TQue& que, const PointSource& source) {
    Rays<Ctx::Acc> rays;
    rays.numRays = source.numRays;

    rays.rand_counters = ctx.alloc<RandCounter>(source.numRays);

    auto que_a = ctx.createQue();

    auto horizontalAngles = process(ctx, que_a, source.rayAngle.x, rays.rand_counters.get(), source.numRays);
    auto verticalAngles   = process(ctx, que_a, source.rayAngle.y, rays.rand_counters.get(), source.numRays);

    auto que_b = ctx.createQue(que_a);

    processAngleToDirection(ctx, queues_b, horizontalAngles, verticalAngles, rays.direction_x, rays.direction_y, rays.direction_z, source.numRays);

    rays.position_x = process(ctx, queues_a, source.rayOrigin.x, rays.rand_counters.get(), source.numRays);
    rays.position_y = process(ctx, queues_a, source.rayOrigin.y, rays.rand_counters.get(), source.numRays);
    rays.position_z = process(ctx, queues_a, source.rayOrigin.z, rays.rand_counters.get(), source.numRays);

    rays.energy = process(ctx, queues_a, source.rayEnergy, rays.rand_counters.get(), source.numRays);

    processPolarizationToElectricField(ctx, queues_a, source.rayPolarization, rays.electric_field_x, rays.electric_field_y, rays.electric_field_z,
                                       source.numRays);

    rays.optical_path_length = ctx.alloc<double>(rays.numRays);
    alpaka::memset(ctx.devAcc(), queues_a.getQueue(), rays.optical_path_length.get(), 0.0, rays.numRays);
}

void trace(const Group& group) {
    for (const SourceNode* sourceNode : group.getSources()) {
        const auto& source = sourceNode.source;
        std::visit([](const auto& source) { process(source); }, source);
    }
}
