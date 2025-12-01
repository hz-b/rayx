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

namespace rayx {
namespace {

constexpr int WARP_SIZE            = 32;
constexpr int GRID_STRIDE_MULTIPLE = WARP_SIZE;

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
                                const bool* __restrict flags, const int n) const {
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
    /// beamline object transforms
    OptBuf<Acc, ObjectTransform> d_objectTransforms;

    /// beamline elements
    OptBuf<Acc, OpticalElement> d_elements;

    /// mask for which elements to record events
    OptBuf<Acc, bool> d_objectRecordMask;

    // output events per tracing. required if 'events' is enabled in output config
    /// output events from tracer kernel
    RaysBuf<Acc> d_eventsBatch;
    /// output events, compacted for faster transfer
    RaysBuf<Acc> d_compactEventsBatch;
    /// flag for each possible ouput event, wether it was stored or not. used for compaction
    OptBuf<Acc, bool> d_eventStoreFlags;
    OptBuf<Acc, int> d_eventStoreFlagsPrefixSum;

    /// holds configuration state of allocated resources. required to trace correctly
    struct BeamlineConfig {
        int numSources;
        int numElements;
    };

    /// update resources
    template <typename Queue>
    BeamlineConfig update(Queue q, const Group& group, int maxEvents, int numRaysBatchAtMost, const ObjectIndexMask& objectRecordMask,
                          const RayAttrMask attrRecordMask) {
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
        // TODO: this should be two arrays, one of elements, one for transforms
        const auto elementsAndTransforms = group.compileElements();
        auto elements                    = std::vector<OpticalElement>(elementsAndTransforms.size());
        std::transform(elementsAndTransforms.begin(), elementsAndTransforms.end(), elements.begin(),
                       [](const OpticalElementAndTransform& e) { return e.element; });
        const auto numElements = static_cast<int>(elements.size());
        allocBuf(q, d_elements, numElements);
        alpaka::memcpy(q, *d_elements, alpaka::createView(devHost, elements, numElements));

        const auto sources    = group.getSources();
        const auto numSources = static_cast<int>(sources.size());
        const auto numObjects = numSources + numElements;

        // object transforms
        // TODO: compiling of sources/elements should be revisited
        auto h_objectTransforms = std::vector<ObjectTransform>(numObjects);
        std::transform(sources.begin(), sources.end(), h_objectTransforms.begin(), [](const DesignSource* designSource) {
            return ObjectTransform{
                // TODO: make sure to do this DesignPlane:XZ thing correctly
                .m_inTrans  = calcTransformationMatrices(designSource->getPosition(), designSource->getOrientation(), true, DesignPlane::XZ),
                .m_outTrans = calcTransformationMatrices(designSource->getPosition(), designSource->getOrientation(), false, DesignPlane::XZ),
            };
        });
        std::transform(elementsAndTransforms.begin(), elementsAndTransforms.end(), h_objectTransforms.begin() + numSources,
                       [](const OpticalElementAndTransform& e) { return e.transform; });
        allocBuf(q, d_objectTransforms, numObjects);
        alpaka::memcpy(q, *d_objectTransforms, alpaka::createView(devHost, h_objectTransforms, numObjects), numObjects);

        // object record mask
        allocBuf(q, d_objectRecordMask, numObjects);
        auto h_objectRecordMask = std::make_unique<bool[]>(numObjects);
        for (int i = 0; i < numObjects; ++i) { h_objectRecordMask[i] = objectRecordMask.shouldRecordObject(i); }
        alpaka::memcpy(q, *d_objectRecordMask, alpaka::createView(devHost, h_objectRecordMask.get(), numObjects));

        const auto numEventsBatchAtMost                     = numRaysBatchAtMost * maxEvents;
        const auto numEventsBatchAtMostAccountForGridStride = nextMultiple(numRaysBatchAtMost, GRID_STRIDE_MULTIPLE) * maxEvents;

        // output events and compacted output events
        allocRaysBuf(q, attrRecordMask, d_eventsBatch, numEventsBatchAtMostAccountForGridStride);
        allocRaysBuf(q, attrRecordMask, d_compactEventsBatch, numEventsBatchAtMost);

        // event storage flags, used for compaction of events
        allocBuf(q, d_eventStoreFlags, numEventsBatchAtMostAccountForGridStride);
        allocBuf(q, d_eventStoreFlagsPrefixSum, numEventsBatchAtMostAccountForGridStride);

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
    virtual Rays trace(const Group& beamline, Sequential sequential, const ObjectIndexMask& objectRecordMask, const RayAttrMask attrRecordMask,
                       const int maxEventsElements, const int maxBatchSize) override {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto maxEventsSources = 1;
        const auto maxEvents        = maxEventsSources + maxEventsElements;

        const auto platformHost = alpaka::PlatformCpu{};
        const auto devHost      = alpaka::getDevByIdx(platformHost, 0);
        const auto platformAcc  = alpaka::Platform<Acc>{};
        const auto devAcc       = alpaka::getDevByIdx(platformAcc, m_deviceIndex);
        using Queue             = alpaka::Queue<Acc, alpaka::Blocking>;
        auto q                  = Queue(devAcc);

        const auto sourceConf   = m_genRaysResources.update(q, beamline, maxBatchSize);
        const auto beamlineConf = m_resources.update(q, beamline, maxEvents, sourceConf.numRaysBatchAtMost, objectRecordMask, attrRecordMask);

        RAYX_VERB << "trace beamline:";
        RAYX_VERB << "\t- num sources: " << beamlineConf.numSources;
        RAYX_VERB << "\t- num elements: " << beamlineConf.numElements;
        RAYX_VERB << "\t- sequential: " << (sequential == Sequential::Yes ? "yes" : "no");
        RAYX_VERB << "\t- max events on elements: " << maxEventsElements;
        RAYX_VERB << "\t- num rays: " << sourceConf.numRaysTotal;
        RAYX_VERB << "\t- max batch size: " << maxBatchSize;
        RAYX_VERB << "\t- batch size: " << sourceConf.numRaysBatchAtMost;
        RAYX_VERB << "\t- num batches: " << sourceConf.numBatches;
        // TODO: print object mask
        RAYX_VERB << "\t- using ray attribute mask: " << to_string(attrRecordMask);
        RAYX_VERB << "\t- backend tag: " << AccTag{}.get_name();
        RAYX_VERB << "\t- device index: " << m_deviceIndex;
        RAYX_VERB << "\t- device name: " << alpaka::getName(devAcc);
        RAYX_VERB << "\t- host device name: " << alpaka::getName(devHost);

        const auto numRaysBatchAtMostAccountForGridStride   = nextMultiple(sourceConf.numRaysBatchAtMost, GRID_STRIDE_MULTIPLE);
        const auto numEventsBatchAtMostAccountForGridStride = numRaysBatchAtMostAccountForGridStride * maxEvents;
        auto h_compactEventsBatches                         = std::vector<Rays>(sourceConf.numBatches);
        auto h_eventStoreFlags                              = std::make_unique<bool[]>(numEventsBatchAtMostAccountForGridStride);
        auto h_eventStoreFlagsPrefixSum                     = std::vector<int>(numEventsBatchAtMostAccountForGridStride);
        auto numEventsTotal                                 = 0;

        for (int batchIndex = 0; batchIndex < sourceConf.numBatches; ++batchIndex) {
            RAYX_VERB << "processing batch (" << (batchIndex + 1) << "/" << sourceConf.numBatches << ")";

            // generate input rays for batch
            auto batchConf = m_genRaysResources.genRaysBatch(devAcc, q, batchIndex);

            const auto numRaysBatchAccountForGridStride   = nextMultiple(batchConf.numRaysBatch, GRID_STRIDE_MULTIPLE);
            const auto numEventsBatchAccountForGridStride = numRaysBatchAccountForGridStride * maxEvents;

            // clear buffers
            alpaka::memset(q, *m_resources.d_eventStoreFlags, 0, numEventsBatchAccountForGridStride);

            // from here we need to account for grid stride in the output buffers of the trace function: uncompacte events and storedFlag

            // trace current batch
            traceBatch(devAcc, q, beamlineConf.numSources, beamlineConf.numElements, maxEvents, sequential, attrRecordMask, batchConf,
                       numRaysBatchAccountForGridStride);

            alpaka::memcpy(q, alpaka::createView(devHost, h_eventStoreFlags.get(), numEventsBatchAccountForGridStride),
                           *m_resources.d_eventStoreFlags, numEventsBatchAccountForGridStride);
            const auto h_eventStoreFlagsPrefixSumEnd = std::exclusive_scan(
                h_eventStoreFlags.get(), h_eventStoreFlags.get() + numEventsBatchAccountForGridStride, h_eventStoreFlagsPrefixSum.begin(), 0);
            const auto numEventsBatch =
                *(h_eventStoreFlagsPrefixSumEnd - 1);  // access the last element of the exclusive scan result to get the total count
            alpaka::memcpy(q, *m_resources.d_eventStoreFlagsPrefixSum,
                           alpaka::createView(devHost, h_eventStoreFlagsPrefixSum, numEventsBatchAccountForGridStride),
                           numEventsBatchAccountForGridStride);

            // TODO: here we could apply more filters by turning off storedFlags

            // compact events to remove unused events
            compactEvents(devAcc, q, numEventsBatchAccountForGridStride, attrRecordMask);

            // end of acocunt for grid stride, because from here we use the compacted buffers

            numEventsTotal += numEventsBatch;

            h_compactEventsBatches[batchIndex] = transferEventsBatch(devHost, q, numEventsBatch, attrRecordMask);

            RAYX_VERB << "finished batch (" << (batchIndex + 1) << "/" << sourceConf.numBatches << ") with batch size = " << batchConf.numRaysBatch
                      << ", recorded " << numEventsBatch << " events";
        }

        RAYX_VERB << "number of recorded events: " << numEventsTotal;

        return Rays::concat(h_compactEventsBatches);
    }

  private:
    template <typename DevAcc, typename Queue>
    void traceBatch(DevAcc devAcc, Queue q, int numSources, int numElements, int maxEvents, Sequential sequential, RayAttrMask attrRecordMask,
                    GenRaysAcc::BatchConfig& batchConf, int numRaysBatchAccountForGridStride) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto constState = ConstState{
            // constants
            .maxEvents              = maxEvents,
            .sequential             = sequential,
            .numSources             = numSources,
            .numElements            = numElements,
            .outputEventsGridStride = numRaysBatchAccountForGridStride,

            // buffers
            .objectTransforms = alpaka::getPtrNative(*m_resources.d_objectTransforms),
            .elements         = alpaka::getPtrNative(*m_resources.d_elements),
            .materialIndices  = alpaka::getPtrNative(*m_resources.d_materialIndices),
            .materialTable    = alpaka::getPtrNative(*m_resources.d_materialTable),
            .objectRecordMask = alpaka::getPtrNative(*m_resources.d_objectRecordMask),
            .attrRecordMask   = attrRecordMask,
            .rays             = raysBufToRaysPtr(batchConf.d_rays),
        };

        const auto mutableState = MutableState{
            // buffers
            .events      = raysBufToRaysPtr(m_resources.d_eventsBatch),
            .storedFlags = alpaka::getPtrNative(*m_resources.d_eventStoreFlags),
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
    void compactEvents(DevAcc devAcc, Queue q, const int numEventsBatchAccountForGridStride, const RayAttrMask attrRecordMask) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        // TODO: compare performance to single scatter kernel execution handling all attributes

        auto execKernel = [&]<typename TOptBuf>(TOptBuf& compactAttrBuf, const TOptBuf& attrBuf) {
            execWithValidWorkDiv<Acc>(devAcc, q, numEventsBatchAccountForGridStride, BlockSizeConstraint::None{}, ScatterCompactKernel{},
                                      alpaka::getPtrNative(*compactAttrBuf), alpaka::getPtrNative(*attrBuf),
                                      alpaka::getPtrNative(*m_resources.d_eventStoreFlagsPrefixSum),
                                      alpaka::getPtrNative(*m_resources.d_eventStoreFlags), numEventsBatchAccountForGridStride);
        };

#define X(type, name, flag)                                                                  \
    if (contains(attrRecordMask, RayAttrMask::flag)) {                                       \
        RAYX_VERB << "execute ScatterCompactKernel for compaction of ray attribute: " #name; \
        execKernel(m_resources.d_compactEventsBatch.name, m_resources.d_eventsBatch.name);   \
    }

        RAYX_X_MACRO_RAY_ATTR
#undef X
    }

    template <typename DevHost, typename Queue>
    Rays transferEventsBatch(DevHost& devHost, Queue q, const int numEventsBatch, const RayAttrMask attrRecordMask) {
        const auto transfer = [&]<typename T>(std::vector<T>& dst, const OptBuf<Acc, T>& d_compactEventsBatch) {
            // resize to fit source events and element events
            dst.resize(numEventsBatch);

            // transfer
            alpaka::memcpy(q, alpaka::createView(devHost, dst, numEventsBatch), *d_compactEventsBatch, numEventsBatch);
        };

        Rays h_compactEventsBatch;

#define X(type, name, flag) \
    if (contains(attrRecordMask, RayAttrMask::flag)) transfer(h_compactEventsBatch.name, m_resources.d_compactEventsBatch.name);

        RAYX_X_MACRO_RAY_ATTR
#undef X

        return h_compactEventsBatch;
    }
};

}  // namespace rayx
