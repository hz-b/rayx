#pragma once

#include <alpaka/alpaka.hpp>

#include "Allocator.h"
#include "QueuePool.h"

namespace rayx::detail::host {

template <typename TAccTag>
class Ctx {
    privte : Ctx() {}

  public:
    Ctx(const Ctx&)            = delete;
    Ctx(Ctx&&)                 = default;
    Ctx& operator=(const Ctx&) = delete;
    Ctx& operator=(Ctx&&)      = default;

    using AccTag = TAccTag;
    using AccDev = alpaka::TagToAcc<AccTag, alpaka::DimInt<1>, int>;
    using Queue  = alpaka::Queue<AccDev, alpaka::Blocking>;  // TODO: change to NonBlocking

    static Ctx createInitialCtx(int deviceIndex, uint64_t seed) {
        Ctx ctx;
        ctx.m_dev         = alpaka::getDevByIdx<AccDev>(deviceIndex);
        ctx.m_allocator   = std::make_shared<Allocator<AccDev>>(deviceIndex);
        ctx.m_seedCounter = std::make_shared<SeedCounter>(seed);
        ctx.m_queuePool   = QueuePool<Queue>(ctx.m_dev);
        return ctx;
    }

    // TODO: enable these methods when needed
    // static Ctx createCtxForSameDevice(const Ctx& self) const {
    //     Ctx other;
    //     other.m_dev          = self.m_dev;
    //     other.m_allocator    = self.m_allocator;
    //     other.m_seedCounter  = self.m_seedCounter;
    //     other.m_queuePool    = QueuePool<Queue>(m_dev);
    //     return other;
    // }
    //
    // template <typename TAccTag = AccTag>
    // static Ctx<TAccTag> createCtxForDifferentDevice(const Ctx& self, int deviceIndex) {
    //     Ctx<TAccTag> other;
    //     using AccDev = typename Ctx::AccDev;
    //     auto other = Ctx();
    //     other.m_dev          = alpaka::getDevByIdx<AccDev>(deviceIndex);
    //     other.m_allocator    = std::make_shared<Allocator<AccDev>>(deviceIndex);
    //     other.m_seedCounter  = self.m_seedCounter;
    //     other.m_queuePool    = QueuePool<Queue>(m_dev);
    //     return other;
    // }

    const AccDev& dev() const { return m_dev; }
    const uint64_t nextSeed() { return m_seedCounter++; }

    template <typename T>
    Buf_t<Ctx, T> alloc(int size) {
        return m_allocator->alloc<T>(m_dev, size);
    }

    Que_c createQue() { return m_queuePool.createQue(); }

  private:
    AccDev m_dev;
    std::shared_ptr<Allocator<AccTag>> m_allocator;
    std::shared_ptr<SeedCounter> m_seedCounter;
    QueuePool<Queue> m_queuePool;
};

template <typename T>
struct isCtx : std::false_type {};

template <typename TAccTag>
struct isCtx<Ctx<TAccTag>> : std::true_type {};

template <typename T>
constexpr bool isCtx_v = isCtx<T>::value;

template <typename T>
concept Ctx_c = isCtx_v<T>;

template <typename TAccTag, typename T>
struct isCtxOfAccTag : std::false_type {};

template <typename TAccTag, typename T>
struct isCtxOfAccTag<Ctx<TAccTag>, T> : std::bool_constant<std::same_as<typename Ctx<TAccTag>::AccTag, T>> {};

template <typename TAccTag, typename T>
constexpr bool isCtxOfAccTag_v = isCtxOfAccTag<TAccTag, T>::value;

template <typename TAccTag, typename T>
concept CtxOfAccTag_c = isCtxOfAccTag_v<TAccTag, T>;

template <typename T>
concept CtxCpuStd_c = CtxOfAccTag_c<alpaka::TagAccCpuStd, T>;

template <typename T>
concept CtxGpuCuda_c = CtxOfAccTag_c<alpaka::TagAccGpuCudaRt, T>;

}  // namespace rayx::detail::host
