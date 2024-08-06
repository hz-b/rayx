#include "Scheduler.h"

#include "Platform.h"
#include "SimpleTracer.h"

namespace {

using DeviceType = RAYX::DeviceConfig::DeviceType;
using DeviceIndex = RAYX::DeviceConfig::Device::Index;

std::shared_ptr<RAYX::DeviceTracer> createDeviceTracer(DeviceType deviceType, DeviceIndex deviceIndex) {
    using Dim = alpaka::DimInt<1>;
    using Idx = int32_t;

    switch (deviceType) {
        case DeviceType::GpuCuda:
#if defined(RAYX_CUDA)
            using GpuAccCuda = RAYX::GpuAccCuda<Dim, Idx>;
            return std::make_shared<RAYX::SimpleTracer<GpuAccCuda>>(deviceIndex);
#else
            RAYX_ERR << "Failed to create Tracer with Cuda device. Cuda was disabled during build.";
            return nullptr;
#endif
        case DeviceType::GpuHip:
#if defined(RAYX_HIP)
            using GpuAccHip = RAYX::GpuAccHip<Dim, Idx>;
            return std::make_shared<RAYX::SimpleTracer<GpuAccHip>>(deviceIndex);
#else
            RAYX_ERR << "Failed to create Tracer with Hip device. Hip was disabled during build.";
            return nullptr;
#endif
        default:  // case DeviceType::Cpu
            using CpuAcc = RAYX::DefaultCpuAcc<Dim, Idx>;
            return std::make_shared<RAYX::SimpleTracer<CpuAcc>>(deviceIndex);
    }
}

RAYX::EventList batchOutputToBatch(RAYX::DeviceTracer::BatchOutput&& batchOutput) {
    auto rays = std::vector<std::span<RAYX::Ray>>();
    rays.reserve(batchOutput.eventOffsets.size());

    for (size_t i = 0; i < batchOutput.eventOffsets.size(); ++i) {
        const auto data = batchOutput.events.data();
        const auto offset = batchOutput.eventOffsets[i];
        const auto size = batchOutput.eventCounts[i];
        const auto ray = std::span<RAYX::Ray>(data + offset, size);
        rays.emplace_back(ray);
    }

    return RAYX::EventList {
        .events = std::move(batchOutput.events),
        .rays = std::move(rays),
    };
}

}  // unnamed namespace

namespace RAYX {

Scheduler::Scheduler(const DeviceConfig& deviceConfig) : m_deviceTracerShouldStop(false) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    const int numDeviceTracers = deviceConfig.enabledDevicesCount();
    if (numDeviceTracers == 0) RAYX_ERR << "The number of selected devices must be at least 1";

    m_deviceTracerFutures = std::vector<std::future<void>>(numDeviceTracers);

    int deviceTracerIndex = 0;
    for (const auto& device : deviceConfig.devices) {
        if (device.enable) {
            RAYX_LOG << "Creating tracer with device: " << device.name;
            auto deviceTracer = createDeviceTracer(device.type, device.index);

            auto runDeviceTracer = [this, deviceTracerIndex, deviceTracer] {
                while (!m_deviceTracerShouldStop) {
                    using namespace std::chrono_literals;
                    if (auto batchJob = m_batchJobQueue.pop_wait(); batchJob) {
                        auto batchOutput = deviceTracer->traceBatch(batchJob->beamlineInput, batchJob->batchInput);
                        auto batch = batchOutputToBatch(std::move(batchOutput));
                        batchJob->batchPromise.set_value(std::move(batch));
                    }
                }
            };

            m_deviceTracerFutures[deviceTracerIndex] = std::async(std::launch::async, runDeviceTracer);
            ++deviceTracerIndex;
        }
    }
}

Scheduler::~Scheduler() {
    m_deviceTracerShouldStop = true;
    m_batchJobQueue.releaseWaiting();
    for (auto& deviceTracerFuture : m_deviceTracerFutures) deviceTracerFuture.wait();
}

Scheduler::TraceResult Scheduler::trace(const Beamline& beamline) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // don't trace if there are no optical elements
    if (beamline.m_DesignElements.size() == 0) {
        RAYX_WARN << "There are no optical elements in the traced Beamline";
        return {};
    }

    // prepare input data
    auto extractElements = [&beamline] {
        std::vector<Element> elements;
        elements.reserve(beamline.m_DesignElements.size());
        for (const auto& e : beamline.m_DesignElements) elements.push_back(e.compile());
        return elements;
    };
    const auto elements = extractElements();
    const auto rays = beamline.getInputRays(1);  // TODO: Thread count, or move ray generation to DeviceTracer
    const auto materialTables = beamline.calcMinimalMaterialTables();
    const auto randomSeed = randomDouble();

    const auto numRays = rays.size();
    const auto numBatches = (static_cast<int>(rays.size()) + 1) / DEFAULT_BATCH_SIZE;

    const auto beamlineInput = std::make_shared<DeviceTracer::BeamlineInput>(DeviceTracer::BeamlineInput{
        .elements = std::move(elements),
        .rays = std::move(rays),
        .materialTables = std::move(materialTables),
        .randomSeed = randomSeed,
    });

    auto batchFutures = TraceResult();
    batchFutures.reserve(numBatches);

    for (int i = 0; i < static_cast<int>(rays.size()); i += DEFAULT_BATCH_SIZE) {  // TODO: --batch (batch size)
        const auto batchInput = DeviceTracer::BatchInput{
            .raysOffset = i,
            .raysSize = std::min(DEFAULT_BATCH_SIZE, static_cast<int>(rays.size()) - i),
        };

        auto batchPromise = std::promise<EventList>();
        batchFutures.push_back(batchPromise.get_future());

        auto batchJob = BatchJob{
            .beamlineInput = beamlineInput,
            .batchInput = batchInput,
            .batchPromise = std::move(batchPromise),
        };

        m_batchJobQueue.push(std::move(batchJob));
    }

    return batchFutures;
}

}  // namespace RAYX
