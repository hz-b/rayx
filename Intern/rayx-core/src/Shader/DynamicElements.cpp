#include "DynamicElements.h"
#include "Collision.h"
#include "Behave.h"
#include "InvocationState.h"
#include "Utils.h"
#include "Helper.h"

void dynamicElements() {
    // initializes the global state.
    init();

    #ifdef RAYX_DEBUG_MODE // Debug Matrix only works in GPU Mode and on DEBUG Build Type
    // TODO(Sven): rework debugging on GPU
    // #ifdef GLSL
    //     // Set Debug Struct of current Ray to identity
    //     pushConstants.inv_d_struct[uint(gl_GlobalInvocationID)]._dMat = dmat4(1);
    // #endif
    #endif

    Element nextElement;
    // at the end of this function we apply inTrans, if no collision happened (i.e. nextElement undefined), we want this to do nothing.
    nextElement.m_inTrans = dmat4(1);
    nextElement.m_outTrans = dmat4(1);

    // Iterate through all bounces
    while (true) {
        Collision col = findCollision();
        if (!col.found) {
            // no element was hit.
            // Tracing is done!
            return;
        }

        // transform ray and intersection point in ELEMENT coordiantes
        nextElement = inv_elements[col.elementIndex];
        Ray elem_ray = rayMatrixMult(_ray, nextElement.m_inTrans);

        // Calculate interaction(reflection,material, absorption etc.) of ray with detected next element
        int btype = int(nextElement.m_behaviour.m_type);

        elem_ray.m_pathLength += length(elem_ray.m_position - col.hitpoint);
        elem_ray.m_position = col.hitpoint;
        elem_ray.m_lastElement = col.elementIndex;

        switch(btype) {
            case (BTYPE_MIRROR):
                elem_ray = behaveMirror(elem_ray, col.elementIndex, col);
                break;
            case (BTYPE_GRATING):
                elem_ray = behaveGrating(elem_ray, col.elementIndex, col);
                break;
            case (BTYPE_SLIT) :
                elem_ray = behaveSlit(elem_ray, col.elementIndex, col);
                break;
            case (BTYPE_RZP):
                elem_ray = behaveRZP(elem_ray, col.elementIndex, col);
                break;
            case (BTYPE_IMAGE_PLANE):
                elem_ray = behaveImagePlane(elem_ray, col.elementIndex, col);
                break;
        }

        // the ray might finalize due to being absorbed, or because an error occured while tracing!
        if (inv_finalized) { return; }

        recordEvent(elem_ray, ETYPE_JUST_HIT_ELEM);

        // transform back to WORLD coordinates
        _ray = rayMatrixMult(elem_ray, nextElement.m_outTrans);
    }
}
