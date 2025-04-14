#pragma once

#include "Accelerator.h"
#include "Beamline/Beamline.h"
#include "Material/Material.h"
#include "RAY-Core.h"
#include "Random.h"
#include "Trace.h"
#include "Shader/Ray.h"
#include "Shader/Collision.h"
#include "Element/Element.h"
#include "Shader/WaveFrontLogic.h"
#include "Shader/WaveFrontRayCast.h"
#include "Shader/WaveFrontBehave.h"
#include "Shader/WaveFrontHistogram.h"
#include "Shader/WaveFrontOutputEventsGather.h"
#include "Debug/Debug.h"

namespace {
    constexpr int MAX_BATCH_SIZE = 1 << 20; // TODO: parametrise functions that alloc device memory, for cases where we have smaller batches, to reduce allocation overhead
}

namespace RAYX {

// resources per beamline. required for tracing. constant per trace
template <typename Acc>
struct BeamlineResources {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;

    alpaka::Buf<Acc, OpticalElement, Dim, Idx> d_elements;
    alpaka::Buf<Acc, int, Dim, Idx> d_materialIndices;
    alpaka::Buf<Acc, double, Dim, Idx> d_materialTable;
};

// wave front resources. required for tracing
template <typename Acc>
struct WaveFrontResources {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;

    // ray cast queue
    alpaka::Buf<Acc, int, Dim, Idx> d_rayCastQueueSize;
    alpaka::Buf<Acc, int, Dim, Idx> d_rayCastQueueWaveIndices;

    // behave queues
    // TODO: store them into one array. reduces boilerplate code and potentially improves performance
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveSlitQueueSize;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveSlitQueueWaveIndices;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveRZPQueueSize;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveRZPQueueWaveIndices;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveGratingQueueSize;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveGratingQueueWaveIndices;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveMirrorQueueSize;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveMirrorQueueWaveIndices;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveImagePlaneQueueSize;
    alpaka::Buf<Acc, int, Dim, Idx> d_behaveImagePlaneQueueWaveIndices;

    // ray data. initially contains input rays. gets updated with new ray data after each bounce
    // TODO: consider double buffering for simultaneous compute & transfer
    alpaka::Buf<Acc, Ray, Dim, Idx> d_rays;

    // random number generator per ray
    alpaka::Buf<Acc, Rand, Dim, Idx> d_rayRands;

    // collision data. one entry for each ray
    alpaka::Buf<Acc, Collision, Dim, Idx> d_collisions;

    alpaka::Buf<alpaka::DevCpu, int, Dim, Idx> h_numActiveRays;
};

// output events per tracing. required if 'events' is enabled in output config
template <typename Acc>
struct OutputEventsResources {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;

    // host side events buffer for memory transfer
    alpaka::Buf<alpaka::DevCpu, Ray, Dim, Idx> h_rays;

    // device side events buffer for compaction
    alpaka::Buf<Acc, Ray, Dim, Idx> d_rays;

    // ray events queue
    alpaka::Buf<Acc, int, Dim, Idx> d_rayEventQueueSize;
    alpaka::Buf<Acc, int, Dim, Idx> d_rayEventQueueWaveIndices;
    alpaka::Buf<alpaka::DevCpu, int, Dim, Idx> h_rayEventQueueSize;
    alpaka::Buf<alpaka::DevCpu, int, Dim, Idx> h_rayEventQueueWaveIndices;

    // TODO: maybe add gather the rays before transfer for performance
};

// output analysis per tracing, required if 'analysis' is enabled in output config
template <typename Acc>
struct OutputAnalysisResources {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;

    alpaka::Buf<Acc, int, Dim, Idx> d_histogram;
    alpaka::Buf<alpaka::DevCpu, int, Dim, Idx> h_histogram;
    const int histogramWidth;
};

template <typename Acc, typename Queue>
BeamlineResources<Acc> createBeamlineResources(Queue q, const Group& group) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    using Vec = alpaka::Vec<Dim, Idx>;

    const auto elements = group.compileElements();
    const auto materialTables = group.calcMinimalMaterialTables();
    const auto& materialIndices = materialTables.indexTable;
    const auto& materialTable = materialTables.materialTable;

    auto d_elements = alpaka::allocAsyncBufIfSupported<OpticalElement, Idx>(q, Vec{elements.size()});
    auto d_materialIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{materialIndices.size()});
    auto d_materialTable = alpaka::allocAsyncBufIfSupported<double, Idx>(q, Vec{materialTable.size()});

    const auto hostPlatform = alpaka::PlatformCpu{};
    const auto host = alpaka::getDevByIdx(hostPlatform, 0);

    alpaka::memcpy(q, d_elements, alpaka::createView(host, elements, Vec{elements.size()}));
    alpaka::memcpy(q, d_materialIndices, alpaka::createView(host, materialIndices, Vec{materialIndices.size()}));
    alpaka::memcpy(q, d_materialTable, alpaka::createView(host, materialTable, Vec{materialTable.size()}));

    return {
        d_elements,
        d_materialIndices,
        d_materialTable,
    };
}

template <typename Acc, typename Queue>
WaveFrontResources<Acc> createWaveFrontResources(Queue q, const Group& group, double randomSeed) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    using Vec = alpaka::Vec<Dim, Idx>;

    auto d_rayCastQueueSize = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{1});
    auto d_rayCastQueueWaveIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{MAX_BATCH_SIZE});

    auto d_behaveSlitQueueSize = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{1});
    auto d_behaveSlitQueueWaveIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{MAX_BATCH_SIZE});
    auto d_behaveRZPQueueSize = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{1});
    auto d_behaveRZPQueueWaveIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{MAX_BATCH_SIZE});
    auto d_behaveGratingQueueSize = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{1});
    auto d_behaveGratingQueueWaveIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{MAX_BATCH_SIZE});
    auto d_behaveMirrorQueueSize = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{1});
    auto d_behaveMirrorQueueWaveIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{MAX_BATCH_SIZE});
    auto d_behaveImagePlaneQueueSize = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{1});
    auto d_behaveImagePlaneQueueWaveIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{MAX_BATCH_SIZE});
    alpaka::memset(q, d_behaveSlitQueueSize, 0);
    alpaka::memset(q, d_behaveRZPQueueSize, 0);
    alpaka::memset(q, d_behaveGratingQueueSize, 0);
    alpaka::memset(q, d_behaveMirrorQueueSize, 0);
    alpaka::memset(q, d_behaveImagePlaneQueueSize, 0);

    auto d_rays = alpaka::allocAsyncBufIfSupported<Ray, Idx>(q, Vec{MAX_BATCH_SIZE});
    auto d_rayRands = alpaka::allocAsyncBufIfSupported<Rand, Idx>(q, Vec{MAX_BATCH_SIZE});
    auto d_collisions = alpaka::allocAsyncBufIfSupported<Collision, Idx>(q, Vec{MAX_BATCH_SIZE});
    alpaka::memset(q, d_collisions, 0);

    const auto hostPlatform = alpaka::PlatformCpu{};
    const auto host = alpaka::getDevByIdx(hostPlatform, 0);
    const auto inputRays = group.compileSources(1); // TODO: generate rays on device
    assert(inputRays.size() < MAX_BATCH_SIZE); // TODO: keep this assert until we generate rays on device
    alpaka::memcpy(q, d_rays, alpaka::createView(host, inputRays, Vec{inputRays.size()}));

    // TODO: find a way to skip this step
    const int rayCastQueueSize = inputRays.size();
    auto rayCastQueueWaveIndices = std::vector<int>(inputRays.size());
    std::iota(rayCastQueueWaveIndices.begin(), rayCastQueueWaveIndices.end(), 0);
    alpaka::memcpy(q, d_rayCastQueueSize, alpaka::createView(host, &rayCastQueueSize, Vec{1}));
    alpaka::memcpy(q, d_rayCastQueueWaveIndices, alpaka::createView(host, rayCastQueueWaveIndices, Vec{inputRays.size()}));

    // TODO: generate rands in newRay step
    auto rayRands = std::vector<Rand>(inputRays.size());
    std::generate(rayRands.begin(), rayRands.end(), [&, i=0] () mutable {
        return Rand(i++, inputRays.size(), randomSeed);
    });
    alpaka::memcpy(q, d_rayRands, alpaka::createView(host, rayRands, Vec{inputRays.size()}));

    const auto accPlatform = alpaka::Platform<Acc>{};
    auto h_numActiveRays = alpaka::allocMappedBufIfSupported<int, Idx>(host, accPlatform, Vec{1});

    return {
        d_rayCastQueueSize,
        d_rayCastQueueWaveIndices,

        d_behaveSlitQueueSize,
        d_behaveSlitQueueWaveIndices,
        d_behaveRZPQueueSize,
        d_behaveRZPQueueWaveIndices,
        d_behaveGratingQueueSize,
        d_behaveGratingQueueWaveIndices,
        d_behaveMirrorQueueSize,
        d_behaveMirrorQueueWaveIndices,
        d_behaveImagePlaneQueueSize,
        d_behaveImagePlaneQueueWaveIndices,

        d_rays,
        d_rayRands,
        d_collisions,

        h_numActiveRays,
    };
}

template <typename Acc, typename Queue>
OutputEventsResources<Acc> createOutputEventsResources(Queue q) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    using Vec = alpaka::Vec<Dim, Idx>;

    const auto hostPlatform = alpaka::PlatformCpu{};
    const auto host = alpaka::getDevByIdx(hostPlatform, 0);
    const auto accPlatform = alpaka::Platform<Acc>{};

    auto h_rays = alpaka::allocMappedBufIfSupported<Ray, Idx>(host, accPlatform, Vec{MAX_BATCH_SIZE});
    auto d_rays = alpaka::allocAsyncBufIfSupported<Ray, Idx>(q, Vec{MAX_BATCH_SIZE});

    auto d_rayEventQueueSize = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{1});
    auto d_rayEventQueueWaveIndices = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{MAX_BATCH_SIZE});
    auto h_rayEventQueueSize = alpaka::allocMappedBufIfSupported<int, Idx>(host, accPlatform, Vec{1});
    auto h_rayEventQueueWaveIndices = alpaka::allocMappedBufIfSupported<int, Idx>(host, accPlatform, Vec{MAX_BATCH_SIZE});
    alpaka::memset(q, d_rayEventQueueSize, 0);

    return {
        h_rays,
        d_rays,
        d_rayEventQueueSize,
        d_rayEventQueueWaveIndices,
        h_rayEventQueueSize,
        h_rayEventQueueWaveIndices,
    };
}

template <typename Acc, typename Queue>
OutputAnalysisResources<Acc> createOutputAnalysisResources(Queue q) {
    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    using Vec = alpaka::Vec<Dim, Idx>;

    const auto hostPlatform = alpaka::PlatformCpu{};
    const auto host = alpaka::getDevByIdx(hostPlatform, 0);
    const auto accPlatform = alpaka::Platform<Acc>{};

    const auto histogramWidth = 16;
    auto d_histogram = alpaka::allocAsyncBufIfSupported<int, Idx>(q, Vec{histogramWidth});
    auto h_histogram = alpaka::allocMappedBufIfSupported<int, Idx>(host, accPlatform, Vec{histogramWidth});

    return {
        d_histogram,
        h_histogram,
        histogramWidth,
    };
}

template <typename Acc>
OutputData waveFrontTrace(const Group& group, const int deviceIndex, const OutputConfig outputConfig) {
    if (!outputConfig.events && !outputConfig.analysis)
        return {};

    auto outputData = OutputData{};

    using Dim = alpaka::Dim<Acc>;
    using Idx = alpaka::Idx<Acc>;
    using Vec = alpaka::Vec<Dim, Idx>;

    const auto hostPlatform = alpaka::PlatformCpu{};
    const auto host = alpaka::getDevByIdx(hostPlatform, 0);
    const auto accPlatform = alpaka::Platform<Acc>{};
    const auto acc = getDevByIdx(accPlatform, deviceIndex);

    using Queue = alpaka::Queue<Acc, alpaka::NonBlocking>;
    auto q_trace = Queue(acc);
    auto q_outputEventsGather = Queue(acc);
    auto q_outputEventsTransferLaunch = Queue(acc);
    auto q_outputEventsTransfer = Queue(acc);
    auto q_outputEventsCollectLaunch = Queue(acc);
    auto q_outputEventsCollect = Queue(acc);

    using Event = alpaka::Event<Queue>;
    auto e_numActiveRays = alpaka::Event<Queue>(acc);
    auto e_outputEventsGather = std::vector<Event>();
    auto e_outputEventsTransferLaunch = std::vector<Event>();
    auto e_outputEventsTransfer = std::vector<Event>();
    auto e_outputEventsCollect = std::vector<Event>();
    std::generate_n(std::back_inserter(e_outputEventsGather), outputConfig.maxEvents, [&] { return Event(acc); });
    std::generate_n(std::back_inserter(e_outputEventsTransfer), outputConfig.maxEvents, [&] { return Event(acc); });
    std::generate_n(std::back_inserter(e_outputEventsTransferLaunch), outputConfig.maxEvents, [&] { return Event(acc); });
    std::generate_n(std::back_inserter(e_outputEventsCollect), outputConfig.maxEvents, [&] { return Event(acc); });
    auto numEvents = std::vector<int>(outputConfig.maxEvents);

    const auto beamlineResources = createBeamlineResources<Acc>(q_trace, group);
    const double randomSeed = randomDouble();
    auto waveFrontResources = createWaveFrontResources<Acc>(q_trace, group, randomSeed);
    auto outputEventsResources = createOutputEventsResources<Acc>(q_trace);
    auto outputAnalysisResources = createOutputAnalysisResources<Acc>(q_trace);

    const int numInputRays = alpaka::getExtents(waveFrontResources.d_rays)[0];
    const int numElements = group.numElements(); // TODO: remove this expensive redundant computation. elements are already compiled at this point

    auto bundleHistory = BundleHistory(numInputRays);
    for (auto& rayHist : bundleHistory)
        rayHist.reserve(2);

    auto numIterations = 0;
    for (int i = 0; i < outputConfig.maxEvents; ++i) {
        ++numIterations;

        alpaka::exec<Acc>(
            q_trace,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontRayCastKernel{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            numElements,
            alpaka::getPtrNative(waveFrontResources.d_rays),
            alpaka::getPtrNative(waveFrontResources.d_rayRands),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_collisions)
        );
        alpaka::memset(q_trace, waveFrontResources.d_rayCastQueueSize, 0);

        if (i != 0)
            alpaka::wait(q_trace, e_outputEventsGather[i-1]);

        alpaka::exec<Acc>(
            q_trace,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontLogicKernel{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            alpaka::getPtrNative(waveFrontResources.d_rays),
            numInputRays,
            alpaka::getPtrNative(waveFrontResources.d_collisions),

            alpaka::getPtrNative(waveFrontResources.d_behaveSlitQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveSlitQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_behaveRZPQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveRZPQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_behaveGratingQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveGratingQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_behaveMirrorQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveMirrorQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_behaveImagePlaneQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveImagePlaneQueueWaveIndices),
            alpaka::getPtrNative(outputEventsResources.d_rayEventQueueSize),
            alpaka::getPtrNative(outputEventsResources.d_rayEventQueueWaveIndices)
        );

        alpaka::exec<Acc>(
            q_trace,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontBehaveKernel<BehaveTypeSlit>{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            alpaka::getPtrNative(beamlineResources.d_materialIndices),
            alpaka::getPtrNative(beamlineResources.d_materialTable),
            alpaka::getPtrNative(waveFrontResources.d_collisions),
            alpaka::getPtrNative(waveFrontResources.d_behaveSlitQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveSlitQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_rays),
            alpaka::getPtrNative(waveFrontResources.d_rayRands),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueWaveIndices)
        );
        alpaka::memset(q_trace, waveFrontResources.d_behaveSlitQueueSize, 0);

        alpaka::exec<Acc>(
            q_trace,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontBehaveKernel<BehaveTypeRZP>{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            alpaka::getPtrNative(beamlineResources.d_materialIndices),
            alpaka::getPtrNative(beamlineResources.d_materialTable),
            alpaka::getPtrNative(waveFrontResources.d_collisions),
            alpaka::getPtrNative(waveFrontResources.d_behaveRZPQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveRZPQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_rays),
            alpaka::getPtrNative(waveFrontResources.d_rayRands),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueWaveIndices)
        );
        alpaka::memset(q_trace, waveFrontResources.d_behaveRZPQueueSize, 0);

        alpaka::exec<Acc>(
            q_trace,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontBehaveKernel<BehaveTypeGrating>{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            alpaka::getPtrNative(beamlineResources.d_materialIndices),
            alpaka::getPtrNative(beamlineResources.d_materialTable),
            alpaka::getPtrNative(waveFrontResources.d_collisions),
            alpaka::getPtrNative(waveFrontResources.d_behaveGratingQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveGratingQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_rays),
            alpaka::getPtrNative(waveFrontResources.d_rayRands),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueWaveIndices)
        );
        alpaka::memset(q_trace, waveFrontResources.d_behaveGratingQueueSize, 0);

        alpaka::exec<Acc>(
            q_trace,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontBehaveKernel<BehaveTypeMirror>{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            alpaka::getPtrNative(beamlineResources.d_materialIndices),
            alpaka::getPtrNative(beamlineResources.d_materialTable),
            alpaka::getPtrNative(waveFrontResources.d_collisions),
            alpaka::getPtrNative(waveFrontResources.d_behaveMirrorQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveMirrorQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_rays),
            alpaka::getPtrNative(waveFrontResources.d_rayRands),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueWaveIndices)
        );
        alpaka::memset(q_trace, waveFrontResources.d_behaveMirrorQueueSize, 0);

        alpaka::exec<Acc>(
            q_trace,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontBehaveKernel<BehaveTypeImagePlane>{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            alpaka::getPtrNative(beamlineResources.d_materialIndices),
            alpaka::getPtrNative(beamlineResources.d_materialTable),
            alpaka::getPtrNative(waveFrontResources.d_collisions),
            alpaka::getPtrNative(waveFrontResources.d_behaveImagePlaneQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_behaveImagePlaneQueueWaveIndices),
            alpaka::getPtrNative(waveFrontResources.d_rays),
            alpaka::getPtrNative(waveFrontResources.d_rayRands),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueSize),
            alpaka::getPtrNative(waveFrontResources.d_rayCastQueueWaveIndices)
        );
        alpaka::memset(q_trace, waveFrontResources.d_behaveImagePlaneQueueSize, 0);

        alpaka::memcpy(q_trace, waveFrontResources.h_numActiveRays, waveFrontResources.d_rayCastQueueSize);
        alpaka::enqueue(q_trace, e_numActiveRays);

        alpaka::wait(q_outputEventsGather, e_numActiveRays);
        if (i != 0) alpaka::wait(q_outputEventsGather, e_outputEventsTransferLaunch[i-1]);
        if (i != 0) alpaka::wait(q_outputEventsGather, e_outputEventsTransfer[i-1]);
        alpaka::exec<Acc>(
            q_outputEventsGather,
            getWorkDivForAcc<Acc>(numInputRays),
            WaveFrontOutputEventsGatherKernel{},

            alpaka::getPtrNative(beamlineResources.d_elements),
            alpaka::getPtrNative(waveFrontResources.d_rays),
            alpaka::getPtrNative(outputEventsResources.d_rayEventQueueSize),
            alpaka::getPtrNative(outputEventsResources.d_rayEventQueueWaveIndices),
            alpaka::getPtrNative(outputEventsResources.d_rays)
        );
        alpaka::memcpy(q_outputEventsGather, outputEventsResources.h_rayEventQueueSize, outputEventsResources.d_rayEventQueueSize, Vec{1});
        alpaka::memset(q_outputEventsGather, outputEventsResources.d_rayEventQueueSize, 0);
        alpaka::enqueue(q_outputEventsGather, e_outputEventsGather[i]);

        alpaka::wait(q_outputEventsTransferLaunch, e_outputEventsGather[i]);
        if (i != 0) alpaka::wait(q_outputEventsTransferLaunch, e_outputEventsCollect[i-1]);
        alpaka::enqueue(q_outputEventsTransferLaunch, [&, i] () mutable {
            numEvents[i] = outputEventsResources.h_rayEventQueueSize[0];
            RAYX_D_LOG << "[iteration: " << i << "]: traced events: " << numEvents[i];

            alpaka::memcpy(q_outputEventsTransfer, outputEventsResources.h_rayEventQueueWaveIndices, outputEventsResources.d_rayEventQueueWaveIndices, Vec{numEvents[i]});
            alpaka::memcpy(q_outputEventsTransfer, outputEventsResources.h_rays, outputEventsResources.d_rays, Vec{numEvents[i]});
            alpaka::enqueue(q_outputEventsTransfer, e_outputEventsTransfer[i]);
        });
        alpaka::enqueue(q_outputEventsTransferLaunch, e_outputEventsTransferLaunch[i]);

        alpaka::wait(q_outputEventsCollect, e_outputEventsTransferLaunch[i]);
        alpaka::wait(q_outputEventsCollect, e_outputEventsTransfer[i]);
        alpaka::enqueue(q_outputEventsCollect, [&, i] () mutable {
            // TODO: bad performance due to millions of allocations
            for (int eventIndex = 0; eventIndex < numEvents[i]; ++eventIndex) {
                const auto waveIndex = outputEventsResources.h_rayEventQueueWaveIndices[eventIndex];
                const auto& ray = outputEventsResources.h_rays[waveIndex];
                // const auto globalIndex = ray.gobalIndex;
                auto& rayEvents = bundleHistory[waveIndex]; // TODO: use global ray index here
                rayEvents.push_back(ray);
            }
        });
        alpaka::enqueue(q_outputEventsCollect, e_outputEventsCollect[i]);

        alpaka::wait(e_numActiveRays);
        const auto numActiveRays = *alpaka::getPtrNative(waveFrontResources.h_numActiveRays);
        RAYX_D_LOG << "[iteration: " << i << "]: active rays for next iteration: " << numActiveRays;
        if (numActiveRays == 0)
            break;
        // TODO: this loop numIterations and maxEvents do not relate to the same thing. they need to be handled indvidually
        if (i == outputConfig.maxEvents - 1)
            RAYX_WARN << "too many events! consider increasing 'max events' setting (currently 'max events' is " << outputConfig.maxEvents <<")"; // TODO: better error handling
    }

    // TODO: properly wait for all queued tasks and all takss that are yet to be queued.
    for (int i = 0; i < numIterations; ++i) {
        alpaka::wait(e_outputEventsCollect[i]);
    }
    alpaka::wait(acc);

    RAYX_D_LOG << "done tracing! num iterations: " << numIterations;

    outputData.events = std::move(bundleHistory);
    return outputData;
}

}
