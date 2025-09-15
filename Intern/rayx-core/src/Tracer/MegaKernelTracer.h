#pragma once

#include <numeric>
#include <set>

#include "Beamline/Beamline.h"
#include "Debug/Instrumentor.h"
#include "DeviceTracer.h"
#include "GenRays.h"
#include "Material/Material.h"
#include "Random.h"
#include "Shader/Trace.h"
#include "Util.h"

namespace RAYX {
namespace {

struct TraceSequentialKernel {
    template <typename Acc>
    RAYX_FN_ACC void operator()(const Acc& __restrict acc, const ConstState constState, MutableState mutableState, const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) traceSequential(gid, constState, mutableState);
    }
};

struct TraceNonSequentialKernel {
    template <typename Acc>
    RAYX_FN_ACC void operator()(const Acc& __restrict acc, const ConstState constState, MutableState mutableState, const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) traceNonSequential(gid, constState, mutableState);
    }
};

struct ScatterCompactKernel {
    template <typename Acc, typename T>
    RAYX_FN_ACC void operator()(const Acc& __restrict acc, T* __restrict dst, const T* __restrict src, const int* __restrict prefix,
                                const bool* __restrict flags, const int n) {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n && flags[gid]) {
            const auto index = prefix[gid];
            dst[index]       = src[gid];
        }
    }
};

}  // unnamed namespace

/// keeps track of all resources used by the tracer. manages allocation and update of buffers
/// note: members starting with h_ reside on host side, while d_ reside on device side
template <typename Acc>
struct Resources {
    using Dim = alpaka::DimInt<1>;
    using Idx = int;

    // resources per program execution. constant per program execution
    /// material data
    OptBuf<Acc, int> d_materialIndices;
    OptBuf<Acc, double> d_materialTable;

    // resources per beamline. constant per beamline
    /// beamline elements
    OptBuf<Acc, OpticalElement> d_elements;

    /// mask for which elements to record events
    OptBuf<Acc, bool> d_elementRecordMask;

    // output events per tracing. required if 'events' is enabled in output config
    /// output events from tracer kernel
    RaysBuf<Acc> d_eventsBatch;
    /// output events, compacted for faster transfer
    RaysBuf<Acc> d_compactEventsBatch;
    /// flag for each possible ouput event, wether it was stored or not. used for compaction
    OptBuf<Acc, bool> d_eventStoreFlags;

    /// ray attributes layed out as structure of arrays (SoA)
    outputEvents;

    /// holds configuration state of allocated resources. required to trace correctly
    struct BeamlineConfig {
        int numSources;
        int numElements;
    };

    /// update resources
    template <typename Queue>
    BeamlineConfig update(Queue q, const Group& group, int maxEvents, int numRaysBatchAtMost, const ObjectMask& objectRecordMask) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto platformHost = alpaka::PlatformCpu{};
        const auto devHost      = alpaka::getDevByIdx(platformHost, 0);

        // material data
        const auto materialTables     = group.calcMinimalMaterialTables();
        const auto& materialIndices   = materialTables.indices;
        const auto& materialTable     = materialTables.materials;
        const auto numMaterialIndices = static_cast<int>(materialIndices.size());
        const auto materialTableSize  = static_cast<int>(materialTable.size());
        allocBuf(q, d_materialIndices, materialIndices.size());
        allocBuf(q, d_materialTable, materialTable.size());
        alpaka::memcpy(q, *d_materialIndices, alpaka::createView(devHost, materialIndices, numMaterialIndices));
        alpaka::memcpy(q, *d_materialTable, alpaka::createView(devHost, materialTable, materialTableSize));

        // beamline elements
        const auto elements    = group.compileElements();
        const auto numElements = static_cast<int>(elements.size());
        allocBuf(q, d_elements, numElements);
        alpaka::memcpy(q, *d_elements, alpaka::createView(devHost, elements, numElements));

        // record mask
        const auto numSources = static_cast<int>(group.numSources());
        const auto numObjects = numSources + numElements;
        assert(objectRecordMask.size() == static_cast<size_t>(numObjects));
        allocBuf(q, d_elementRecordMask, numObjects);
        auto h_elementRecordMask = std::make_unique<bool[]>(numElements);
        for (int i = 0; i < numElements; ++i) { h_elementRecordMask[i] = objectRecordMask.shouldRecordElement(i); }
        alpaka::memcpy(q, *d_elementRecordMask, alpaka::createView(devHost, h_elementRecordMask.get(), numElements));

        const auto numEventsAtMost = numRaysBatchAtMost * maxEvents;

        // output events and compacted output events
#define X(type, name, flag)                                 \
    if (!!(attrRecordMask & RayAttrMask::flag)) {                \
        allocBuf(q, d_eventsBatch.name, numEventsAtMost);        \
        allocBuf(q, d_compactEventsBatch.name, numEventsAtMost); \
    }

        RAYX_X_MACRO_RAY_ATTR
#undef X

        // event storage flags, used for compaction of events
        allocBuf(q, d_eventStoreFlags, numEventsAtMost);

        return {
            .numSources  = numSources,
            .numElements = numElements,
        };
    }
};

/**
 * The MegaKernelTracer class implements a ray tracer using a mega-kernel strategy.
 *
 * This tracer processes rays in batches as they propagate through a beamline. It
 * generates rays from source definitions, traces them through optical elements, and
 * records interactions according to configurable event masks.
 *
 * Key features:
 * - Supports both sequential and non-sequential ray tracing modes.
 * - Uses Alpaka to enable parallel execution across multiple backends (CPU, GPU, etc.).
 * - Manages critical resources such as ray buffers, event buffers, and material data,
 *   ensuring optimized memory usage and efficient host–device data transfers.
 *
 * Workflow:
 * 1. Generate rays from sources.
 * 2. Execute the mega-kernel tracing function.
 * 3. Compact recorded events to optimize memory transfers.
 * 4. Transfer compacted recorded events back to the host.
 * 5. Aggregate results from all batches into a final Rays object for output.
 */
template <typename AccTag>
class MegaKernelTracer : public DeviceTracer {
  public:
    explicit MegaKernelTracer(int deviceIndex) : m_deviceIndex(deviceIndex) {}
    MegaKernelTracer(const MegaKernelTracer&)            = delete;
    MegaKernelTracer(MegaKernelTracer&&)                 = default;
    MegaKernelTracer& operator=(const MegaKernelTracer&) = delete;
    MegaKernelTracer& operator=(MegaKernelTracer&&)      = default;

  private:
    using Dim = alpaka::DimInt<1>;
    using Idx = int;
    using Acc = alpaka::TagToAcc<AccTag, Dim, Idx>;

    const int m_deviceIndex;
    Resources<Acc> m_resources;

    using GenRaysAcc = GenRays<Acc>;
    GenRaysAcc m_genRaysResources;

  public:
    virtual Rays trace(const Group& beamline, Sequential sequential, const ObjectMask& objectRecordMask, const RayAttrMask attrRecordMask,
                       const int maxEvents, const int maxBatchSize) override {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto platformHost = alpaka::PlatformCpu{};
        const auto devHost      = alpaka::getDevByIdx(platformHost, 0);
        const auto platformAcc  = alpaka::Platform<Acc>{};
        const auto devAcc       = alpaka::getDevByIdx(platformAcc, m_deviceIndex);
        using Queue             = alpaka::Queue<Acc, alpaka::Blocking>;
        auto q                  = Queue(devAcc);

        const auto sourceConf   = m_genRaysResources.update(q, beamline, maxBatchSize);
        const auto beamlineConf = m_resources.update(q, beamline, maxEvents, sourceConf.numRaysBatchAtMost, recordMask);

        RAYX_VERB << "trace beamline:";
        RAYX_VERB << "\tnum elements: " << beamlineConf.numElements;
        RAYX_VERB << "\tnum light sources: " << beamlineConf.numSources;
        RAYX_VERB << "\tsequential: " << (sequential == Sequential::Yes ? "yes" : "no");
        RAYX_VERB << "\tmax events: " << maxEvents;
        RAYX_VERB << "\tnum rays: " << sourceConf.numRaysTotal;
        RAYX_VERB << "\tmax batch size: " << maxBatchSize;
        RAYX_VERB << "\tbatch size: " << sourceConf.numRaysBatchAtMost;
        RAYX_VERB << "\tnum batches: " << sourceConf.numBatches;
        // TODO: object mask
        // TODO: attr mask
        RAYX_VERB << "\tbackend tag: " << AccTag{}.get_name();
        RAYX_VERB << "\tdevice index: " << m_deviceIndex;
        RAYX_VERB << "\tdevice name: " << alpaka::getName(devAcc);
        RAYX_VERB << "\thost device name: " << alpaka::getName(devHost);

        const auto numEventsAtMost      = sourceConf.numRaysBatchAtMost * maxEvents;
        auto numEventsTotal             = 0;
        auto isTooManyEvents            = false;
        auto h_eventStoreFlags          = std::make_unique<bool[]>(numEventsAtMost);
        auto h_eventStoreFlagsPrefixSum = std::vector<int>(numEventsAtMost);
        auto h_compactEvents            = Rays();
        auto h_compactEventsBatch       = std::vector<Rays>(sourceConf.numBatches);

        for (int batchIndex = 0; batchIndex < sourceConf.numBatches; ++batchIndex) {
            const auto numEventsBatchAtMost = batchConf.numRaysBatch * maxEvents;

            // clear buffers
            alpaka::memset(q, *m_resources.d_eventStoreFlags, 0, numEventsBatchAtMost);

            // generate input rays for batch
            auto batchConf = m_genRaysResources.genRaysBatch(devAcc, q, batchIndex);

            // trace current batch
            traceBatch(devAcc, q, beamlineConf.numSources, beamlineConf.numElements, maxEvents, sequential, attrRecordMask, batchConf);

            alpaka::memcpy(q, alpaka::createView(devHost, h_eventStoreFlags.get(), numEventsBatchAtMost), *m_resources.d_eventStoreFlags,
                           numEventsBatchAtMost);
            const auto h_eventStoreFlagsPrefixSumEnd =
                std::exclusive_scan(h_eventStoreFlags.get(), h_eventStoreFlags.get() + numEventsBatchAtMost, h_eventStoreFlagsPrefixSum.begin(), 0);
            const auto numEventsBatch =
                *(h_eventStoreFlagsPrefixSumEnd - 1);  // access the last element of the exclusive scan result to get the total count
            alpaka::memcpy(q, *m_resources.d_eventStoreFlagsPrefixSum, alpaka::createView(devHost, h_eventStoreFlagsPrefixSum, numEventsBatch),
                           numEventsBatch);

            // compact events to remove unused events
            compactEvents(devAcc, q, numEventsBatch);

            // transpose events from AoS to SoA data layout
            compactEventsToRays(devAcc, q, numEventsBatch);

            numEventsTotal += numEventsBatch;

#define X(type, name, flag)                                                                                   \
    if (!!(attrRecordMask & RayAttrMask::flag)) {                                                             \
        h_compactEvents.name.resize(numEventsTotal);                                                          \
        alpaka::memcpy(q, alpaka::createView(devHost, h_compactEventsBatch[batchIndex].name, numEventsBatch), \
                       *m_resources.d_compactEventsBatch.name, numEventsBatch);                               \
    }

            RAYX_X_MACRO_RAY_ATTR
#undef X

            // TODO: dont just get TooManyEvents. make a binary or of all event_types so that we get a mask of all events that happened. use
            // std::reduce or std::fold_left
            if (!!(attrRecordMask & RayAttrMask::EventType)) {
                const auto isTooManyEvents = [](const EventType eventType) { return eventType == EventType::TooManyEvents; };
                isTooManyEvents            = isTooManyEvents || std::any_of(h_compactEventsBatch[batchIndex].event_type.begin(),
                                                                            h_compactEventsBatch[batchIndex].event_type.end(), isTooManyEvents);
            }

            RAYX_VERB << "batch (" << (batchIndex + 1) << "/" << sourceConf.numBatches << ") with batch size = " << batchConf.numRaysBatch
                      << ", traced " << numEventsBatch << " events";
        }

#define X(type, name, flag)                                                                                  \
    if (!!(attrRecordMask & RayAttrMask::flag)) {                                                                 \
        auto batchOffset = 0;                                                                                     \
        h_compactEvents.name.resize(numEventsTotal);                                                              \
        for (auto batchIndex = 0; batchIndex < sourceConf.numBatches; ++batchIndex) {                             \
            std::copy(h_compactEventsBatch[batchIndex].name.begin(), h_compactEventsBatch[batchIndex].name.end(), \
                      h_compactEvents.name.begin() + batchOffset);                                                \
            batchOffset += h_compactEventsBatch[batchIndex].name.size();                                          \
        }                                                                                                         \
    }

        RAYX_X_MACRO_RAY_ATTR
#undef X

        h_compactEvents.num_events = numEventsTotal;

        RAYX_VERB << "number of recorded events: " << numEventsTotal;

        if (isTooManyEvents) RAYX_WARN << "capacity of events exceeded. could not record all events! consider increasing max events.";
        if (!(attrRecordMask & RayAttrMask::EventType))
            RAYX_WARN << "Unable to test events for EventType::TooManyEvents after tracing, because ray attribute event_type was not recorded";

        return h_compactEvents;
    }

  private:
    template <typename DevAcc, typename Queue>
    void traceBatch(DevAcc devAcc, Queue q, int numSources, int numElements, int maxEvents, Sequential sequential, RayAttrMask attrRecordMask,
                    GenRaysAcc::BatchConfig& batchConf) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto constState = ConstState{
            // constants
            .maxEvents   = maxEvents,
            .sequential  = sequential,
            .numSources  = numSources,
            .numElements = numElements,

            // buffers
            .elements          = alpaka::getPtrNative(*m_resources.d_elements),
            .materials = Materials { .indices = alpaka::getPtrNative(*m_resources.d_materialIndices), tables = alpaka::getPtrNative(*m_resources.d_materialTable) },
            .elementEecordMask = alpaka::getPtrNative(*m_resources.d_elementRecordMask),
            .attrRecordMask    = attrRecordMask,
            .rays              = raysBufToRaysPtr(batchConfig.d_rays),
        };

        const auto mutableState = MutableState{
            // buffers
            .events      = alpaka::getPtrNative(*m_resources.d_eventsBatch),
            .eventCounts = alpaka::getPtrNative(*m_resources.d_compactEventCounts),
            .rands       = alpaka::getPtrNative(*batchConf.d_rands),
        };

        if (sequential == Sequential::Yes) {
            RAYX_VERB << "execute TraceSequentialKernel";
            execWithValidWorkDiv<Acc>(devAcc, q, batchConf.numRaysBatch, BlockSizeConstraint::None{}, TraceSequentialKernel{}, constState,
                                      mutableState, batchConf.numRaysBatch);
        } else {
            RAYX_VERB << "execute TraceNonSequentialKernel";
            execWithValidWorkDiv<Acc>(devAcc, q, batchConf.numRaysBatch, BlockSizeConstraint::None{}, TraceNonSequentialKernel{}, constState,
                                      mutableState, batchConf.numRaysBatch);
        }
    }

    template <typename DevAcc, typename Queue>
    void compactEvents(DevAcc devAcc, Queue q, const int numEventsBatch, const RayAttrMask attrRecordMask) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        // we dont want to compact events if there are none
        if (numEventsBatch == 0) return;

        // TODO: compare performance to single scatter kernel execution handling all attributes
        auto execKernel = [&](auto& compactAttrBuf, const auto& attrBuf) {
            execWithValidWorkDiv<Acc>(devAcc, q, numEventsBatch, BlockSizeConstraint::None{}, ScatterCompactKernel{},
                                      alpaka::getPtrNative(*compactAttrBuf), alpaka::getPtrNative(*attrBuf),
                                      alpaka::getPtrNative(*m_resources.d_eventStoreFlagsPrefixSum),
                                      alpaka::getPtrNative(*m_resources.d_eventStoreFlags), numEventsBatch);
        };

#define X(type, name, flag)                                                                \
    if (!!(attrRecordMask & RayAttrMask::flag)) {                                          \
        RAYX_VERB << "execute ScatterCompactKernel for ray attribute: "##name;             \
        execKernel(m_resources.d_compactEventsBatch.name, m_resources.d_eventsBatch.name); \
    }

        RAYX_X_MACRO_RAY_ATTR
#undef X
    }

    template <typename DevAcc, typename Queue>
    void gatherCompactEvents(DevAcc devAcc, Queue q, const int numRaysBatch, const int batchStartRayIndex, const int maxEvents,
                             const int numEventsBatch) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        // we dont want to compact events if there are none
        if (numEventsBatch == 0) return;

        RAYX_VERB << "execute GatherIndicesKernel";
        execWithValidWorkDiv<Acc>(devAcc, q, numRaysBatch, BlockSizeConstraint::None{}, GatherIndicesKernel{},
                                  alpaka::getPtrNative(*m_resources.d_compactEventGatherSrcIndices),
                                  alpaka::getPtrNative(*m_resources.d_compactEventCounts), alpaka::getPtrNative(*m_resources.d_compactEventOffsets),
                                  alpaka::getPtrNative(*m_resources.soaEvents.path_id), batchStartRayIndex, maxEvents, numRaysBatch);

        RAYX_VERB << "execute GatherKernel";
        execWithValidWorkDiv<Acc>(devAcc, q, numEventsBatch, BlockSizeConstraint::None{}, GatherKernel{},
                                  alpaka::getPtrNative(*m_resources.d_compactEventsBatch), alpaka::getPtrNative(*m_resources.d_eventsBatch),
                                  alpaka::getPtrNative(*m_resources.d_compactEventGatherSrcIndices), numEventsBatch);
    }

    template <typename DevAcc, typename Queue>
    void compactEventsToRays(DevAcc devAcc, Queue q, const int numEventsBatch) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        // we dont want to transpose events if there are none
        if (numEventsBatch == 0) return;

        RAYX_VERB << "execute CompactRaysToRaysKernel";
        execWithValidWorkDiv<Acc>(devAcc, q, numEventsBatch, BlockSizeConstraint::None{}, CompactRaysToRaysKernel{},
                                  raySoaBufToRaySoaRef(m_resources.soaEvents), alpaka::getPtrNative(*m_resources.d_compactEventsBatch),
                                  numEventsBatch);
    }
};

}  // namespace RAYX
