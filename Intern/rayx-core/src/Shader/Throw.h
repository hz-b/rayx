#pragma once

#include "Core.h"

#define _throw(string, ...)                                                                       \
    do {                                                                                          \
        printf("Error occurred while executing kernel: " string "\n" __VA_OPT__(, ) __VA_ARGS__); \
        assert(false);                                                                            \
    } while (0)

#define _assert(condition, string, ...) \
    if (!(condition)) _throw("assert(" #condition "): " string __VA_OPT__(, ) __VA_ARGS__)

#define _warn(condition, string, ...) \
    if (!(condition)) printf("Warning occurred while executing kernel: " string "\n" __VA_OPT__(, ) __VA_ARGS__)

#ifdef RAYX_DEBUG_MODE
#define _debug_throw_debug(string, ...) _throw(string __VA_OPT__(, ) __VA_ARGS__)
#define _debug_assert(condition, string, ...) _assert(condition, string __VA_OPT__(, ) __VA_ARGS__)
#define _debug_warn(condition, string, ...) _warn(condition, string __VA_OPT__(, ) __VA_ARGS__)
#else
#define _debug_throw(string, ...)
#define _debug_assert(condition, string, ...)
#define _debug_warn(condition, string, ...)
#endif
