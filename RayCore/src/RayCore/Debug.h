// Include this if you want macros for debugging
// Just use it as: DEBUG(<statement>);

#pragma once

#ifdef RAY_DEBUG
#  define DEBUG(x) (x)
#else
#  define DEBUG(x) do {} while (0)
#endif