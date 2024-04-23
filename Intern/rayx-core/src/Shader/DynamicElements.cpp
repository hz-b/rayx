#include "DynamicElements.h"
#include "Collision.h"
#include "Behave.h"
#include "Utils.h"
#include "Helper.h"
#include "EventType.h"

namespace RAYX {

RAYX_FUNC
void dynamicElements(int gid, Inv& inv) {
    // initializes the global state.
    inv.globalInvocationId = gid;
    init(inv);

    #ifdef RAYX_DEBUG_MODE // Debug Matrix only works in GPU Mode and on DEBUG Build Type
    // TODO(Sven): rework debugging on GPU
    // #ifdef GLSL
    //     // Set Debug Struct of current Ray to identity
    //     pushConstants.inv.d_struct[uint(inv.globalInvocationId)]._dMat = dmat4(1);
    // #endif
    #endif

    Ray _ray = inv.rayData[gid];

    Element nextElement;
    // at the end of this function we apply inTrans, if no collision happened (i.e. nextElement undefined), we want this to do nothing.
    nextElement.m_inTrans = dmat4(1);
    nextElement.m_outTrans = dmat4(1);

    // Iterate through all bounces
    while (true) {
        Collision col = findCollision(_ray, inv);
        if (!col.found) {
            // no element was hit.
            // Tracing is done!
            break;
        }

        // transform ray and intersection point in ELEMENT coordiantes
        nextElement = inv.elements[col.elementIndex];
        Ray elem_ray = rayMatrixMult(_ray, nextElement.m_inTrans);

        // Calculate interaction(reflection,material, absorption etc.) of ray with detected next element
        int btype = int(nextElement.m_behaviour.m_type);

        elem_ray.m_pathLength += length(elem_ray.m_position - col.hitpoint);
        elem_ray.m_position = col.hitpoint;
        elem_ray.m_lastElement = col.elementIndex;

        switch(btype) {
            case (BTYPE_MIRROR):
                elem_ray = behaveMirror(elem_ray, col.elementIndex, col, inv);
                break;
            case (BTYPE_GRATING):
                elem_ray = behaveGrating(elem_ray, col.elementIndex, col, inv);
                break;
            case (BTYPE_SLIT) :
                elem_ray = behaveSlit(elem_ray, col.elementIndex, col, inv);
                break;
            case (BTYPE_RZP):
                elem_ray = behaveRZP(elem_ray, col.elementIndex, col, inv);
                break;
            case (BTYPE_IMAGE_PLANE):
                elem_ray = behaveImagePlane(elem_ray, col.elementIndex, col, inv);
                break;
        }

        // the ray might finalize due to being absorbed, or because an error occured while tracing!
        if (inv.finalized) { break; }

        recordEvent(elem_ray, ETYPE_JUST_HIT_ELEM, inv);

        // transform back to WORLD coordinates
        _ray = rayMatrixMult(elem_ray, nextElement.m_outTrans);
    }

    // store recorded events count
    auto eventsCount = static_cast<int>(inv.nextEventIndex - inv.pushConstants.startEventID);
    eventsCount = std::max(0, std::min(static_cast<int>(inv.pushConstants.maxEvents), eventsCount));
    inv.outputRayCounts[gid] = eventsCount;
}

} // namespace RAYX
