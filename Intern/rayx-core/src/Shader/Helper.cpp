#include "Helper.h"

#include "EventType.h"
#include "Throw.h"

namespace RAYX {

RAYX_FN_ACC
void init(InvState& inv) {
    inv.finalized = false;

    // TODO(Sven): dont waste time with initializing
    // sets all output rays controlled by this shader call to ETYPE_UNINIT.
    for (uint32_t i = uint32_t(inv.pushConstants.startEventID); i < inv.pushConstants.maxEvents; i++) {
        inv.outputRays[output_index(i, inv)].m_eventType = ETYPE_UNINIT;
    }
    inv.nextEventIndex = 0;

    // ray specific "seed" for random numbers -> every ray has a different starting value for the counter that creates the random number
    // TODO Random seeds should probably not be doubles! Casting MAX_UINT64 to double loses precision.
    const uint64_t MAX_UINT64 = ~(uint64_t(0));
    const double MAX_UINT64_DOUBLE = 18446744073709551616.0;
    uint64_t workerCounterNum = MAX_UINT64 / uint64_t(inv.pushConstants.numRays);
    inv.ctr = rayId(inv) * workerCounterNum + uint64_t(inv.pushConstants.randomSeed * MAX_UINT64_DOUBLE);
}

RAYX_FN_ACC
uint64_t rayId(InvState& inv) { return uint64_t(inv.pushConstants.rayIdStart) + uint64_t(inv.globalInvocationId); }

// `i in [0, maxEvents-1]`.
// Will return the index in outputRays to access the `i'th` output ray belonging to this shader call.
// Typically used as `outputRays[output_index(i)]`.
RAYX_FN_ACC
uint32_t output_index(uint32_t i, InvState& inv) {
    return uint32_t(inv.globalInvocationId) * uint32_t(inv.pushConstants.maxEvents - inv.pushConstants.startEventID) + i -
           uint32_t(inv.pushConstants.startEventID);
}

// record an event and store it in the next free spot in outputRays.
// `r` will typically be ray, or some related ray.
RAYX_FN_ACC
void recordEvent(Ray r, double w, InvState& inv) {
    if (inv.nextEventIndex < inv.pushConstants.startEventID) {
        inv.nextEventIndex += 1;
        return;
    }
    if (inv.finalized) {
        return;
    }

    // recording of event type ETYPE_UINIT is forbidden.
    if (w == ETYPE_UNINIT) {
        _throw("recordEvent failed: weight UNINIT is invalid in recordEvent");

        return;
    }

    // the outputRays array might be full!
    if (inv.nextEventIndex >= inv.pushConstants.maxEvents) {
        inv.finalized = true;

        // change the last event to "ETYPE_TOO_MANY_EVENTS".
        uint32_t idx = output_index(uint32_t(inv.pushConstants.maxEvents - 1), inv);
        inv.outputRays[idx].m_eventType = ETYPE_TOO_MANY_EVENTS;

        _throw("recordEvent failed: too many events!");

        return;
    }

    r.m_eventType = w;

    uint32_t idx = output_index(uint32_t(inv.nextEventIndex), inv);
    inv.outputRays[idx] = r;

    inv.nextEventIndex += 1;
}

// Like `recordEvent` above, but it will prevent recording more events after this.
// Is used for events terminating the path of the ray.
RAYX_FN_ACC
void recordFinalEvent(Ray r, double w, InvState& inv) {
    recordEvent(r, w, inv);
    inv.finalized = true;
}

}  // namespace RAYX
