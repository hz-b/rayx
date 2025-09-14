#pragma once

#include <numeric>
#include <set>

#include "Beamline/Beamline.h"
#include "Debug/Instrumentor.h"
#include "DeviceTracer.h"
#include "GenRays.h"
#include "Material/Material.h"
#include "Random.h"
#include "Shader/DynamicElements.h"
#include "Util.h"

namespace RAYX {
namespace {

struct TraceSequentialKernel {
    template <typename Acc>
    RAYX_FN_ACC void operator()(const Acc& acc, const ConstState constState, MutableState mutableState, const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) traceSequential(gid, constState, mutableState);
    }
};

struct TraceNonSequentialKernel {
    template <typename Acc>
    RAYX_FN_ACC void operator()(const Acc& acc, const ConstState constState, MutableState mutableState, const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) traceNonSequential(gid, constState, mutableState);
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
    /// output events
    OptBuf<Acc, Ray> d_events;
    /// output events, compacted for faster transfer
    OptBuf<Acc, Ray> d_compactEvents;
    /// number of events for each input ray
    OptBuf<Acc, int> d_compactEventCounts;
    /// offset into compact events buffer for earch input ray
    OptBuf<Acc, int> d_compactEventOffsets;
    /// maps compact events buffer indices to events buffer indices
    OptBuf<Acc, int> d_compactEventGatherSrcIndices;

    /// ray attributes layed out as structure of arrays (SoA)
    RaySoaBuf<Acc> soaEvents;

    /// holds configuration state of allocated resources. required to trace correctly
    struct BeamlineConfig {
        int numSources;
        int numElements;
    };

    /// update resources
    template <typename Queue>
    BeamlineConfig update(Queue q, const Group& group, int maxEvents, int numRaysBatchAtMost, const ObjectRecordMask& objectRecordMask) {
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

        // output events
        allocBuf(q, d_events, numRaysBatchAtMost * maxEvents);

        // compact events
        allocBuf(q, d_compactEvents, numRaysBatchAtMost * maxEvents);
        allocBuf(q, d_compactEventCounts, numRaysBatchAtMost);
        allocBuf(q, d_compactEventOffsets, numRaysBatchAtMost);
        allocBuf(q, d_compactEventGatherSrcIndices, numRaysBatchAtMost * maxEvents);

        // SoA events
#define X(type, name, flag, map) allocBuf(q, soaEvents.name, numRaysBatchAtMost* maxEvents);

        RAYX_X_MACRO_RAY_ATTR
#undef X

        return {
            .numSources  = numSources,
            .numElements = numElements,
        };
    }
};

// this class interfaces the mega kernel tracer implementation
// it takes care of creating, destroying and updating device resources
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
    virtual Rays trace(const Group& beamline, Sequential sequential, const ObjectRecordMask& objectRecordMask, const RayAttrFlag attr,
                       const int maxEvents, const int maxBatchSize) override;
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

    auto numEventsTotal      = 0;
    auto isTooManyEvents     = false;
    auto compactEventCounts  = std::vector<int>(sourceConf.numRaysBatchAtMost);
    auto compactEventOffsets = std::vector<int>(sourceConf.numRaysBatchAtMost);
    auto compactEvents       = std::vector<Ray>(sourceConf.numRaysBatchAtMost * maxEvents);

    auto raysoaBatch = std::vector<Rays>(sourceConf.numBatches);

    for (int batchIndex = 0; batchIndex < sourceConf.numBatches; ++batchIndex) {
        // clear buffers
        alpaka::memset(q, *m_resources.d_compactEventCounts, 0);

        // generate input rays for batch
        auto batchConf = m_genRaysResources.genRaysBatch(devAcc, q, batchIndex);

        // trace current batch
        traceBatch(devAcc, q, beamlineConf.numSources, beamlineConf.numElements, maxEvents, sequential, attr, batchConf);

        alpaka::memcpy(q, alpaka::createView(devHost, compactEventCounts, batchConf.numRaysBatch), *m_resources.d_compactEventCounts,
                       batchConf.numRaysBatch);
        std::exclusive_scan(compactEventCounts.begin(), compactEventCounts.begin() + batchConf.numRaysBatch, compactEventOffsets.begin(), 0);
        alpaka::memcpy(q, *m_resources.d_compactEventOffsets, alpaka::createView(devHost, compactEventOffsets, batchConf.numRaysBatch),
                       batchConf.numRaysBatch);
        const auto numEventsBatch = compactEventOffsets[batchConf.numRaysBatch - 1] + compactEventCounts[batchConf.numRaysBatch - 1];

        // make events compact by gathering events into compactEvents using compactEventCounts and compactEventOffsets
        gatherCompactEvents(devAcc, q, batchConf.numRaysBatch, batchConf.batchStartRayIndex, maxEvents, numEventsBatch);

        // transpose events from AoS to SoA data layout
        compactEventsToRays(devAcc, q, numEventsBatch);

#define X(type, name, flag, map)                                                                      \
    if ((attr & RayAttrFlag::flag) != RayAttrFlag::None) {                                            \
        raysoaBatch[batchIndex].name.resize(numEventsBatch);                                          \
        auto name##_view = alpaka::createView(devHost, raysoaBatch[batchIndex].name, numEventsBatch); \
        alpaka::memcpy(q, name##_view, *m_resources.soaEvents.name, numEventsBatch);                  \
    }

        RAYX_X_MACRO_RAY_ATTR
#undef X

        isTooManyEvents = isTooManyEvents || checkTooManyEvents(compactEvents, compactEventCounts, compactEventOffsets, batchConf.numRaysBatch);

        RAYX_VERB << "batch (" << (batchIndex + 1) << "/" << sourceConf.numBatches << ") with batch size = " << batchConf.numRaysBatch << ", traced "
                  << numEventsBatch << " events";
        numEventsTotal += numEventsBatch;
    }

    Rays raysoa;
#define X(type, name, flag, map)                                                                                                    \
    if ((attr & RayAttrFlag::flag) != RayAttrFlag::None) {                                                                          \
        int batchOffset = 0;                                                                                                        \
        raysoa.name.resize(numEventsTotal);                                                                                         \
        for (int batchIndex = 0; batchIndex < sourceConf.numBatches; ++batchIndex) {                                                \
            std::copy(raysoaBatch[batchIndex].name.begin(), raysoaBatch[batchIndex].name.end(), raysoa.name.begin() + batchOffset); \
            batchOffset += raysoaBatch[batchIndex].name.size();                                                                     \
        }                                                                                                                           \
    }

    RAYX_X_MACRO_RAY_ATTR
#undef X

    // find the number of ray paths, that have at least 1 event, which is equal to the unique values in compactEventOffsets.
    // std::unique requries a sorted array, which is the case for compactEventOffsets
    raysoa.num_paths =
        static_cast<int>(std::distance(compactEventOffsets.begin(), std::unique(compactEventOffsets.begin(), compactEventOffsets.end()))) -
        (numEventsTotal ? 0 : 1);
    raysoa.num_events = numEventsTotal;

    if (isTooManyEvents) RAYX_WARN << "capacity of events exceeded. could not record all events! consider increasing max events.";
    RAYX_VERB << "number of recorded events: " << numEventsTotal;
    return raysoa;
}

private : template <typename DevAcc, typename Queue>
          void
          traceBatch(DevAcc devAcc, Queue q, int numSources, int numElements, int maxEvents, Sequential sequential, RayAttrFlag attr,
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
        .materialIndices   = alpaka::getPtrNative(*m_resources.d_materialIndices),
        .materialTables    = alpaka::getPtrNative(*m_resources.d_materialTable),
        .elementEecordMask = alpaka::getPtrNative(*m_resources.d_elementRecordMask),
        .attrRecordMask    = attr,
        .rays              = alpaka::getPtrNative(*batchConf.d_rays),
    };

    const auto mutableState = MutableState{
        // buffers
        .events      = alpaka::getPtrNative(*m_resources.d_events),
        .eventCounts = alpaka::getPtrNative(*m_resources.d_compactEventCounts),
        .rands       = alpaka::getPtrNative(*batchConf.d_rands),
    };

    if (sequential == Sequential::Yes) {
        RAYX_VERB << "execute TraceSequentialKernel";
        execWithValidWorkDiv<Acc>(devAcc, q, batchConf.numRaysBatch, BlockSizeConstraint::None{}, TraceSequentialKernel{}, constState, mutableState,
                                  batchConf.numRaysBatch);
    } else {
        RAYX_VERB << "execute TraceNonSequentialKernel";
        execWithValidWorkDiv<Acc>(devAcc, q, batchConf.numRaysBatch, BlockSizeConstraint::None{}, TraceNonSequentialKernel{}, constState,
                                  mutableState, batchConf.numRaysBatch);
    }
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
                              alpaka::getPtrNative(*m_resources.d_compactEvents), alpaka::getPtrNative(*m_resources.d_events),
                              alpaka::getPtrNative(*m_resources.d_compactEventGatherSrcIndices), numEventsBatch);
}

template <typename DevAcc, typename Queue>
void compactEventsToRays(DevAcc devAcc, Queue q, const int numEventsBatch) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // we dont want to transpose events if there are none
    if (numEventsBatch == 0) return;

    RAYX_VERB << "execute CompactRaysToRaysKernel";
    execWithValidWorkDiv<Acc>(devAcc, q, numEventsBatch, BlockSizeConstraint::None{}, CompactRaysToRaysKernel{},
                              raySoaBufToRaySoaRef(m_resources.soaEvents), alpaka::getPtrNative(*m_resources.d_compactEvents), numEventsBatch);
}
};  // namespace RAYX

}  // namespace RAYX
