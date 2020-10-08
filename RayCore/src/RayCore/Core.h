#pragma once

#ifdef RAY_PLATFORM_WINDOWS
	#ifdef RAY_BUILD_DLL
		#define RAY_API __declspec(dllexport)
	#else
		#define RAY_API __declspec(dllimport)
	#endif
#else
	#error RAY-UI only supports Windows for now
#endif