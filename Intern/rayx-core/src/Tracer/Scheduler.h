#pragma once

#include <future>
#include <memory>
#include <vector>

#include "BlockingQueue.h"
#include "Core.h"
#include "DeviceConfig.h"
#include "Shader/Ray.h"
#include "Tracer.h"

namespace RAYX {

struct RAYX_API Batch {
    std::vector<Ray> events;
    std::vector<std::span<Ray>> rays;
};

class RAYX_API Scheduler {
  public:
    Scheduler(const DeviceConfig& deviceConfig);
    ~Scheduler();

    using TraceResult = std::vector<std::future<Batch>>;
    TraceResult trace(const Beamline& beamline);

  private:
    struct BatchJob {
        std::shared_ptr<DeviceTracer::BeamlineInput> beamlineInput;
        DeviceTracer::BatchInput batchInput;
        std::promise<Batch> batchPromise;
    };

    BlockingQueue<BatchJob> m_batchJobQueue;
    std::atomic<bool> m_deviceTracerShouldStop;
    std::vector<std::future<void>> m_deviceTracerFutures;
};

}  // namespace RAYX
