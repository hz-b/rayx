#pragma once

#include <cmath>
#include <cstring>

#include "DeviceTracer.h"
#include "Gather.h"
#include "Scan.h"
#include "Util.h"

#include "RAY-Core.h"
#include "Random.h"
#include "Beamline/Beamline.h"
#include "Material/Material.h"
#include "Shader/DynamicElements.h"

namespace {

struct DynamicElementsKernel {
    template <typename Acc>
    RAYX_FUNC
    void operator() (const Acc& acc, Inv inv) const {
        using Idx = alpaka::Idx<Acc>;
        const Idx gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < static_cast<Idx>(inv.rayData.size()))
            dynamicElements(gid, inv);
    }
};

} // unnamed namespace

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

    BundleHistory trace(
        const Beamline&,
        Sequential sequential,
        uint64_t max_batch_size,
        int THREAD_COUNT,
        unsigned int maxEvents,
        int startEventID
    ) override;

  private:
    struct TraceResult {
        int totalEventsCount;
    };

    TraceResult traceBatch(Queue queue, const Idx numInputRays);

    template <typename T>
    struct Buffer {
        using Buf = std::optional<alpaka::Buf<Acc, T, Dim, Idx>>;
        using Staging = std::optional<alpaka::Buf<Cpu, T, Dim, Idx>>;

        Buf buf;
        Staging staging;
    };

    struct BeamlineInput {
        Buffer<Element> elements;
        Buffer<int>     materialIndices;
        Buffer<double>  materialData;
    } m_beamlineInput;

    struct BatchInput {
        Buffer<Ray> rays;
    } m_batchInput;

    struct BatchOutput {
        Buffer<Idx> compactEventCounts;
        Buffer<Idx> compactEventOffsets;
        Buffer<Ray> compactEvents;
        typename Buffer<Ray>::Buf events;
    } m_batchOutput;

    const int m_deviceIndex;
};

template <typename Acc>
SimpleTracer<Acc>::SimpleTracer(int deviceIndex) :
    m_deviceIndex(deviceIndex)
{
}

template <typename Acc>
BundleHistory SimpleTracer<Acc>::trace(const Beamline& b, Sequential seq, uint64_t max_batch_size, int thread_count, unsigned int maxEvents, int startEventID) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "maxEvents: " << maxEvents;

    auto rays = b.getInputRays(thread_count);

    // don't trace if there are no optical elements
    if (b.m_OpticalElements.size() == 0) {
        // an empty history suffices, nothing is happening to the rays!
        BundleHistory result;
        return result;
    }

    auto randomSeed = randomDouble();
    auto materialTables = b.calcMinimalMaterialTables();

    std::vector<Element> elements;
    elements.reserve(b.m_OpticalElements.size());
    for (const auto& e : b.m_OpticalElements) {
        elements.push_back(e.m_element);
    }

    const auto cpu = getDevice<Cpu>();
    const auto acc = getDevice<Acc>(m_deviceIndex);
    const auto acc_platform = alpaka::Platform<Acc>();
    auto queue = Queue(acc);

    auto alloc = [cpu, acc_platform] <typename T> (Queue queue, Buffer<T>& buffer, Idx size) {
        const auto shouldAlloc = !buffer.buf || alpaka::getExtentProduct(*buffer.buf) < size;
        if (shouldAlloc) {
            buffer.buf = alpaka::allocAsyncBufIfSupported<T, Idx>(queue, Vec{size});
            buffer.staging = alpaka::allocMappedBufIfSupported<T, Idx>(cpu, acc_platform, Vec{size});
        }
    };

    auto allocBuf = [] <typename T> (Queue queue, typename Buffer<T>::Buf& buf, Idx size) {
        const auto shouldAlloc = !buf || alpaka::getExtentProduct(*buf) < size;
        if (shouldAlloc)
            buf = alpaka::allocAsyncBufIfSupported<T, Idx>(queue, Vec{size});
    };

    auto copyVec = [] <typename T> (Queue queue, Buffer<T>& dst, const std::vector<T>& src) {
        std::memcpy(alpaka::getPtrNative(*dst.staging), src.data(), src.size() * sizeof(T));
        alpaka::memcpy(queue, *dst.buf, *dst.staging, Vec{src.size()});
    };

    auto copyRaw = [] <typename T> (Queue queue, Buffer<T>& dst, const T* src, const Idx size) {
        std::memcpy(alpaka::getPtrNative(*dst.staging), src, size * sizeof(T));
        alpaka::memcpy(queue, *dst.buf, *dst.staging, Vec{size});
    };

    auto copyBack = [cpu] <typename T> (Queue queue, Buffer<T>& buffer, Idx size) {
        alpaka::memcpy(queue, *buffer.staging, *buffer.buf, Vec{size});
        return std::span<T>(alpaka::getPtrNative(*buffer.staging), size);
    };

    alloc(queue, m_beamlineInput.elements, elements.size());
    alloc(queue, m_beamlineInput.materialIndices, materialTables.indexTable.size());
    alloc(queue, m_beamlineInput.materialData, materialTables.materialTable.size());
    copyVec(queue, m_beamlineInput.elements, elements);
    copyVec(queue, m_beamlineInput.materialIndices, materialTables.indexTable);
    copyVec(queue, m_beamlineInput.materialData, materialTables.materialTable);

    // This will be the complete BundleHistory.
    // All initialized events will have been put into this by the end of this function.
    BundleHistory result;

    // iterate over all batches.
    for (int batch_id = 0; batch_id * max_batch_size < rays.size(); batch_id++) {
        // `rayIdStart` is the ray-id of the first ray of this batch.
        // All previous batches consisted of `max_batch_size`-many rays.
        // (Only the last batch might be smaller than max_batch_size, if the number of rays isn't divisible by max_batch_size).
        auto rayIdStart = batch_id * max_batch_size;

        auto remaining_rays = rays.size() - batch_id * max_batch_size;

        // The number of input-rays that we put into this batch.
        // Typically equal to max_batch_size, except for the last batch.
        const auto batch_size = (max_batch_size < remaining_rays) ? max_batch_size : remaining_rays;
        const auto numInputRays = batch_size;
        const auto numOutputRays = numInputRays * (maxEvents - startEventID);

        alloc(queue, m_batchInput.rays, numInputRays);
        alloc(queue, m_batchOutput.compactEventCounts, numInputRays);
        alloc(queue, m_batchOutput.compactEventOffsets, numInputRays);
        // alloc(queue, m_batchOutput.compactOutputEvents, numInputRays);
        allocBuf.template operator()<Ray>(queue, m_batchOutput.events, numOutputRays);

        auto inputRays = rays.data() + rayIdStart;
        copyRaw.template operator()<Ray>(queue, m_batchInput.rays, inputRays, batch_size);

        auto sequential = (double)(seq == Sequential::Yes);
        m_pushConstants = {
            .rayIdStart = (double)rayIdStart,
            .numRays = (double)rays.size(),
            .randomSeed = randomSeed,
            .maxEvents = (double)maxEvents,
            .sequential = sequential,
            .startEventID = (double)startEventID
        };

        // run the actual tracer (GPU/CPU).
        auto traceResult = traceBatch(queue, numInputRays);
        RAYX_LOG << "Traced " << traceResult.totalEventsCount << " events.";

        auto compactEventCounts = copyBack(queue, m_batchOutput.compactEventCounts, numInputRays);
        auto compactEventOffsets = copyBack(queue, m_batchOutput.compactEventOffsets, numInputRays);
        auto compactEvents = copyBack(queue, m_batchOutput.compactEvents, traceResult.totalEventsCount);

        alpaka::wait(queue);

        // put all events from the rawBatch to unified `BundleHistory result`.
        {
            RAYX_PROFILE_SCOPE_STDOUT("BundleHistory-calculation");
            for (uint i = 0; i < batch_size; i++) {
                // We now create the Rayhistory for the `i`th ray of the batch:
                auto begin = compactEvents.data() + compactEventOffsets[i];
                auto end = begin + compactEventCounts[i];
                auto hist = RayHistory(begin, end);

                // We put the `hist` for the `i`th ray of the batch into the global `BundleHistory result`.
                result.push_back(hist);
            }
        }
    }

    return result;
}

template <typename Acc>
SimpleTracer<Acc>::TraceResult SimpleTracer<Acc>::traceBatch(Queue queue, const Idx numInputRays) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // reference resources

    auto inv = Inv {
        // shader instance local variables
        .globalInvocationId = {},
        .finalized          = {},
        .ctr                = {},
        .nextEventIndex     = {},

        // buffers
        .rayData            = bufToSpan(*m_batchInput.rays.buf),
        .outputData         = bufToSpan(*m_batchOutput.events),
        .outputRayCounts    = bufToSpan(*m_batchOutput.compactEventCounts.buf),
        .elements           = bufToSpan(*m_beamlineInput.elements.buf),
        .xyznull            = {},
        .matIdx             = bufToSpan(*m_beamlineInput.materialIndices.buf),
        .mat                = bufToSpan(*m_beamlineInput.materialData.buf),

#ifdef RAYX_DEBUG_MODE
        .d_struct           = {},
#endif

        .pushConstants      = m_pushConstants,
    };

    // execute dynamic elements shader

    alpaka::exec<Acc>(
        queue,
        getWorkDivForAcc<Acc>(numInputRays),
        DynamicElementsKernel{},
        inv
    );

    // make output events compact

    auto totalEventsCount = scan_sum<Acc, Idx>(
        queue,
        *m_batchOutput.compactEventOffsets.buf,
        *m_batchOutput.compactEventCounts.buf,
        numInputRays
    );

    const auto cpu = getDevice<Cpu>();
    const auto acc_platform = alpaka::Platform<Acc>();
    auto alloc = [cpu, acc_platform] <typename T> (Queue queue, Buffer<T>& buffer, Idx size) {
        const auto shouldAlloc = !buffer.buf || alpaka::getExtentProduct(*buffer.buf) < size;
        if (shouldAlloc) {
            buffer.buf = alpaka::allocAsyncBufIfSupported<T, Idx>(queue, Vec{size});
            buffer.staging = alpaka::allocMappedBufIfSupported<T, Idx>(cpu, acc_platform, Vec{size});
        }
    };

    alloc(queue, m_batchOutput.compactEvents, totalEventsCount);

    gather_n<Acc, Ray>(
        queue,
        *m_batchOutput.compactEvents.buf,
        *m_batchOutput.events,
        *m_batchOutput.compactEventOffsets.buf,
        *m_batchOutput.compactEventCounts.buf,
        static_cast<Idx>(inv.pushConstants.maxEvents),
        numInputRays
    );

    return TraceResult {
        .totalEventsCount = totalEventsCount,
    };
}

}  // namespace RAYX
