#include "test_metrix.h"
#include "Model/Beamline.h"

void trace () {
    [[maybe_unused]] auto metrix = createMetrixBeamline();
    [[maybe_unused]] auto metrixModel = rayx::detail::toModel(*metrix);
    // [[maybe_unused]] auto metrixHost = toHost<Dev>(q, metrixModel);
    // [[maybe_unused]] auto metrixDevice = toDevice(metrixHost);
    // [[maybe_unused]] auto rays = rayx::trace(metrixDevice);
}
