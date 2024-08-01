#pragma once

#include <cmath>
#include <cstring>

#include "Beamline/Beamline.h"
#include "DeviceTracer.h"
#include "Gather.h"
#include "Material/Material.h"
#include "RAY-Core.h"
#include "Random.h"
#include "Scan.h"
#include "Shader/DynamicElements.h"
#include "Util.h"

namespace {

struct DynamicElementsKernel {
    template <typename Acc>
    RAYX_FN_ACC void operator()(const Acc& acc, RAYX::InvState inv) const {
        using Idx = alpaka::Idx<Acc>;
        const Idx gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < static_cast<Idx>(inv.inputRays.size())) dynamicElements(gid, inv);
    }
};

}  // unnamed namespace

namespace RAYX {

/**
 * @brief SimpleTracer sequentially executes tracing in batches the CPU or GPU
 */
template <typename TAcc>
class SimpleTracer : public DeviceTracer {
  private:
    using Acc = TAcc;
    using Cpu = alpaka::DevCpu;

    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    using Vec = alpaka::Vec<Dim, Idx>;

    using QueueProperty = alpaka::NonBlocking;
    using Queue = alpaka::Queue<Acc, QueueProperty>;

  public:
    SimpleTracer(int deviceIndex);

    DeviceTracer::BatchOutput traceBatch(const DeviceTracer::BeamlineInputPtr& beamlineInput, const DeviceTracer::BatchInput& batchInput) override;

  private:
    struct TraceResult {
        int totalEventsCount;
    };

    /// Buffer references a buffer on a device and stores the size of that buffer.
    template <typename T>
    struct Buffer {
        using Buf = alpaka::Buf<Acc, T, Dim, Idx>;
        using OptBuf = std::optional<Buf>;

        OptBuf buf;
        Idx size;
    };

    const int m_deviceIndex;

    /// BeamlineInput contains beamline data, that is constant across all batches
    struct BeamlineInput {
        Buffer<Element> elements;
        Buffer<int> materialIndices;
        Buffer<double> materialData;
    } m_beamlineInput;

    /// BatchINput contains data corresponding to a single batch
    /// The data is stored on the accelerator device
    struct BatchInput {
        Buffer<Ray> rays;
    } m_batchInput;

    /// BatchOutput contains data corresponding to a single batch
    /// The data is stored on the accelerator device
    struct BatchOutput {
        // compact events are compact output rays, thus no unused slots are in between.
        Buffer<Idx> compactEventCounts;
        Buffer<Idx> compactEventOffsets;
        Buffer<Ray> compactEvents;
        // events is a buffer with capacity to hold a number of rays: maxEvents * batchSize
        Buffer<Ray> events;
    } m_batchOutput;

    /// BatchOutput contains data corresponding to a single batch
    /// The data is stored on the host
    /// The contents of the BatchResult contain the resulting events from a batch
    struct BatchResult {
        std::vector<Idx> compactEventCounts;
        std::vector<Idx> compactEventOffsets;
        std::vector<Ray> compactEvents;
    } m_batchResult;

    template <typename T>
    void resizeBufferIfNeeded(Queue q, Buffer<T>& buffer, const Idx size);

    template <typename T>
    void transferToBuffer(Queue q, alpaka::DevCpu cpu, Buffer<T>& dst, const T* src, const Idx size);
    template <typename T>
    void transferToBuffer(Queue q, alpaka::DevCpu cpu, Buffer<T>& dst, const std::vector<T>& src, const Idx size);

    template <typename T>
    void transferFromBuffer(Queue q, alpaka::DevCpu cpu, T* dst, Buffer<T>& src, const Idx size);
    template <typename T>
    void transferFromBuffer(Queue q, alpaka::DevCpu cpu, std::vector<T>& dst, Buffer<T>& src, const Idx size);

    template <typename T>
    std::span<T> bufferToSpan(Buffer<T>& buffer);

    TraceResult traceBatchOnDevice(Queue q, const Idx batchSize);

    DeviceTracer::BeamlineInputPtr m_prevBeamlineInput = nullptr;
};

template <typename Acc>
SimpleTracer<Acc>::SimpleTracer(int deviceIndex) : m_deviceIndex(deviceIndex) {}

template <typename Acc>
DeviceTracer::BatchOutput SimpleTracer<Acc>::traceBatch(const DeviceTracer::BeamlineInputPtr& beamlineInput,
                                                        const DeviceTracer::BatchInput& batchInput) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    const auto seq = Sequential::No;
    const auto batchSize = batchInput.raysSize;
    const auto maxEvents = 32;
    const auto startEventID = 0;

    const auto cpu = getDevice<Cpu>(0);
    const auto acc = getDevice<Acc>(m_deviceIndex);
    auto q = Queue(acc);

    const auto maxOutputEventsCount = static_cast<Idx>(batchSize * (maxEvents - startEventID));
    const auto initialCompactEventsSize = static_cast<Idx>(batchSize * glm::min(2, maxEvents));

    resizeBufferIfNeeded(q, m_batchOutput.compactEvents, initialCompactEventsSize);
    resizeBufferIfNeeded(q, m_batchOutput.compactEventCounts, batchSize);
    resizeBufferIfNeeded(q, m_batchOutput.compactEventOffsets, batchSize);
    resizeBufferIfNeeded(q, m_batchOutput.events, maxOutputEventsCount);

    if (beamlineInput != m_prevBeamlineInput) {
        transferToBuffer(q, cpu, m_beamlineInput.elements, beamlineInput->elements, static_cast<Idx>(beamlineInput->elements.size()));
        transferToBuffer(q, cpu, m_beamlineInput.materialIndices, beamlineInput->materialTables.indexTable,
                         static_cast<Idx>(beamlineInput->materialTables.indexTable.size()));
        transferToBuffer(q, cpu, m_beamlineInput.materialData, beamlineInput->materialTables.materialTable,
                         static_cast<Idx>(beamlineInput->materialTables.materialTable.size()));

        m_prevBeamlineInput = beamlineInput;
    }

    const auto rayIdStart = batchInput.raysOffset;
    const auto remaining_rays = beamlineInput->rays.size() - batchInput.raysOffset;

    const auto sequential = (double)(seq == Sequential::Yes);
    m_pushConstants = {.rayIdStart = (double)batchInput.raysOffset,
                       .numRays = (double)beamlineInput->rays.size(),
                       .randomSeed = beamlineInput->randomSeed,
                       .maxEvents = (double)maxEvents,
                       .sequential = sequential,
                       .startEventID = (double)startEventID};

    const auto inputRays = beamlineInput->rays.data() + rayIdStart;
    transferToBuffer(q, cpu, m_batchInput.rays, inputRays, static_cast<Idx>(batchSize));

    // run the actual tracer (GPU/CPU).
    const auto traceResult = traceBatchOnDevice(q, batchSize);
    RAYX_LOG << alpaka::getName(acc) << ": Traced " << traceResult.totalEventsCount << " events.";

    transferFromBuffer(q, cpu, m_batchResult.compactEventCounts, m_batchOutput.compactEventCounts, static_cast<Idx>(batchSize));
    transferFromBuffer(q, cpu, m_batchResult.compactEventOffsets, m_batchOutput.compactEventOffsets, static_cast<Idx>(batchSize));
    transferFromBuffer(q, cpu, m_batchResult.compactEvents, m_batchOutput.compactEvents, static_cast<Idx>(traceResult.totalEventsCount));

    alpaka::wait(q);

    return DeviceTracer::BatchOutput{
        .eventCounts = std::move(m_batchResult.compactEventCounts),
        .eventOffsets = std::move(m_batchResult.compactEventOffsets),
        .events = std::move(m_batchResult.compactEvents),
    };
}

template <typename Acc>
SimpleTracer<Acc>::TraceResult SimpleTracer<Acc>::traceBatchOnDevice(Queue q, const Idx batchSize) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // reference resources

    auto inv = InvState{
        // shader instance local variables
        .globalInvocationId = {},
        .finalized = {},
        .ctr = {},
        .nextEventIndex = {},

        // buffers
        .inputRays = bufferToSpan(m_batchInput.rays),
        .outputRays = bufferToSpan(m_batchOutput.events),
        .outputRayCounts = bufferToSpan(m_batchOutput.compactEventCounts),
        .elements = bufferToSpan(m_beamlineInput.elements),
        .matIdx = bufferToSpan(m_beamlineInput.materialIndices),
        .mat = bufferToSpan(m_beamlineInput.materialData),

#ifdef RAYX_DEBUG_MODE
        .d_struct = {},
#endif

        .pushConstants = m_pushConstants,
    };

    // execute dynamic elements shader

    alpaka::exec<Acc>(q, getWorkDivForAcc<Acc>(batchSize), DynamicElementsKernel{}, inv);

    // make output events compact

    auto totalEventsCount = scanSum<Acc, Idx>(q, *m_batchOutput.compactEventOffsets.buf, *m_batchOutput.compactEventCounts.buf, batchSize);

    resizeBufferIfNeeded(q, m_batchOutput.compactEvents, totalEventsCount);

    gather<Acc, Ray>(q, *m_batchOutput.compactEvents.buf, *m_batchOutput.events.buf, *m_batchOutput.compactEventOffsets.buf,
                     *m_batchOutput.compactEventCounts.buf, static_cast<Idx>(inv.pushConstants.maxEvents), batchSize);

    return TraceResult{
        .totalEventsCount = totalEventsCount,
    };
}

template <typename Acc>
template <typename T>
void SimpleTracer<Acc>::resizeBufferIfNeeded(Queue q, Buffer<T>& buffer, const Idx size) {
    const auto shouldAlloc = !buffer.buf || alpaka::getExtentProduct(*buffer.buf) < size;
    if (shouldAlloc) {
        const auto nextPowerOfTwo = glm::pow(2, glm::ceil(glm::log(size) / glm::log(2)));
        buffer.buf = alpaka::allocAsyncBufIfSupported<T, Idx>(q, Vec{nextPowerOfTwo});
    }
    buffer.size = size;
}

template <typename Acc>
template <typename T>
void SimpleTracer<Acc>::transferToBuffer(Queue q, alpaka::DevCpu cpu, Buffer<T>& dst, const T* src, const Idx size) {
    resizeBufferIfNeeded(q, dst, size);
    auto srcView = alpaka::createView(cpu, src, Vec{size});
    alpaka::memcpy(q, *dst.buf, srcView, Vec{size});
}

template <typename Acc>
template <typename T>
void SimpleTracer<Acc>::transferToBuffer(Queue q, alpaka::DevCpu cpu, Buffer<T>& dst, const std::vector<T>& src, const Idx size) {
    transferToBuffer(q, cpu, dst, src.data(), size);
}

template <typename Acc>
template <typename T>
void SimpleTracer<Acc>::transferFromBuffer(Queue q, alpaka::DevCpu cpu, T* dst, Buffer<T>& src, const Idx size) {
    auto dstView = alpaka::createView(cpu, dst, Vec{size});
    alpaka::memcpy(q, dstView, *src.buf, Vec{size});
}

template <typename Acc>
template <typename T>
void SimpleTracer<Acc>::transferFromBuffer(Queue q, alpaka::DevCpu cpu, std::vector<T>& dst, Buffer<T>& src, const Idx size) {
    dst.resize(size);
    auto dstView = alpaka::createView(cpu, dst, Vec{size});
    alpaka::memcpy(q, dstView, *src.buf, Vec{size});
}

template <typename Acc>
template <typename T>
std::span<T> SimpleTracer<Acc>::bufferToSpan(Buffer<T>& buffer) {
    return bufToSpan(*buffer.buf, buffer.size);
}

}  // namespace RAYX
