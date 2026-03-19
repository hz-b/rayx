#pragma once

#include <alpaka/alpaka.hpp>

#include "Tracer.h"
#include "rayx/Core.h"
#include "rayx/IO/Exception.h"
#include "rayx/Model/Beamline.h"

namespace rayx::detail {

constexpr int DEFAULT_BATCH_SIZE = 100000;

constexpr int getMaxEvents(const SequentialTraceModeType, const int numObjects) { return numObjects; }

constexpr int getMaxEvents(const NonSequentialTraceMode mode, const int numObjects) {
    if (mode.maxEventsPerPath) {
        if (*mode.maxEventsPerPath <= 0)
            throw std::invalid_argument(std::format("maxEventsPerPath must be positive, but got {}", *mode.maxEventsPerPath));
        return *mode.maxEventsPerPath;
    }
    return numObjects * 2 + 8;  // this is arbitrary, hoping this is enough for most beamlines
}

constexpr int getMaxEvents(const TraceMode mode, const int numObjects) {
    return std::visit([numObjects](const auto& arg) { return getMaxEvents(arg, numObjects); }, mode);
}

template <typename AccTag>
class MegaKernelTracer : public DeviceTracer {
    using Dim = alpaka::DimInt<1>;
    using Idx = int32_t;
    using Acc = alpaka::TagToAcc<AccTag, Dim, Idx>;

  public:
    MegaKernelTracer(const Device::Index deviceIndex) : m_deviceIndex(deviceIndex) {}

    Rays trace(model::Beamline beamline, const TraceOptions& options) override {
        const auto numSources             = beamline.sources.size();
        const auto numElements            = beamline.elements.size();
        const auto numObjects             = numSources + numElements;
        const auto maxEvents              = getMaxEvents(options.traceMode, numObjects);
        const auto objectRecordMask = toRawObjectMask(options.objectRecordMask, numObjects);
        const auto attrRecordMask         = options.attrRecordMask;
        const auto batchSize        = options.batchSize ? *options.batchSize : DEFAULT_BATCH_SIZE;
        const auto isSequential           = std::holds_alternative<SequentialTraceModeType>(options.traceMode);
        throw unimplementedCodePathException();
    }

  private:
    Device::Index m_deviceIndex;
};

}  // namespace rayx::detail
