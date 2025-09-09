#pragma once

// Memory leak detection (RAYX_NEW instead of new allows leaks to be detected)
#ifdef RAYX_DEBUG_MODE
#ifdef RAYX_PLATFORM_MSVC
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#define RAYX_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define RAYX_NEW new
#endif

/**
 *  Defining the RAYX_API macro, which helps with
 *  building the library (context based import/export of code).
 */
#if defined(RAYX_PLATFORM_MSVC)  //  Microsoft
#ifdef RAYX_BUILD_DLL
#define RAYX_API __declspec(dllexport)
#else
#define RAYX_API __declspec(dllimport)
#endif
#elif defined(RAYX_PLATFORM_GCC)  //  GCC
#ifdef RAYX_BUILD_DLL
#define RAYX_API __attribute__((visibility("default")))
#else
#define RAYX_API
#endif
#elif defined(RAYX_PLATFORM_CLANG)  //  Clang
#ifdef RAYX_BUILD_DLL
#define RAYX_API __attribute__((visibility("default")))
#else
#define RAYX_API
#endif
#else  //  do nothing and hope for the best?
#ifdef RAYX_BUILD_DLL
#define RAYX_API
#else
#define RAYX_API
#endif
#endif

// make string comparison available for msvc compiler
// not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#ifdef RAYX_BUILD_DLL
#include <alpaka/core/Common.hpp>
#define RAYX_FN_ACC ALPAKA_FN_ACC
#else
#define RAYX_FN_ACC
#endif

#if defined(RAYX_BUILD_DLL) && defined(RAYX_CUDA_ENABLED)
#define RAYX_CONSTEXPR_ACC constexpr __device__
#else
#define RAYX_CONSTEXPR_ACC constexpr
#endif
