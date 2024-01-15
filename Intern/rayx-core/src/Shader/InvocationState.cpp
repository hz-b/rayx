#include "InvocationState.h"

/**************************************************************
 *                    vars
 **************************************************************/

#ifndef GLSL
int gl_GlobalInvocationID;
#endif

bool inv_finalized;

// the random number state.
uint64_t inv_ctr;

uint64_t inv_nextEventIndex;


/**************************************************************
 *                    SHADER ARRAYS
 **************************************************************/

// don't use this directly. Use `_ray` instead.
SHADER_ARRAY(Ray, inv_rayData, 0, ibuf);

// don't use this directly. Use `recordEvent` instead.
SHADER_ARRAY(Ray, inv_outputData, 1, obuf);

// std430 is required so that Element is layouted like in C++.
// see https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL) for more details on layouting.
SHADER_ARRAY(Element, inv_elements, 2, qbuf);

// TODO unused!
SHADER_ARRAY(dvec4, inv_xyznull, 3, xyzbuf);

// matIdx[0..91] contains the indices of the Palik Table, whereas matIdx[92..92+91] contains the indices of the NffTable.
// generally, an entry of matIdx tells you the index you need to look up in the 'mat'-table below.
SHADER_ARRAY(int, inv_matIdx, 4, materialIndexBuf);

// contains: (Palik tables of all materials concatenated together) concatenated with (Nff tables of all materials concatenated together)
// use the matIdx-table to determine the relevant indices for when which table starts and stops.
SHADER_ARRAY(double, inv_mat, 5, materialBuf);

#ifdef RAYX_DEBUG_MODE
SHADER_ARRAY(_debug_struct, inv_d_struct, 6, debugBuf);
#endif


/**************************************************************
 *                    PushConstants
 **************************************************************/
#ifndef GLSL
    PushConstants inv_pushConstants;
#else 
    layout( push_constant ) uniform constants
    {   
        double rayIdStart;
        double numRays;
        double randomSeed;
        double maxEvents;
        double sequential; // sequential tracing only.
    } inv_pushConstants;
#endif
