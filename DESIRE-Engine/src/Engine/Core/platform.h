#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Determine platform
// --------------------------------------------------------------------------------------------------------------------

// Desktop platforms
#define DESIRE_PLATFORM_WINDOWS	0
#define DESIRE_PLATFORM_LINUX	0
#define DESIRE_PLATFORM_OSX		0
// Console platforms
#define DESIRE_PLATFORM_XBOXONE	0
#define DESIRE_PLATFORM_PS4		0
#define DESIRE_PLATFORM_NX		0
// Mobile platforms
#define DESIRE_PLATFORM_ANDROID	0
#define DESIRE_PLATFORM_IOS		0

#if defined(_DURANGO) || defined(_XBOX_ONE)
	#undef DESIRE_PLATFORM_XBOXONE
	#define DESIRE_PLATFORM_XBOXONE 1
#elif defined(_WIN32) || defined(_WIN64)
	#include <sdkddkver.h>
	#undef DESIRE_PLATFORM_WINDOWS
	#define DESIRE_PLATFORM_WINDOWS 1
#elif defined(__ANDROID__)
	// Also defines __linux__
	#undef DESIRE_PLATFORM_ANDROID
	#define DESIRE_PLATFORM_ANDROID 1
#elif defined(__linux__) || defined(__linux)
	#undef DESIRE_PLATFORM_LINUX
	#define DESIRE_PLATFORM_LINUX 1
#elif defined(__APPLE__) && __APPLE__
	#include <TargetConditionals.h>
	#if TARGET_OS_IPHONE
		#undef DESIRE_PLATFORM_IOS
		#define DESIRE_PLATFORM_IOS 1
	#elif TARGET_OS_MAC
		#undef DESIRE_PLATFORM_OSX
		#define DESIRE_PLATFORM_OSX 1
	#endif
#elif defined(__ORBIS__)
	#undef DESIRE_PLATFORM_PS4
	#define DESIRE_PLATFORM_PS4 1
#elif defined(__NX__)
	#undef DESIRE_PLATFORM_NX
	#define DESIRE_PLATFORM_NX 1
#endif

// Sanity check
#if (DESIRE_PLATFORM_WINDOWS + DESIRE_PLATFORM_LINUX + DESIRE_PLATFORM_OSX + DESIRE_PLATFORM_XBOXONE + DESIRE_PLATFORM_PS4 + DESIRE_PLATFORM_NX + DESIRE_PLATFORM_ANDROID + DESIRE_PLATFORM_IOS) != 1
	#error "Platform detection failed"
#endif

#if DESIRE_PLATFORM_WINDOWS || DESIRE_PLATFORM_LINUX || DESIRE_PLATFORM_OSX
	#define DESIRE_PLATFORM_TYPE_DESKTOP	1
	#define DESIRE_PLATFORM_TYPE_CONSOLE	0
	#define DESIRE_PLATFORM_TYPE_MOBILE		0
#elif DESIRE_PLATFORM_XBOXONE || DESIRE_PLATFORM_PS4 || DESIRE_PLATFORM_NX
	#define DESIRE_PLATFORM_TYPE_DESKTOP	0
	#define DESIRE_PLATFORM_TYPE_CONSOLE	1
	#define DESIRE_PLATFORM_TYPE_MOBILE		0
#elif DESIRE_PLATFORM_ANDROID || DESIRE_PLATFORM_IOS
	#define DESIRE_PLATFORM_TYPE_DESKTOP	0
	#define DESIRE_PLATFORM_TYPE_CONSOLE	0
	#define DESIRE_PLATFORM_TYPE_MOBILE		1
#endif

// --------------------------------------------------------------------------------------------------------------------

#if DESIRE_PLATFORM_TYPE_DESKTOP || DESIRE_PLATFORM_XBOXONE || DESIRE_PLATFORM_PS4
	#define DESIRE_USE_SSE	1
	#define DESIRE_USE_NEON	0

	// Enable SSE 4.1
	#if !defined(__SSE4_1__)
		#define __SSE4_1__
	#endif

	// Header files for SIMD intrinsics:
	//	<immintrin.h>	AVX			Intel(R) Architecture intrinsic functions
	//	<wmmintrin.h>	AES			Intel(R) AES and PCLMULQDQ intrinsics
	//	<ammintrin.h>	SSE4A		AMD-specific intrinsics
	//
	//	<nmmintrin.h>	SSE4.2
	//	<smmintrin.h>	SSE4.1
	//	<tmmintrin.h>	SSSE3
	//	<pmmintrin.h>	SSE3
	//	<emmintrin.h>	SSE2
	//	<xmmintrin.h>	SSE
	//	<mmintrin.h>	MMX

	#if defined(__SSE4_1__)
		#include <smmintrin.h>
	#else
		// We are safe to use SSE3 because an x64 CPU with no SSE3 support is the 'first-generation' 64-bit which
		// isn't supported by Windows 8.1 x64 native due to the requirements for CMPXCHG16b, PrefetchW, and LAHF/SAHF
		#include <pmmintrin.h>
	#endif
#elif DESIRE_PLATFORM_ANDROID || DESIRE_PLATFORM_IOS || DESIRE_PLATFORM_NX
	#define DESIRE_USE_SSE	0
	#define DESIRE_USE_NEON	1

	#include <arm64_neon.h>
#else
	#define DESIRE_USE_SSE	0
	#define DESIRE_USE_NEON	0
#endif

// --------------------------------------------------------------------------------------------------------------------

// The value of this macro represents the maximum length of a file name string
#define DESIRE_MAX_PATH_LEN					512

// --------------------------------------------------------------------------------------------------------------------

#if defined(_MSC_VER)
	#define DESIRE_ATTRIBUTE_PACKED
	#define DESIRE_PRAGMA(X)				__pragma(X)
	#define DESIRE_DISABLE_WARNINGS			DESIRE_PRAGMA(warning(push, 1))
	#define DESIRE_ENABLE_WARNINGS			DESIRE_PRAGMA(warning(pop))
#else
	#define DESIRE_ATTRIBUTE_PACKED			__attribute__((packed))
	#define DESIRE_PRAGMA(X)				_Pragma(#X)
	#define DESIRE_DISABLE_WARNINGS
	#define DESIRE_ENABLE_WARNINGS
#endif

// --------------------------------------------------------------------------------------------------------------------
