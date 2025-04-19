#pragma once

#include <alpaka/alpaka.hpp>
#include <numeric>

#include "Beamline/Beamline.h"
#include "Debug/Instrumentor.h"
#include "DeviceTracer.h"
#include "Material/Material.h"
#include "Random.h"
#include "Shader/DynamicElements.h"

namespace RAYX {
namespace {

inline int calcNumBatches(const int batchSize, const int numRaysTotal) { return (batchSize + numRaysTotal - 1) / batchSize; }

inline int nextPowerOfTwo(const int value) { return static_cast<int>(glm::pow(2, glm::ceil(glm::log(value) / glm::log(2)))); }

/// conditionally allocate buffer with specified minimum size.
/// if the buffer already fulfills size requirements, this function does nothing. thus, this function never shrinks a buffer.
/// actual allocation size is nextPowerOfTwo(size).
/// this function is designed to optimize the repetitive use of the buffer with potentially different size requirements (e.g. tracing multiple
/// beamlines one after the other)
template <typename Queue, typename Buf>
inline void allocBuf(Queue q, std::optional<Buf>& buf, const int size) {
    using Idx = alpaka::Idx<Buf>;
    using Elem = alpaka::Elem<Buf>;

    const auto shouldAlloc = !buf || alpaka::getExtents(*buf)[0] < size;
    if (shouldAlloc) RAYX_VERB << (!buf ? "new alloc on device: " : "realloc on device: ") << nextPowerOfTwo(size * sizeof(Elem)) << " bytes";
    if (shouldAlloc) buf = alpaka::allocAsyncBufIfSupported<Elem, Idx>(q, nextPowerOfTwo(size));
}

inline bool checkTooManyEvents(const std::vector<Ray>& compactEvents, const std::vector<int>& compactEventCounts,
                               const std::vector<int>& compactEventOffsets, const int batchSize) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (int i = 0; i < batchSize; ++i) {
        if (0 < compactEventCounts[i]) {
            const auto offset = compactEventOffsets[i];
            const auto count = compactEventCounts[i];
            const auto lastEventIndex = offset + count - 1;
            if (compactEvents[lastEventIndex].m_eventType == EventType::TooManyEvents) return true;
        }
    }

    return false;
}

inline void collectCompactEventsIntoBundleHistory(BundleHistory& bundleHistory, const std::vector<Ray>& compactEvents,
                                                  const std::vector<int>& compactEventCounts, const std::vector<int>& compactEventOffsets,
                                                  const int batchSize) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (int i = 0; i < batchSize; i++) {
        const auto begin = compactEvents.data() + compactEventOffsets[i];
        const auto end = begin + compactEventCounts[i];

        // add events to history, only if there are events
        bool hasEvents = 0 < std::distance(begin, end);
        if (hasEvents) bundleHistory.emplace_back(begin, end);
    }
}

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

        const auto numBatches = calcNumBatches(preferredBatchSize, h_rays.size());
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
    virtual BundleHistory trace(const Group& beamline, const Sequential sequential, const int maxBatchSize, const int maxEvents) override {
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
            alpaka::memset(q, *m_resources.d_compactEventOffsets, 0);

            // transfer rays from host to device for current batch
            auto raysView = alpaka::createView(devHost, m_resources.h_rays, conf.numRaysTotal);
            auto raysViewBatch = alpaka::createSubView(raysView, batchSize, batchStartRayIndex);
            alpaka::memcpy(q, *m_resources.d_rays, raysViewBatch);

            // trace current batch
            traceBatch(devAcc, q, conf.numElements, conf.numRaysTotal, batchSize, batchStartRayIndex, maxEvents, randomSeed, sequential);

            // prefix sum on compactEventCounts to get compactEventOffsets
            alpaka::memcpy(q, alpaka::createView(devHost, compactEventCounts, batchSize), *m_resources.d_compactEventCounts, batchSize);
            std::exclusive_scan(compactEventCounts.begin(), compactEventCounts.begin() + batchSize, compactEventOffsets.begin(), 0);
            alpaka::memcpy(q, *m_resources.d_compactEventOffsets, alpaka::createView(devHost, compactEventOffsets, batchSize), batchSize);
            const auto numEventsBatch = compactEventOffsets.back() + compactEventCounts[batchSize - 1];

            // make events compact by gathering events into compactEvents using compactEventCounts and compactEventOffsets
            gatherCompactEvents(devAcc, q, batchSize, maxEvents);

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

    // private:
    struct DynamicElementsKernel {
        template <typename Acc>
        RAYX_FN_ACC void operator()(const Acc& acc, const InvState inv, OutputEvents outputEvents) const {
            const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

            if (gid < inv.batchSize) dynamicElements(gid, inv, outputEvents);
        }
    };

    template <typename DevAcc, typename Queue>
    void traceBatch(DevAcc devAcc, Queue q, int numElements, int numRaysTotal, int batchSize, int batchStartRayIndex, int maxEvents,
                    double randomSeed, Sequential sequential) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        // inputs
        const auto inv = InvState{
            // constants
            .numRaysTotal = numRaysTotal,
            .batchSize = batchSize,
            .batchStartRayIndex = batchStartRayIndex,
            .maxEvents = maxEvents,
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

        execWithValidWorkDiv(devAcc, q, batchSize, DynamicElementsKernel{}, inv, outputEvents);
    }

    struct GatherKernel {
        template <typename Acc, typename T>
        RAYX_FN_ACC void operator()(const Acc& acc, T* dst, const T* src, const int* srcSizes, const int* srcOffsets, const int srcMaxSize,
                                    const int n) const {
            const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

            if (gid < n) {
                auto offset = srcOffsets[gid];
                auto size = srcSizes[gid];

                for (int i = 0; i < size; ++i) {
                    const auto idst = offset + i;
                    const auto isrc = gid * srcMaxSize + i;
                    dst[idst] = src[isrc];
                }
            }
        }
    };

    template <typename DevAcc, typename Queue>
    void gatherCompactEvents(DevAcc devAcc, Queue q, int batchSize, int maxEvents) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        execWithValidWorkDiv(devAcc, q, batchSize, GatherKernel{}, alpaka::getPtrNative(*m_resources.d_compactEvents),
                             alpaka::getPtrNative(*m_resources.d_events), alpaka::getPtrNative(*m_resources.d_compactEventCounts),
                             alpaka::getPtrNative(*m_resources.d_compactEventOffsets), maxEvents, batchSize);
    }

    template <typename Queue, typename DevAcc, typename Kernel, typename... Args>
    void execWithValidWorkDiv(DevAcc devAcc, Queue q, const int numElements, const Kernel& kernel, Args&&... args) {
        const auto conf = alpaka::KernelCfg<Acc>{numElements, 1};
        // TODO: make sure blockSize is divisible by 128
        const auto workDiv = alpaka::getValidWorkDiv(conf, devAcc, kernel, std::forward<Args>(args)...);

        RAYX_VERB << "executing kernel with launch config: "
                   << "blocks = " << workDiv.m_gridBlockExtent[0] << ", "
                   << "threads = " << workDiv.m_blockThreadExtent[0] << ", "
                   << "elements = " << workDiv.m_threadElemExtent[0];  // TODO: why does m_blockThreadExtent not divide by warpSize?

        alpaka::exec<Acc>(q, workDiv, kernel, std::forward<Args>(args)...);
    }
};

}  // namespace RAYX
