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

        if (gid < inv.pushConstants.numRays)
            dynamicElements(gid, inv);
    }
};

template <typename Acc>
std::vector<Ray> traceBatch(const TraceRawConfig& cfg, const PushConstants& pushConstants) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;

    using Cpu = alpaka::DevCpu;
    const auto [cpu_platform, cpu] = pickFirstDevice<Cpu>();
    const auto [d_platform, acc] = pickFirstDevice<Acc>();

    using QueueProperty = alpaka::NonBlocking;
    using AccQueue = alpaka::Queue<Acc, QueueProperty>;
    auto queue = AccQueue(acc);

    const auto numInputRays = cfg.m_rays.size();
    const auto numOutputRays = numInputRays * ((size_t)cfg.m_maxEvents - (size_t)cfg.m_startEventID);
    const auto& materialTables = cfg.m_materialTables;

    using Vec = alpaka::Vec<Dim, Idx>;

    auto rayData = createBuffer<Idx, Vec, Ray>(queue, cfg.m_rays, cpu);
    auto outputRays = createBuffer<Idx, Ray>(queue, Vec{numOutputRays});
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

    auto output = std::vector<Ray>(numOutputRays);
    auto outputView = alpaka::createView(cpu, output);
    alpaka::memcpy(queue, outputView, outputRays, Vec{numOutputRays});

    alpaka::wait(queue);
    return output;
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
        std::vector<Ray> rawBatch;
        {
            RAYX_PROFILE_SCOPE_STDOUT("Tracing");
            rawBatch = m_traceBatchOnPlatformFn(cfg, m_pushConstants);
            RAYX_LOG << "Traced " << rawBatch.size() << " events.";
            assert(rawBatch.size() == batch_size * (maxEvents - startEventID));
        }

        // put all events from the rawBatch to unified `BundleHistory result`.
        {
            RAYX_PROFILE_SCOPE_STDOUT("BundleHistory-calculation");
            for (uint i = 0; i < batch_size; i++) {
                // We now create the Rayhistory for the `i`th ray of the batch:
                RayHistory hist;
                hist.reserve(maxEvents - startEventID);

                // iterate through the event-indices that were allocated for this particular ray.
                for (uint j = 0; j < maxEvents - startEventID; j++) {
                    // each ray has space for `maxEvents - startEventID` many events.
                    // Hence the first event for ray i is always at index `i * (maxEvents - startEventID)`.
                    uint idx = i * (maxEvents - startEventID) + j;
                    Ray r = rawBatch[idx];

                    // we discard UNINIT events.
                    // These events only come up, if the ray had less than `maxEvents`-many events.
                    if (r.m_eventType == ETYPE_UNINIT) {
                        // further we know that after one UNINIT event, there will only come further UNINIT events.
                        // Hence we are done for this ray, and will look for the next one.
                        break;
                    } else {
                        hist.push_back(r);
                    }
                }

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
