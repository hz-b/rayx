#pragma once

namespace rayx::detail::device {

using SeparateValues         = host::SeparateValues<double>;
using WhiteNoiseDistribution = host::WhiteNoiseDistribution<double>;
using GaussianDistribution   = host::GaussianDistribution<double>;

struct BakedDistribution {
    double* values;
    double* weights;
    int numValues;
    bool interpolate;
};

BakedDistribution toDevice(const host::BakedDistribution<T>& distribution) {
    return BakedDistribution{
        .values      = alpaka::getPtrNative(distribution.values),
        .weights     = alpaka::getPtrNative(distribution.weights),
        .numValues   = distribution.numValues,
        .interpolate = distribution.interpolate,
    };
}

/*
 * device layer functions
 */

ALPAKA_FN_ACC inline double process(const SeparateValuesDistribution& distribution, const RandCounter rand) {
    return distribution.center - distribution.range * 0.5 +
           randomIntInRange(0, distribution.numValues) / static_cast<double>(distribution.numValues - 1) * distribution.range;
}

ALPAKA_FN_ACC inline double process(const WhiteNoiseDistribution& distribution, const RandCounter rand) {
    return distribution.center + 0.5 * rand.randomDoubleInRange(-distribution.range, distribution.range);
}

ALPAKA_FN_ACC inline double process(const GaussianDistribution& distribution, const RandCounter rand) {
    return rand.randomDoubleNormalDistributed(distribution.mean, distribution.standardDeviation);
}

/*
 * kenrels
 */

// TODO: consider using shared memory for performance improvement

// TIPS BY COPILOT:
// see e.g. https://developer.nvidia.com/blog/using-shared-memory-cuda-cc/
// this is how i would do it:
// https://stackoverflow.com/questions/24476287/cuda-shared-memory-for-random-number-generation
// but it needs to be adapted to alpaka
// also consider vectorization on cpu
// see e.g.
// https://developer.arm.com/documentation/100069/0610/Using-SIMD-instructions/Using-SIMD-instructions-in-C-and-C++/Vectorization-in-C-and-C++-code
// for cpu we can use e.g. glm::vec4 or glm::dvec4 for vectorization
// for gpu we can use e.g. float4 or double4
// this needs to be tested and benchmarked
// also consider using thrust for gpu
// see e.g. https://thrust.github.io/doc/group__random.html
// for cpu we can use e.g. std::random or boost::random
// see e.g. https://en.cppreference.com/w/cpp/numeric/random
// but this needs to be tested and benchmarked
// also consider using curand for gpu
// see e.g. https://docs.nvidia.com/cuda/curand/index.html
// but this needs to be tested and benchmarked

constexpr int numElementsPerThread = 8;

template <typename TAcc>
struct SeparateValuesDistributionKernel {
    static constexpr int numElementsPerThread = numElementsPerThread;

    ALPAKA_FN_ACC
    void operator()(const auto& __restrict acc, double* __restrict dst, const SeparateValuesDistribution distribution, const uint64_t seed,
                    const int n) const {
        const int gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];
        auto rand     = Rand(acc, seed);

        if (n <= gid * numElementsPerThread) return;

        for (int i = 0; i < numElementsPerThread; ++i) {
            const int idx = gid * numElementsPerThread + i;
            dst[idx]      = process(distribution, rand);
        }
    }
};

template <typename TAcc>
struct WhiteNoiseDistributionKernel {
    static constexpr int numElementsPerThread = numElementsPerThread;

    ALPAKA_FN_ACC
    void operator()(const auto& __restrict acc, double* __restrict dst, const WhiteNoiseDistribution distribution, const uint64_t seed,
                    const int n) const {
        const int gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];
        auto rand     = Rand(acc, seed);

        if (n <= gid * numElementsPerThread) return;

        for (int i = 0; i < numElementsPerThread; ++i) {
            const int idx = gid * numElementsPerThread + i;
            dst[idx]      = process(distribution, rand);
        }
    }
};

template <typename TAcc>
struct GaussianDistributionKernel {
    static constexpr int numElementsPerThread = numElementsPerThread;

    ALPAKA_FN_ACC
    void operator()(const auto& __restrict acc, double* __restrict dst, const GaussianDistribution distribution, const uint64_t seed,
                    const int n) const {
        const int gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];
        auto rand     = Rand(acc, seed);

        if (n <= gid * numElementsPerThread) return;

        for (int i = 0; i < numElementsPerThread; ++i) {
            const int idx = gid * numElementsPerThread + i;
            dst[idx]      = process(distribution, rand);
        }
    }
};

/*
 * kernel execution functions
 */

template <Ctx_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const SeparateValuesDistribution& distribution, const int n) {
    auto dst = ctx.allocBuf<double>(que, n);
    execWithValidWorkDiv<Acc_t<TCtx>>(ctx, que, n, BlockSizeConstraint::None{}, SeparateValuesDistributionKernel{}, alpaka::getPtrNative(dst),
                                      toDevice(distribution), alpaka::getPtrNative(randCounters), n);
    return dst;
}

template <Ctx_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const WhiteNoiseDistribution& distribution, const int n) {
    auto dst = ctx.allocBuf<double>(que, n);
    execWithValidWorkDiv<Acc_t<TCtx>>(ctx, que, n, BlockSizeConstraint::None{}, WhiteNoiseDistributionKernel{}, alpaka::getPtrNative(dst),
                                      toDevice(distribution), alpaka::getPtrNative(randCounters), n);
    return dst;
}

template <Ctx_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const GaussianDistribution& distribution, const int n) {
    auto dst = ctx.allocBuf<double>(que, n);
    execWithValidWorkDiv<Acc_t<TCtx>>(ctx, que, n, BlockSizeConstraint::None{}, GaussianDistributionKernel{}, alpaka::getPtrNative(dst),
                                      toDevice(distribution), alpaka::getPtrNative(randCounters), n);
    return dst;
}

template <CtxCpuOmp_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const BakedDistribution& distribution, const int n) {
    auto dst = ctx.allocBuf<double>(que, n);
    // TODO: implement BakedDistributionKernel using std algorithms
    return dst;
}

template <CtxGpuCuda_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const BakedDistribution& distribution, const int n) {
    auto dst = ctx.allocBuf<double>(que, n);
    // TODO: implement BakedDistributionKernel using thrust algorithms
    return dst;
}

}  // namespace rayx::detail::device
