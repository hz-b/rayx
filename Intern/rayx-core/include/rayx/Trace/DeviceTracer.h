#pragma once

namespace rayx::detail::model {
struct Beamline;
}

namespace rayx {
struct Rays;
struct TraceOptions;
}  // namespace rayx

namespace rayx::detail {

class DeviceTracer {
  public:
    virtual Rays trace(model::Beamline beamline, const TraceOptions& options);
};

}  // namespace rayx::detail
