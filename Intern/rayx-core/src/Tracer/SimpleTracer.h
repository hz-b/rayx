#pragma once

#include <cmath>
#include <cstring>

#include "DeviceTracer.h"
#include "RAY-Core.h"
#include "Random.h"
#include "Common.h"
#include "Accelerator.h"
#include "Util.h"
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

struct PredicateKernel {
    template <typename Acc>
    RAYX_FUNC
    void operator() (
        const Acc& acc,
        alpaka::Idx<Acc>* predicates,
        const alpaka::Idx<Acc>* src,
        const alpaka::Idx<Acc> numPredicatesPerElement
    ) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        for (int i = 0; i < numPredicatesPerElement; ++i) {
            int idx = gid * numPredicatesPerElement + i;
            auto pass = i < src[gid];
            predicates[idx] = pass ? 1 : 0;
        }
    }
};

struct ScatterKernel {
    template <typename Acc, typename T>
    RAYX_FUNC
    void operator() (
        const Acc& acc,
        T* dst,
        const T* src,
        const alpaka::Idx<Acc>* predicates,
        const alpaka::Idx<Acc>* indices
    ) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (predicates[gid])
            dst[indices[gid]] = src[gid];
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
    using AccDev = alpaka::Dev<Acc>;
    using Cpu = alpaka::DevCpu;

    using CpuPlatform = alpaka::Platform<Cpu>;
    using AccPlatform = alpaka::Platform<Acc>;

    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    static_assert(std::is_same_v<Idx, int>); // TODO(Sven): ensure TraceResult has the same Idx type as SimpleTracer
    using Vec = alpaka::Vec<Dim, Idx>;

    using Seq = alpaka::AccCpuSerial<Dim, Idx>;

    // template <typename T>
    // using AccBuf = alpaka::Buf<Acc, T, Dim, Idx>;
    // template <typename T>
    // using CpuBuf = alpaka::Buf<Cpu, T, Dim, Idx>;

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
    TraceResult traceBatch(const TraceRawConfig& cfg);

    template <typename T>
    Idx scan_sum(Queue queue, Buf<Acc, T> dst, Buf<Acc, T> src, const Idx n);

    template <typename T>
    void scatter(Queue queue, Buf<Acc, T> dst, Buf<Acc, T> src, Buf<Acc, Idx> predicates, Buf<Acc, Idx> indices, const Idx n);

    void createFilterPredicates(Queue queue, Buf<Acc, Idx> predicates, Buf<Acc, Idx> src, const Idx numPredicatesPerElement, const Idx n);

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
        auto batch_size = (max_batch_size < remaining_rays) ? max_batch_size : remaining_rays;

        std::vector<Element> elements;
        elements.reserve(b.m_OpticalElements.size());
        for (const auto& e : b.m_OpticalElements) {
            elements.push_back(e.m_element);
        }

        // create a TraceRawConfig.
        TraceRawConfig cfg = {
            .m_rays = std::vector<Ray>(rays.begin() + rayIdStart, rays.begin() + rayIdStart + batch_size),
            .m_rayIdStart = (double)rayIdStart,
            .m_numRays = (double)rays.size(),
            .m_randomSeed = randomSeed,
            .m_maxEvents = (double)maxEvents,
            .m_startEventID = (double)startEventID,
            .m_materialTables = materialTables,
            .m_elements = elements,
        };

        auto sequential = (double)(seq == Sequential::Yes);
        PushConstants pushConstants = {.rayIdStart = (double)rayIdStart,
                                       .numRays = (double)rays.size(),
                                       .randomSeed = randomSeed,
                                       .maxEvents = (double)maxEvents,
                                       .sequential = sequential,
                                       .startEventID = (double)startEventID};
        setPushConstants(&pushConstants);

        // run the actual tracer (GPU/CPU).
        TraceResult traceResult;
        {
            RAYX_PROFILE_SCOPE_STDOUT("Tracing");
            traceResult = traceBatch(cfg);
            RAYX_LOG << "Traced " << traceResult.count << " events.";
        }

        // put all events from the rawBatch to unified `BundleHistory result`.
        {
            RAYX_PROFILE_SCOPE_STDOUT("BundleHistory-calculation");
            for (uint i = 0; i < batch_size; i++) {
                // We now create the Rayhistory for the `i`th ray of the batch:
                RayHistory hist;

                auto begin = traceResult.events.data() + traceResult.offsets[i];
                auto end = begin + traceResult.sizes[i];
                hist.insert(hist.end(), begin, end);

                // We put the `hist` for the `i`th ray of the batch into the global `BundleHistory result`.
                result.push_back(hist);
            }
        }
    }

    return result;
}

template <typename Acc>
void SimpleTracer<Acc>::createFilterPredicates(Queue queue, Buf<Acc, Idx> predicates, Buf<Acc, Idx> src, const Idx numPredicatesPerElement, const Idx n) {
    alpaka::exec<Acc>(
        queue,
        getWorkDivForAcc<Acc>(n),
        PredicateKernel{},
        alpaka::getPtrNative(predicates),
        alpaka::getPtrNative(src),
        numPredicatesPerElement
    );
}

// TODO(Sven): implement parallel scan
template <typename Acc>
template <typename T>
alpaka::Idx<Acc> SimpleTracer<Acc>::scan_sum(Queue queue, Buf<Acc, T> dst, Buf<Acc, T> src, const Idx n) {
    RAYX_PROFILE_SCOPE_STDOUT("scan");

    auto seq = getDevice<Seq>();

    auto h_src = std::vector<T>(n);
    auto h_dst = std::vector<T>(n);

    auto h_srcView = alpaka::createView(seq, h_src);
    auto h_dstView = alpaka::createView(seq, h_dst);

    alpaka::memcpy(queue, h_srcView, src, Vec{n});

    Idx sum;
    alpaka::exec<Seq>(
        queue,
        alpaka::WorkDivMembers<Dim, Idx> { Vec{1}, Vec{1}, Vec{1}, },
        [] ALPAKA_FN_HOST (const auto&, Idx* sum, T* dst, const T* src, const Idx n) {
            *sum = 0;
            for (Idx i = 0; i < n; ++i) {
                dst[i] = *sum;
                *sum += src[i];
            }
        },
        &sum,
        alpaka::getPtrNative(h_dst),
        alpaka::getPtrNative(h_src),
        n
    );

    // wait because the above kernel is not guaranteed to finish before the next task
    alpaka::wait(queue);

    alpaka::memcpy(queue, dst, h_dst, Vec{n});

    // wait for all operations to finish, before destructing buffers
    alpaka::wait(queue);
    return sum;
}

template <typename Acc>
template <typename T>
void SimpleTracer<Acc>::scatter(Queue queue, Buf<Acc, T> dst, Buf<Acc, T> src, Buf<Acc, Idx> predicates, Buf<Acc, Idx> indices, const Idx n) {
    alpaka::exec<Acc>(
        queue,
        getWorkDivForAcc<Acc>(n),
        ScatterKernel{},
        alpaka::getPtrNative(dst),
        alpaka::getPtrNative(src),
        alpaka::getPtrNative(predicates),
        alpaka::getPtrNative(indices)
    );
}

template <typename Acc>
TraceResult SimpleTracer<Acc>::traceBatch(const TraceRawConfig& cfg) {
    auto host = getDevice<Cpu>();
    auto acc = getDevice<Acc>(m_deviceIndex);

    auto queue = Queue(acc);

    const auto numInputRays = cfg.m_rays.size();
    const auto numOutputRays = numInputRays * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID);
    const auto& materialTables = cfg.m_materialTables;

    auto rayData = createBuffer<Idx, Vec, Ray>(queue, cfg.m_rays, host);
    auto outputRays = createBuffer<Idx, Ray>(queue, Vec{numOutputRays});
    auto outputRayCounts = createBuffer<Idx, Idx>(queue, Vec{numInputRays});
    auto elements = createBuffer<Idx, Vec, Element>(queue, cfg.m_elements, host);
    auto matIdx = createBuffer<Idx, Vec, int>(queue, materialTables.indexTable, host);
    auto mat = createBuffer<Idx, Vec, double>(queue, materialTables.materialTable, host);

    auto inv = Inv {
        // shader instance local variables
        .globalInvocationId = {},
        .finalized = {},
        .ctr = {},
        .nextEventIndex = {},

        // buffers
        .rayData = bufToSpan(rayData),
        .outputData = bufToSpan(outputRays),
        .outputRayCounts = bufToSpan(outputRayCounts),
        .elements = bufToSpan(elements),
        .xyznull = {},
        .matIdx = bufToSpan(matIdx),
        .mat = bufToSpan(mat),

#ifdef RAYX_DEBUG_MODE
        .d_struct = {},
#endif

        // CFG meta passed through pushConstants
        .pushConstants = m_pushConstants,
    };

    // execute dynamic elements shader

    alpaka::exec<Acc>(
        queue,
        getWorkDivForAcc<Acc>(numInputRays),
        DynamicElementsKernel{},
        inv
    );

    // make output events compact

    auto globalEventFilterPredicates = createBuffer<Idx, Idx>(queue, Vec{numOutputRays});
    createFilterPredicates(queue, globalEventFilterPredicates, outputRayCounts, cfg.m_maxEvents, numInputRays);

    auto globalEventIndices = createBuffer<Idx, Idx>(queue, Vec{numOutputRays});
    auto globalEventsCount = scan_sum<Idx>(queue, globalEventIndices, globalEventFilterPredicates, numOutputRays);

    auto compactGlobalEvents = createBuffer<Idx, Ray>(queue, Vec{globalEventsCount});
    scatter<Ray>(queue, compactGlobalEvents, outputRays, globalEventFilterPredicates, globalEventIndices, numOutputRays);

    auto compactRayOffsets = createBuffer<Idx, Idx>(queue, Vec{numInputRays});
    auto globalEventsCount2 = scan_sum<Idx>(queue, compactRayOffsets, outputRayCounts, numInputRays);
    assert(globalEventsCount == globalEventsCount2);

    // transfer
    // TODO(Sven): make transfer a noop for cpu accelerators

    auto h_compactRayOffsets = std::vector<Idx>(numInputRays);
    auto h_compactRayOffsetsView = alpaka::createView(host, h_compactRayOffsets);
    alpaka::memcpy(queue, h_compactRayOffsets, compactRayOffsets, Vec{numInputRays});

    auto h_compactGlobalEvents = std::vector<Ray>(globalEventsCount);
    auto h_compactGlobalEventsView = alpaka::createView(host, h_compactGlobalEvents);
    alpaka::memcpy(queue, h_compactGlobalEventsView, compactGlobalEvents, Vec{globalEventsCount});

    auto h_outputRayCounts = std::vector<Idx>(numInputRays);
    auto h_outputRayCountsView = alpaka::createView(host, h_outputRayCounts);
    alpaka::memcpy(queue, h_outputRayCountsView, outputRayCounts, Vec{numInputRays});

    alpaka::wait(queue);

    return TraceResult {
        std::move(h_compactGlobalEvents),
        std::move(h_compactRayOffsets),
        std::move(h_outputRayCounts),
        globalEventsCount,
    };
}

}  // namespace RAYX
