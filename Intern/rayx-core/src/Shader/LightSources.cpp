#include "LightSources.h"

#include "InvocationState.h"
#include "Throw.h"

namespace {

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // unnamed namespace

namespace RAYX {

RAYX_FN_ACC
Ray lightSourceGetRay(const InvState& inv, Rand& rand) {
    const int32_t rayId = inv.globalInvocationId + inv.pushConstants.rayIdStart;
    const int32_t sourceId = inv.lightSourceId;

    return std::visit(overloaded{[&](const MatrixSource& lightSource) { return lightSource.getRay(rayId, sourceId, rand); },
                                 [&](const auto& lightSource) { return lightSource.getRay(sourceId, rand); },
                                 [&](const BatchInputRays&) { return inv.inputRays[inv.globalInvocationId]; },
                                 [](std::monostate) {
                                     _throw("lightsource is empty");
                                     return Ray{};
                                 }

                      },
                      *inv.lightSource);
};

}  // namespace RAYX
