#include "Helper.h"
#include "InvocationState.h"

void init() {
    inv_finalized = false;

    // sets all output rays controlled by this shader call to ETYPE_UNINIT.
    for (uint i = 0; i < inv_pushConstants.maxEvents; i++) {
        inv_outputData[output_index(i)].m_eventType = ETYPE_UNINIT;
    }
    inv_nextEventIndex = 0;

    // ray specific "seed" for random numbers -> every ray has a different starting value for the counter that creates the random number
    // TODO Random seeds should probably not be doubles! Casting MAX_UINT64 to double loses precision.
    const uint64_t MAX_UINT64 = ~(uint64_t(0));
    const double MAX_UINT64_DOUBLE = 18446744073709551616.0;
    uint64_t workerCounterNum = MAX_UINT64 / uint64_t(inv_pushConstants.numRays);
    inv_ctr = rayId() * workerCounterNum + uint64_t(inv_pushConstants.randomSeed * MAX_UINT64_DOUBLE);
}

uint64_t rayId() {
    return uint64_t(inv_pushConstants.rayIdStart) + uint64_t(gl_GlobalInvocationID);
}


// `i in [0, maxEvents-1]`.
// Will return the index in outputData to access the `i'th` output ray belonging to this shader call.
// Typically used as `outputData[output_index(i)]`.
uint output_index(uint i) {
    return uint(gl_GlobalInvocationID) * uint(inv_pushConstants.maxEvents) + i;
}

// record an event and store it in the next free spot in outputData.
// `r` will typically be _ray, or some related ray.
void recordEvent(Ray r, double w) {
    if (inv_finalized) { return; }

    if (w == ETYPE_UNINIT) {
        #ifndef GLSL
        RAYX_ERR << "recordEvent failed: weight UNINIT is invalid in recordEvent";
        #endif
    }

    if (inv_nextEventIndex >= inv_pushConstants.maxEvents) {
        inv_finalized = true;

        uint idx = output_index(uint(inv_pushConstants.maxEvents-1));
        inv_outputData[idx].m_eventType = ETYPE_TOO_MANY_EVENTS;

        #ifndef GLSL
        RAYX_ERR << "recordEvent failed: too many events!";
        #endif
    } else {
        r.m_eventType = w;

        uint idx = output_index(uint(inv_nextEventIndex));
        inv_outputData[idx] = r;

        inv_nextEventIndex += 1;
    }
}

// Like `recordEvent` above, but it will prevent recording more events after this.
// Is used for events terminating the path of the ray.
void recordFinalEvent(Ray r, double w) {
    recordEvent(r, w);
    inv_finalized = true;
}

