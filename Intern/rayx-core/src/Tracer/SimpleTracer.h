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

namespace RAYX {

namespace {
struct DynamicElementsKernel {
    template <typename Acc>
    RAYX_FN_ACC
    void operator()(const Acc& acc, const InvState inv, OutputEvents outputEvents) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < inv.numRaysBatch) dynamicElements(gid, inv, outputEvents);
    }
};
}  // unnamed namespace

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

    BundleHistory trace(const Group&, Sequential sequential, uint64_t maxBatchSize, int getInputRaysThreadCount, uint32_t maxEvents) override;

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
        Buffer<OpticalElement> elements;
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
        // events is a buffer with capacity to hold a number of rays: maxEvents * numRaysTotal
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

    TraceResult traceBatch(Queue q, int numRaysTotal, int numRaysBatch, int batchStartRayIndex, double randomSeed, int maxEvents, Sequential sequential);
};

template <typename Acc>
SimpleTracer<Acc>::SimpleTracer(int deviceIndex) : m_deviceIndex(deviceIndex) {}

template <typename Acc>
BundleHistory SimpleTracer<Acc>::trace(const Group& group, Sequential sequential, uint64_t maxBatchSize, int getInputRaysThreadCount, uint32_t maxEvents) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    const auto elements = group.compileElements();

    // don't trace if there are no optical elements
    // an empty history suffices, nothing is happening to the rays!
    if (elements.size() == 0) {
        return {};
    }

    const auto rays = group.compileSources(getInputRaysThreadCount);
    const auto materialTables = group.calcMinimalMaterialTables();
    const auto randomSeed = randomDouble();

    const auto cpu = getDevice<Cpu>(0);
    const auto acc = getDevice<Acc>(m_deviceIndex);
    auto q = Queue(acc);

    // in sequential mode, the max number of events is equal to the number of elements
    if (sequential == Sequential::Yes)
        maxEvents = elements.size();

    const auto firstBatchSize = static_cast<Idx>(glm::min(rays.size(), maxBatchSize));
    const auto maxOutputEventsCount = static_cast<Idx>(maxBatchSize * maxEvents);
    const auto initialCompactEventsSize = static_cast<Idx>(maxBatchSize * glm::min(2u, maxEvents));
    resizeBufferIfNeeded(q, m_batchOutput.compactEvents, initialCompactEventsSize);
    resizeBufferIfNeeded(q, m_batchOutput.compactEventCounts, firstBatchSize);
    resizeBufferIfNeeded(q, m_batchOutput.compactEventOffsets, firstBatchSize);
    resizeBufferIfNeeded(q, m_batchOutput.events, maxOutputEventsCount);
    transferToBuffer(q, cpu, m_beamlineInput.elements, elements, static_cast<Idx>(elements.size()));
    transferToBuffer(q, cpu, m_beamlineInput.materialIndices, materialTables.indices, static_cast<Idx>(materialTables.indices.size()));
    transferToBuffer(q, cpu, m_beamlineInput.materialData, materialTables.materials, static_cast<Idx>(materialTables.materials.size()));

    RAYX_VERB << "Tracing Beamline" << ", num elements: " << elements.size() << ", num rays: " << rays.size() << ", sequential: " << static_cast<int>(sequential) << ", max events: " << maxEvents << ", first batch size " << firstBatchSize << ", max batch size: " << maxBatchSize;

    // This will be the complete BundleHistory.
    // All initialized events will have been put into this by the end of this function.
    BundleHistory result;

    // iterate over all batches.
    for (int batch_id = 0; batch_id * maxBatchSize < rays.size(); batch_id++) {
        // `batchStartRayIndex` is the ray-id of the first ray of this batch.
        // All previous batches consisted of `maxBatchSize`-many rays.
        // (Only the last batch might be smaller than maxBatchSize, if the number of rays isn't divisible by maxBatchSize).
        const auto batchStartRayIndex = batch_id * maxBatchSize;
        const auto remaining_rays = rays.size() - batch_id * maxBatchSize;
        // The number of input-rays that we put into this batch.
        // Typically equal to maxBatchSize, except for the last batch.
        const auto batchSize = (maxBatchSize < remaining_rays) ? maxBatchSize : remaining_rays;
        const auto numRaysTotal = batchSize;

        const auto inputRays = rays.data() + batchStartRayIndex;
        transferToBuffer(q, cpu, m_batchInput.rays, inputRays, static_cast<Idx>(batchSize));

        // run the actual tracer (GPU/CPU).
        const auto traceResult = traceBatch(q, numRaysTotal, rays.size(), batchStartRayIndex, randomSeed, maxEvents, sequential);
        RAYX_LOG << "Traced " << traceResult.totalEventsCount << " events.";

        transferFromBuffer(q, cpu, m_batchResult.compactEventCounts, m_batchOutput.compactEventCounts, static_cast<Idx>(numRaysTotal));
        transferFromBuffer(q, cpu, m_batchResult.compactEventOffsets, m_batchOutput.compactEventOffsets, static_cast<Idx>(numRaysTotal));
        transferFromBuffer(q, cpu, m_batchResult.compactEvents, m_batchOutput.compactEvents, static_cast<Idx>(traceResult.totalEventsCount));

        alpaka::wait(q);

        // put all events from the rawBatch to unified `BundleHistory result`.
        {
            RAYX_PROFILE_SCOPE_STDOUT("BundleHistory-calculation");
            for (uint32_t i = 0; i < batchSize; i++) {
                // We now create the Rayhistory for the `i`th ray of the batch:
                auto begin = m_batchResult.compactEvents.data() + m_batchResult.compactEventOffsets[i];
                auto end = begin + m_batchResult.compactEventCounts[i];
                auto hist = RayHistory(begin, end);

                // We put the `hist` for the `i`th ray of the batch into the global `BundleHistory result`.
                result.push_back(hist);
            }
        }
    }

    return result;
}

template <typename Acc>
SimpleTracer<Acc>::TraceResult SimpleTracer<Acc>::traceBatch(Queue q, int numRaysTotal, int numRaysBatch, int batchStartRayIndex, double randomSeed, int maxEvents, Sequential sequential) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // reference resources
    auto inv = InvState{
        // constants
        .numRaysTotal = numRaysTotal,
        .numRaysBatch = numRaysBatch,
        .batchStartRayIndex = batchStartRayIndex,
        .maxEvents = maxEvents,
        .randomSeed = randomSeed,
        .sequential = sequential,

        // buffers
        .elements = alpaka::getPtrNative(*m_beamlineInput.elements.buf),
        .numElements = m_beamlineInput.elements.size,
        .materialIndices = alpaka::getPtrNative(*m_beamlineInput.materialIndices.buf),
        .materialTables = alpaka::getPtrNative(*m_beamlineInput.materialData.buf),
        .inputRays = alpaka::getPtrNative(*m_batchInput.rays.buf),
    };

    auto outputEvents = OutputEvents{
        // buffers
        .events = alpaka::getPtrNative(*m_batchOutput.events.buf),
        .numEvents = alpaka::getPtrNative(*m_batchOutput.compactEventCounts.buf),
    };

    // execute dynamic elements shader

    alpaka::exec<Acc>(q, getWorkDivForAcc<Acc>(numRaysTotal), DynamicElementsKernel{}, inv, outputEvents);

    // make output events compact

    auto totalEventsCount = scanSum<Acc, Idx>(q, *m_batchOutput.compactEventOffsets.buf, *m_batchOutput.compactEventCounts.buf, numRaysTotal);

    resizeBufferIfNeeded(q, m_batchOutput.compactEvents, totalEventsCount);

    gather<Acc, Ray>(q, *m_batchOutput.compactEvents.buf, *m_batchOutput.events.buf, *m_batchOutput.compactEventOffsets.buf,
                     *m_batchOutput.compactEventCounts.buf, maxEvents, numRaysTotal);

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

}  // namespace RAYX
