#pragma once

#include <numeric>

#include "Beamline/Beamline.h"
#include "Debug/Instrumentor.h"
#include "DeviceTracer.h"
#include "Material/Material.h"
#include "Random.h"
#include "Shader/DynamicElements.h"
#include "Util.h"

namespace RAYX {
namespace {

struct DynamicElementsKernel {
    template <typename Acc>
    RAYX_FN_ACC void operator()(const Acc& acc, const InvState inv, OutputEvents outputEvents) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < inv.batchSize) dynamicElements(gid, inv, outputEvents);
    }
};

}  // unnamed namespace

/// keeps track of all resources used by the tracer. manages allocation and update of buffers
/// note: members starting with h_ reside on host side, while d_ reside on device side
template <typename Acc>
struct Resources {
    using Dim = alpaka::DimInt<1>;
    using Idx = int;

    template <typename T>
    using Buf = std::optional<alpaka::Buf<Acc, T, Dim, Idx>>;

    // resources per program execution. constant per program execution
    /// material data
    Buf<int> d_materialIndices;
    Buf<double> d_materialTable;

    // resources per beamline. constant per beamline
    /// beamline elements
    Buf<OpticalElement> d_elements;
    /// all rays generated from all light sources
    std::vector<Ray> h_rays;

    // resources per batch. constant per batch
    /// input rays
    Buf<Ray> d_rays;

    // output events per tracing. required if 'events' is enabled in output config
    /// output events
    Buf<Ray> d_events;
    /// output events, compacted for faster transfer
    Buf<Ray> d_compactEvents;
    /// number of events for each input ray
    Buf<int> d_compactEventCounts;
    /// offset into compact events buffer for earch input ray
    Buf<int> d_compactEventOffsets;
    /// maps compact events buffer indices to events buffer indices
    Buf<int> d_compactEventGatherSrcIndices;

    /// holds configuration state of allocated resources. required to trace correctly
    struct Config {
        int numElements;
        int numRaysTotal;
        int preferredBatchSize;
        int numBatches;
    };

    /// update resources
    template <typename Queue>
    Config update(Queue q, const Group& group, int maxEvents, int maxBatchSize) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto platformHost = alpaka::PlatformCpu{};
        const auto devHost = alpaka::getDevByIdx(platformHost, 0);

        // material data
        const auto materialTables = group.calcMinimalMaterialTables();
        const auto& materialIndices = materialTables.indices;
        const auto& materialTable = materialTables.materials;
        const auto numMaterialIndices = static_cast<int>(materialIndices.size());
        const auto materialTableSize = static_cast<int>(materialTable.size());
        allocBuf(q, d_materialIndices, materialIndices.size());
        allocBuf(q, d_materialTable, materialTable.size());
        alpaka::memcpy(q, *d_materialIndices, alpaka::createView(devHost, materialIndices, numMaterialIndices));
        alpaka::memcpy(q, *d_materialTable, alpaka::createView(devHost, materialTable, materialTableSize));

        // beamline elements
        const auto elements = group.compileElements();
        const auto numElements = static_cast<int>(elements.size());
        allocBuf(q, d_elements, numElements);
        alpaka::memcpy(q, *d_elements, alpaka::createView(devHost, elements, numElements));

        // input rays
        h_rays = group.compileSources(1);  // TODO: generate rays on device
        const auto numRaysTotal = static_cast<int>(h_rays.size());
        const auto preferredBatchSize = std::min(numRaysTotal, maxBatchSize);
        allocBuf(q, d_rays, preferredBatchSize);

        // output events
        allocBuf(q, d_events, preferredBatchSize * maxEvents);
        allocBuf(q, d_compactEvents, preferredBatchSize * maxEvents);
        allocBuf(q, d_compactEventCounts, preferredBatchSize);
        allocBuf(q, d_compactEventOffsets, preferredBatchSize);
        allocBuf(q, d_compactEventGatherSrcIndices, preferredBatchSize * maxEvents);

        const auto numBatches = ceilIntDivision(h_rays.size(), preferredBatchSize);
        return {
            .numElements = numElements,
            .numRaysTotal = numRaysTotal,
            .preferredBatchSize = preferredBatchSize,
            .numBatches = numBatches,
        };
    }
};

// this class interfaces the mega kernel tracer implementation
// it takes care of creating, destroying and updating device resources
template <typename AccTag>
class MegaKernelTracer : public DeviceTracer {
  public:
    explicit MegaKernelTracer(int deviceIndex) : m_deviceIndex(deviceIndex) {}
    MegaKernelTracer(const MegaKernelTracer&) = delete;
    MegaKernelTracer(MegaKernelTracer&&) = default;
    MegaKernelTracer& operator=(const MegaKernelTracer&) = delete;
    MegaKernelTracer& operator=(MegaKernelTracer&&) = default;

  private:
    using Dim = alpaka::DimInt<1>;
    using Idx = int;
    using Acc = alpaka::TagToAcc<AccTag, Dim, Idx>;

    const int m_deviceIndex;
    Resources<Acc> m_resources;

  public:
    virtual BundleHistory trace(const Group& beamline, const Sequential sequential, const int maxBatchSize, const int maxEvents,
                                const int recordElementIndex) override {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto platformHost = alpaka::PlatformCpu{};
        const auto devHost = alpaka::getDevByIdx(platformHost, 0);
        const auto platformAcc = alpaka::Platform<Acc>{};
        const auto devAcc = alpaka::getDevByIdx(platformAcc, m_deviceIndex);
        using Queue = alpaka::Queue<Acc, alpaka::Blocking>;  // TODO
        auto q = Queue(devAcc);

        const auto conf = m_resources.update(q, beamline, maxEvents, maxBatchSize);
        const auto randomSeed = randomDouble();

        RAYX_VERB << "tracing beamline:";
        RAYX_VERB << "\tnum elements: " << conf.numElements;
        RAYX_VERB << "\tnum light sources: " << beamline.numSources();
        RAYX_VERB << "\tsequential: " << (sequential == Sequential::Yes ? "yes" : "no");
        RAYX_VERB << "\tmax events: " << maxEvents;
        RAYX_VERB << "\tnum rays: " << conf.numRaysTotal;
        RAYX_VERB << "\tmax batch size: " << maxBatchSize;
        RAYX_VERB << "\tpreferred batch size: " << conf.preferredBatchSize;
        RAYX_VERB << "\tnum batches: " << conf.numBatches;
        RAYX_VERB << "\ttracing backend tag: " << AccTag{}.get_name();
        RAYX_VERB << "\ttracing device index: " << m_deviceIndex;
        RAYX_VERB << "\ttracing device name: " << alpaka::getName(devAcc);
        RAYX_VERB << "\thost device name: " << alpaka::getName(devHost);

        auto bundleHistory = BundleHistory{};
        auto numEventsTotal = 0;
        auto isTooManyEvents = false;
        auto compactEventCounts = std::vector<int>(conf.preferredBatchSize);
        auto compactEventOffsets = std::vector<int>(conf.preferredBatchSize);
        auto compactEvents = std::vector<Ray>(conf.preferredBatchSize * maxEvents);

        for (int batchIndex = 0; batchIndex < conf.numBatches; ++batchIndex) {
            const auto batchStartRayIndex = batchIndex * conf.preferredBatchSize;
            const auto numRemainingRays = conf.numRaysTotal - batchStartRayIndex;
            const auto batchSize = std::min(numRemainingRays, conf.preferredBatchSize);

            // clear buffers
            alpaka::memset(q, *m_resources.d_compactEventCounts, 0);

            // transfer rays from host to device for current batch
            auto raysView = alpaka::createView(devHost, m_resources.h_rays, conf.numRaysTotal);
            auto raysViewBatch = alpaka::createSubView(raysView, batchSize, batchStartRayIndex);
            alpaka::memcpy(q, *m_resources.d_rays, raysViewBatch);

            // trace current batch
            traceBatch(devAcc, q, conf.numElements, conf.numRaysTotal, batchSize, batchStartRayIndex, maxEvents, recordElementIndex, randomSeed,
                       sequential);

            // prefix sum on compactEventCounts to get compactEventOffsets
            alpaka::memcpy(q, alpaka::createView(devHost, compactEventCounts, batchSize), *m_resources.d_compactEventCounts, batchSize);
            std::exclusive_scan(compactEventCounts.begin(), compactEventCounts.begin() + batchSize, compactEventOffsets.begin(), 0);
            alpaka::memcpy(q, *m_resources.d_compactEventOffsets, alpaka::createView(devHost, compactEventOffsets, batchSize), batchSize);
            const auto numEventsBatch = compactEventOffsets[batchSize - 1] + compactEventCounts[batchSize - 1];

            // make events compact by gathering events into compactEvents using compactEventCounts and compactEventOffsets
            gatherCompactEvents(devAcc, q, batchSize, maxEvents, numEventsBatch);

            // transfer events from device to host for curent batch
            alpaka::memcpy(q, alpaka::createView(devHost, compactEvents, numEventsBatch), *m_resources.d_compactEvents, numEventsBatch);
            isTooManyEvents = isTooManyEvents || checkTooManyEvents(compactEvents, compactEventCounts, compactEventOffsets, batchSize);
            collectCompactEventsIntoBundleHistory(bundleHistory, compactEvents, compactEventCounts, compactEventOffsets, batchSize);

            RAYX_VERB << "batch (" << (batchIndex + 1) << "/" << conf.numBatches << ") with batch size = " << batchSize << ", traced "
                      << numEventsBatch << " events";
            numEventsTotal += numEventsBatch;
        }

        if (isTooManyEvents) RAYX_WARN << "capacity of events exceeded. could not record all events! consider increasing max events.";
        RAYX_VERB << "number of recorded events: " << numEventsTotal;
        return bundleHistory;
    }

  private:
    template <typename DevAcc, typename Queue>
    void traceBatch(DevAcc devAcc, Queue q, int numElements, int numRaysTotal, int batchSize, int batchStartRayIndex, int maxEvents,
                    int recordElementIndex, double randomSeed, Sequential sequential) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        // inputs
        const auto inv = InvState{
            // constants
            .numRaysTotal = numRaysTotal,
            .batchSize = batchSize,
            .batchStartRayIndex = batchStartRayIndex,
            .maxEvents = maxEvents,
            .recordElementIndex = recordElementIndex,
            .randomSeed = randomSeed,
            .sequential = sequential,

            // buffers
            .elements = alpaka::getPtrNative(*m_resources.d_elements),
            .numElements = numElements,
            .materialIndices = alpaka::getPtrNative(*m_resources.d_materialIndices),
            .materialTables = alpaka::getPtrNative(*m_resources.d_materialTable),
            .rays = alpaka::getPtrNative(*m_resources.d_rays),
        };

        // outputs
        const auto outputEvents = OutputEvents{
            // buffers
            .events = alpaka::getPtrNative(*m_resources.d_events),
            .eventCounts = alpaka::getPtrNative(*m_resources.d_compactEventCounts),
        };

        execWithValidWorkDiv<Acc>(devAcc, q, batchSize, MaxBlockSizeConstraint{128}, DynamicElementsKernel{}, inv, outputEvents);
    }

    template <typename DevAcc, typename Queue>
    void gatherCompactEvents(DevAcc devAcc, Queue q, const int batchSize, const int maxEvents, const int numEventsBatch) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        // we dont want to compact events if there are none
        if (numEventsBatch == 0) return;

        execWithValidWorkDiv<Acc>(devAcc, q, batchSize, MaxBlockSizeConstraint{128}, GatherIndicesKernel{}, alpaka::getPtrNative(*m_resources.d_compactEventGatherSrcIndices),
                                  alpaka::getPtrNative(*m_resources.d_compactEventCounts), alpaka::getPtrNative(*m_resources.d_compactEventOffsets),
                                  maxEvents, batchSize);

        execWithValidWorkDiv<Acc>(devAcc, q, numEventsBatch, MaxBlockSizeConstraint{128}, GatherKernel{}, alpaka::getPtrNative(*m_resources.d_compactEvents),
                                  alpaka::getPtrNative(*m_resources.d_events), alpaka::getPtrNative(*m_resources.d_compactEventGatherSrcIndices),
                                  numEventsBatch);
    }
};

}  // namespace RAYX
