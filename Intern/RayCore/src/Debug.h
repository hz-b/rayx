#pragma once
//

/**
 *  Include this if you want macros for debugging.
 *
 *  Include stdlib before this header if you need it
 *
 */

// Memory leak detection (RAYX_NEW instead of new allows leaks to be detected)
#ifdef RAY_DEBUG_MODE
#ifdef RAYX_PLATFORM_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#define RAYX_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define RAYX_NEW new
#endif

// Debug only code; use it as: DEBUG(<statement>);
#ifdef RAY_DEBUG_MODE
#define RAYX_DEBUG(x) (x)
#else
#define RAYX_DEBUG(x) \
    do {              \
    } while (0)
#endif