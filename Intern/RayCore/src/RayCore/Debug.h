#pragma once
// Include this if you want macros for debugging

/* Known issues:
* - Include stdlib before this header if you need it
* -
*/


// Memory leak detection
#	ifdef RAY_DEBUG_MODE
#       ifdef RAYX_PLATFORM_WINDOWS
#		    define _CRTDBG_MAP_ALLOC
#		    include <crtdbg.h>
#       endif
#		    define RAY_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#	else
#		define RAY_NEW new
#	endif

// Debug only code; just use it as: DEBUG(<statement>);
#ifdef RAY_DEBUG_MODE
#  define RAY_DEBUG(x) (x)
#else
#  define RAY_DEBUG(x) do {} while (0)
#endif