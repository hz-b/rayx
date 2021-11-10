#pragma once

/**
 *  In this file we are defining the RAYX_API macro, which helps with
 *  building the library (context based import/export of code).
 */

#if defined(RAYX_PLATFORM_WINDOWS)  //  Microsoft
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
#else  //  do nothing and hope for the best?
#ifdef RAYX_BUILD_DLL
#define RAYX_API
#else
#define RAYX_API
#endif
#pragma warning Unknown dynamic link import / export semantics.
#endif