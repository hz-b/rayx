#include "SimpleTracer.h"

#include <cmath>
#include <cstring>

#include <alpaka/alpaka.hpp>
#include <alpaka/example/ExampleDefaultAcc.hpp>

#include "Beamline/OpticalElement.h"
#include "Material/Material.h"
#include "RAY-Core.h"

#include "Shader/DynamicElements.h"
#include "Shader/InvocationState.h"

#include "AlpakaUtil.h"
#include "Random.h"

#include "Beamline/OpticalElement.h"
#include "RAY-Core.h"
#include "Shader/Constants.h"

namespace {

struct ShaderEntryPoint {
    template <typename TAcc>
    RAYX_FUNC
    void operator() (const TAcc& acc, Inv inv) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < inv.rayData.size())
            dynamicElements(gid, inv);
    }
};

template <typename T>
inline T scan_sum(std::vector<T>& dst, const std::vector<T>& src) {
    RAYX_PROFILE_SCOPE_STDOUT("scan");
    assert(dst.size() == src.size());
    T sum = 0;
    for (int i = 0; i < (int)dst.size(); ++i) {
        dst[i] = sum;
        sum += src[i];
        // printf("scan %d: %d %d, sum: %d\n", i, src[i], dst[i], sum);
    }
    return sum;
}

// TODO: could store as uint32_t bitmap for 32 elements, since output is bool for each event
struct FilterRayEventsKernel {
    template <typename TAcc>
    RAYX_FUNC
    void operator() (
        const TAcc& acc,
        std::span<int> eventPass,
        std::span<const int> numEvents,
        int maxEvents
    ) const {
        const auto rayIdx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        for (int rayEventIdx = 0; rayEventIdx < maxEvents; ++rayEventIdx) {
            int globalEventIdx = rayIdx * maxEvents + rayEventIdx;

            // let recorded events pass. filter out empty slots
            auto pass = rayEventIdx < numEvents[rayIdx];
            // printf("... %d %d, %d %d, %d\n", (int)rayIdx, (int)globalEventIdx, (int)(rayEventIdx), (int)numEvents[rayIdx], (int)pass);
            eventPass[globalEventIdx] = pass ? 1 : 0;
        }
    }
};

struct PackGlobalEventsKernel {
    template <typename TAcc>
    RAYX_FUNC
    void operator() (
        const TAcc& acc,
        std::span<Ray> dst,
        std::span<const Ray> src,
        std::span<int> eventPass,
        std::span<int> globalEventIndex
    ) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        // printf("pack gid(%d) eventPass(%d) globalEventIndex(%d)\n", gid, eventPass[gid], globalEventIndex[gid]);
        if (eventPass[gid])
            dst[globalEventIndex[gid]] = src[gid];
    }
};

template <typename Acc>
inline SimpleTracer::TraceResult traceBatch(const TraceRawConfig& cfg, const PushConstants& pushConstants) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;

    using Cpu = alpaka::DevCpu;
    const auto [cpu_platform, cpu] = pickFirstDevice<Cpu>();
    const auto [d_platform, acc] = pickFirstDevice<Acc>();

    using QueueProperty = alpaka::NonBlocking;
    using AccQueue = alpaka::Queue<Acc, QueueProperty>;
    auto queue = AccQueue(acc);

    const auto numInputRays = cfg.m_rays.size();
    // printf("numInputRays: %d\n", (int)numInputRays);
    const auto numOutputRays = numInputRays * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID);
    const auto& materialTables = cfg.m_materialTables;

    using Vec = alpaka::Vec<Dim, Idx>;

    auto rayData = createBuffer<Idx, Vec, Ray>(queue, cfg.m_rays, cpu);
    auto outputRays = createBuffer<Idx, Ray>(queue, Vec{numOutputRays});
    auto outputRayCounts = createBuffer<Idx, int>(queue, Vec{numInputRays});
    auto elements = createBuffer<Idx, Vec, Element>(queue, cfg.m_elements, cpu);
    auto matIdx = createBuffer<Idx, Vec, int>(queue, materialTables.indexTable, cpu);
    auto mat = createBuffer<Idx, Vec, double>(queue, materialTables.materialTable, cpu);

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
        .pushConstants = pushConstants,
    };

    auto workDiv = getWorkDivForAcc<Acc>(numInputRays);
    alpaka::exec<Acc>(
        queue,
        workDiv,
        ShaderEntryPoint{},
        inv
    );

    // auto h_outputRays = std::vector<Ray>(numOutputRays);
    // auto h_outputRaysView = alpaka::createView(cpu, h_outputRays);
    // alpaka::memcpy(queue, h_outputRaysView, outputRays, Vec{numOutputRays});
    // alpaka::wait(queue);

    // for (int i = 0; i < numOutputRays; ++i) {
    //     bool e = h_outputRays[i].m_eventType != ETYPE_UNINIT;
    //     printf("%d", (int)e);
    // }
    // printf("\n");



    auto h_outputRayCounts = std::vector<int>(numInputRays);
    auto h_outputRayCountsView = alpaka::createView(cpu, h_outputRayCounts);
    alpaka::wait(queue);
    alpaka::memcpy(queue, h_outputRayCountsView, outputRayCounts, Vec{numInputRays});
    alpaka::wait(queue);

    // alpaka::wait(queue);
    // for (int i = 0; i < numInputRays; ++i) {
    //     int e = h_outputRayCounts[i];
    //     if (e < 0)
    //         printf("x %d %d\n", i, e);
    // }
    // printf("\n");



    auto globalEventPass = createBuffer<Idx, int>(queue, Vec{numOutputRays});
    alpaka::exec<Acc>(
        queue,
        workDiv,
        FilterRayEventsKernel{},
        bufToSpan(globalEventPass),
        bufToSpan(outputRayCounts),
        cfg.m_maxEvents
    );

    auto h_globalEventPass = std::vector<int>(numOutputRays);
    auto h_globalEventPassView = alpaka::createView(cpu, h_globalEventPass);
    alpaka::memcpy(queue, h_globalEventPassView, globalEventPass, Vec{numOutputRays});
    alpaka::wait(queue);

    // alpaka::wait(queue);
    // for (int i = 0; i < numOutputRays; ++i) {
    //     bool e = h_globalEventPass[i];
    //     printf("%d", (int)e);
    // }
    // printf("\n");



    auto h_globalEventIndices = std::vector<int>(numOutputRays);
    auto h_globalEventIndicesView = alpaka::createView(cpu, h_globalEventIndices);
    int globalEventsCount = scan_sum(h_globalEventIndices, h_globalEventPass);
    auto packedGlobalEvents = createBuffer<Idx, Ray>(queue, Vec{globalEventsCount});

    // for (int i = 0; i < numOutputRays; ++i) {
    //     int e = h_globalEventIndices[i];
    //     printf("%d ", (int)e);
    // }
    // printf("\n");

    // printf("numOutputRays: %d, packed: %d, ratio: %f\n",
    //     (int)numOutputRays,
    //     (int)globalEventsCount,
    //     (globalEventsCount / static_cast<float>(numOutputRays))
    // );

    auto globalEventIndices = createBuffer<Idx, int>(queue, Vec{numOutputRays});
    alpaka::memcpy(queue, globalEventIndices, h_globalEventIndicesView, Vec{numOutputRays});
    alpaka::wait(queue);

    alpaka::exec<Acc>(
        queue,
        getWorkDivForAcc<Acc>(numOutputRays),
        PackGlobalEventsKernel{},
        bufToSpan(packedGlobalEvents),
        bufToSpan(outputRays),
        bufToSpan(globalEventPass),
        bufToSpan(globalEventIndices)
    );
    auto h_packedGlobalEvents = std::vector<Ray>(globalEventsCount);
    auto h_packedGlobalEventsView = alpaka::createView(cpu, h_packedGlobalEvents);
    alpaka::memcpy(queue, h_packedGlobalEventsView, packedGlobalEvents, Vec{globalEventsCount});
    alpaka::wait(queue);

    // alpaka::wait(queue);
    // for (int i = 0, j = 0; i < numOutputRays; ++i) {
    //     if (h_outputRays[i].m_eventType != ETYPE_UNINIT) {
    //         printf(".");
    //         assert(i < numOutputRays);
    //         assert(j < globalEventsCount);
    //         auto eq = h_outputRays[i].m_eventType == h_packedGlobalEvents[j].m_eventType;
    //         if (!eq)
    //             printf("%d %d, %d %d\n", i, j, (int)h_outputRays[i].m_eventType, (int)h_packedGlobalEvents[j].m_eventType);
    //
    //         ++j;
    //     }
    // }

    auto h_packedGlobalEventOffsets = std::vector<int>(numInputRays);
    scan_sum(h_packedGlobalEventOffsets, h_outputRayCounts);

    return SimpleTracer::TraceResult {
        std::move(h_packedGlobalEvents),
        std::move(h_packedGlobalEventOffsets),
        std::move(h_outputRayCounts),
        globalEventsCount,
    };
}

} // unnamed namespace

namespace RAYX {

SimpleTracer::SimpleTracer(Platform platform) {
    RAYX_VERB << "Initializing Tracer..";

    using Dim = alpaka::DimInt<1>;
    using Idx = int;

    switch (platform) {
        case Platform::Gpu: {
            using Acc = DefaultGpuAcc<Dim, Idx>;

            if constexpr (isAccAvailable<Acc>()) {
                m_traceBatchOnPlatformFn = &traceBatch<Acc>;
                break;
            } else {
                RAYX_WARN
                    << "Gpu Tracer was disabled during build."
                    << "Falling back to Cpu Tracer."
                    << "Add '-x' flag on launch to use the Cpu Tracer directly"
                ;
                [[fallthrough]];
            }
        }
        case Platform::Cpu: {
            using Acc = DefaultCpuAcc<Dim, Idx>;
            m_traceBatchOnPlatformFn = &traceBatch<Acc>;
        }
    }
}

SimpleTracer::~SimpleTracer() = default;

BundleHistory SimpleTracer::trace(const Beamline& b, Sequential seq, uint64_t max_batch_size, int thread_count, unsigned int maxEvents, int startEventID) {
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
            traceResult = m_traceBatchOnPlatformFn(cfg, m_pushConstants);
            RAYX_LOG << "Traced " << traceResult.count << " events.";
            // assert(rawBatch.size() == batch_size * (maxEvents - startEventID));
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

void SimpleTracer::setPushConstants(const PushConstants* p) {
    std::memcpy(&m_pushConstants, p, sizeof(PushConstants));
}

}  // namespace RAYX
