#include "DynamicElements.h"

#include "Behave.h"
#include "Collision.h"
#include "EventType.h"
#include "Helper.h"
#include "Utils.h"

namespace RAYX {

RAYX_FN_ACC
void dynamicElements(int gid, InvState& inv) {
    // initializes the global state.
    inv.globalInvocationId = gid;
    init(inv);

    Ray ray = inv.inputRays[gid];

    OpticalElement nextElement;
    // at the end of this function we apply inTrans, if no collision happened (i.e. nextElement undefined), we want this to do nothing.
    nextElement.m_inTrans = glm::dmat4(1);
    nextElement.m_outTrans = glm::dmat4(1);

    // Iterate through all bounces
    while (true) {
        Collision col = findCollision(ray, inv);
        if (!col.found) {
            // no element was hit.
            // Tracing is done!
            break;
        }

        // transform ray and intersection point in ELEMENT coordiantes
        nextElement = inv.elements[col.elementIndex];
        ray = rayMatrixMult(ray, nextElement.m_inTrans);

        // Calculate interaction(reflection,material, absorption etc.) of ray with detected next element
        auto behaveType = nextElement.m_behaviour.m_type;

        ray.m_pathLength += glm::length(ray.m_position - col.hitpoint);
        ray.m_position = col.hitpoint;
        ray.m_lastElement = col.elementIndex;

        switch (behaveType) {
            case BehaveType::Mirror:
                ray = behaveMirror(ray, col.elementIndex, col, inv);
                break;
            case BehaveType::Grating:
                ray = behaveGrating(ray, col.elementIndex, col, inv);
                break;
            case BehaveType::Slit:
                ray = behaveSlit(ray, col.elementIndex, col, inv);
                break;
            case BehaveType::RZP:
                ray = behaveRZP(ray, col.elementIndex, col, inv);
                break;
            case BehaveType::ImagePlane:
                ray = behaveImagePlane(ray, col.elementIndex, col, inv);
                break;
        }

        // the ray might finalize due to being absorbed, or because an error occured while tracing!
        if (inv.finalized) {
            break;
        }

        recordEvent(ray, ETYPE_JUST_HIT_ELEM, inv);

        // transform back to WORLD coordinates
        ray = rayMatrixMult(ray, nextElement.m_outTrans);
    }

    // store recorded events count
    auto eventsCount = static_cast<int>(inv.nextEventIndex);
    eventsCount = std::max(0, std::min(static_cast<int>(inv.pushConstants.maxEvents), eventsCount));
    inv.outputRayCounts[gid] = eventsCount;
}

}  // namespace RAYX
