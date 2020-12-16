#pragma once

#if defined(RAY_PLATFORM_WINDOWS) //  Microsoft
	#ifdef RAY_BUILD_DLL
		#define RAY_API __declspec(dllexport)
	#else
		#define RAY_API __declspec(dllimport)
	#endif
#elif defined(RAY_PLATFORM_GCC) //  GCC
	#ifdef RAY_BUILD_DLL
		#define RAY_API __attribute__((visibility("default")))
	#else
		#define RAY_API
	#endif
#else //  do nothing and hope for the best?
	#ifdef RAY_BUILD_DLL
		#define RAY_API
	#else
		#define RAY_API
	#endif
	#pragma warning Unknown dynamic link import / export semantics.
#endif