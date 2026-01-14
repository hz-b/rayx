#pragma once

namespace rayx::detail::host {

/*
 * host layer types
 */

using SeparateValues         = design::SeparateValues<double>;
using WhiteNoiseDistribution = design::WhiteNoiseDistribution<double>;
using GaussianDistribution   = design::GaussianDistribution<double>;

template <Ctx_c TCtx>
struct BakedDistribution {
    Buf<TCtx, double> values;
    Buf<TCtx, double> weights;
    int numValues;
    bool interpolate;
};

/*
 * toDevice functions
 */

template <Ctx_c TCtx, typename T>
SeparateValues toDevice(TCtx&, const design::SeparateValues<T>& distribution) {
    return SeparateValues{
        .center    = toDouble(distribution.center),
        .range     = toDouble(distribution.range),
        .numValues = distribution.numValues,
    };
}

template <typename T>
WhiteNoiseDistribution toDevice(TCtx&, const design::WhiteNoiseDistribution<T>& distribution) {
    return WhiteNoiseDistribution{
        .center = toDouble(distribution.center),
        .range  = toDouble(distribution.range),
    };
}

template <typename T>
GaussianDistribution toDevice(TCtx&, const design::GaussianDistribution<T>& distribution) {
    return GaussianDistribution{
        .mean              = toDouble(distribution.mean),
        .standardDeviation = toDouble(distribution.standardDeviation),
    };
}

template <Ctx_c TCtx, typename T>
BakedDistribution<TCtx> toDevice(TCtx& ctx, const design::BakedDistribution<T>& distribution) {
    assert(distribution.values.size() > 0 && "BakedDistribution must have at least one value");
    assert(!distribution.weights ||
           distribution.weights->size() == distribution.values.size() && "BakedDistribution weights size must match values size");

    const int size       = static_cast<int>(distribution.values.size());
    const auto d_values  = ctx.alloc<double>(size);
    const auto d_weights = ctx.alloc<double>(size);

    alpaka::memcpy(ctx.accDev(), alpaka::getPtrNative(d_values), alpaka::createView(ctx.hostDev(), distribution.values.data(), size), size);

    if (distribution.weights) {
        alpaka::memcpy(ctx.accDev(), alpaka::getPtrNative(d_weights), alpaka::createView(ctx.hostDev(), distribution.weights->data(), size), size);
    } else {
        // if no weights are provided, use uniform weights
        std::vector<double> uniformWeights(size, 1.0);
        alpaka::memcpy(ctx.accDev(), alpaka::getPtrNative(d_weights), alpaka::createView(ctx.hostDev(), uniformWeights.data(), size), size);
    }

    return BakedDistribution<TCtx>{
        .values      = d_values,
        .weights     = d_weights,
        .numValues   = size,
        .interpolate = distribution.interpolate,
    };
}

/*
 * job launch functions
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

template <CtxCpuStd_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const BakedDistribution<typename TCtx::AccDev>& distribution, const int n) {
    auto dst = ctx.allocBuf<double>(que, n);
    // TODO: implement BakedDistributionKernel using std algorithms
    return dst;
}

template <CtxGpuCuda_c TCtx>
Buf_t<TCtx, double> process(TCtx& ctx, Que_c auto& que, const BakedDistribution<typename TCtx::AccDev>& distribution, const int n) {
    auto dst = ctx.allocBuf<double>(que, n);
    // TODO: implement BakedDistributionKernel using thrust algorithms
    return dst;
}

}  // namespace rayx::detail::host
