#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	Determine platform
// --------------------------------------------------------------------------------------------------------------------
#define DESIRE_PLATFORM_WINDOWS	0
#define DESIRE_PLATFORM_LINUX	0
#define DESIRE_PLATFORM_OSX		0
#define DESIRE_PLATFORM_IOS		0
#define DESIRE_PLATFORM_TVOS	0
#define DESIRE_PLATFORM_ANDROID	0

#if defined(_WIN32)
	#undef DESIRE_PLATFORM_WINDOWS
	#define DESIRE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__) && __APPLE__
	#include <TargetConditionals.h>
	#if TARGET_OS_TV
		#undef DESIRE_PLATFORM_TVOS
		#define DESIRE_PLATFORM_TVOS 1
	#elif TARGET_OS_IPHONE
		#undef DESIRE_PLATFORM_IOS
		#define DESIRE_PLATFORM_IOS 1
	#elif TARGET_OS_MAC
		#undef DESIRE_PLATFORM_OSX
		#define DESIRE_PLATFORM_OSX 1
	#endif
#elif defined(__ANDROID__)
	#undef DESIRE_PLATFORM_ANDROID
	#define DESIRE_PLATFORM_ANDROID 1
#elif defined(__linux__) || defined(__linux)
	#undef DESIRE_PLATFORM_LINUX
	#define DESIRE_PLATFORM_LINUX 1
#endif

// --------------------------------------------------------------------------------------------------------------------
//	Platform specific macros
// --------------------------------------------------------------------------------------------------------------------

#if defined(DESIRE_PLATFORM_WINDOWS)
	#define DESIRE_ATTRIBUTE_PACKED

	#define DESIRE_PRAGMA(X)				__pragma(X)
	#define DESIRE_DISABLE_WARNINGS			DESIRE_PRAGMA(warning(push, 1))
	#define DESIRE_ENABLE_WARNINGS			DESIRE_PRAGMA(warning(pop))

#elif defined(DESIRE_PLATFORM_LINUX) || defined(DESIRE_PLATFORM_OSX) || defined(DESIRE_PLATFORM_IOS) || defined(DESIRE_PLATFORM_TVOS)
	#define DESIRE_ATTRIBUTE_PACKED			__attribute__((packed))

	#define DESIRE_PRAGMA(X)				_Pragma(#X)
//	#define DESIRE_DISABLE_WARNINGS			DESIRE_PRAGMA(GCC diagnostic ignored "-Wall")
//	#define DESIRE_ENABLE_WARNINGS			DESIRE_PRAGMA(GCC diagnostic warning "-Wall")
	#define DESIRE_DISABLE_WARNINGS
	#define DESIRE_ENABLE_WARNINGS

#else
	#define DESIRE_ATTRIBUTE_PACKED

	#define DESIRE_PRAGMA(X)
	#define DESIRE_DISABLE_WARNINGS
	#define DESIRE_ENABLE_WARNINGS

	static_assert(false, "Unsupported platform");
#endif

// Enable SSE on desktop as all 64-bit CPU has at least SSE2
#if defined(DESIRE_PLATFORM_WINDOWS) || defined(DESIRE_PLATFORM_LINUX) || defined(DESIRE_PLATFORM_OSX)
	#define DESIRE_USE_SSE

	// Enable SSE 4.1
	#if !defined(__SSE4_1__)
		#define __SSE4_1__
	#endif
#endif

// --------------------------------------------------------------------------------------------------------------------
//	Macros
// --------------------------------------------------------------------------------------------------------------------

// Stringify utility macro
#define _DESIRE_STRINGIFY(STR)				#STR
#define DESIRE_STRINGIFY(STR)				_DESIRE_STRINGIFY(STR)

// Macro for displaying compile time message with file and line number
#define DESIRE_TODO(STR)					DESIRE_PRAGMA(message(__FILE__ "(" DESIRE_STRINGIFY(__LINE__) "): TODO - " STR))

// For unused variables
#define DESIRE_UNUSED(X)					((void)X)

// Array sizeof which returns the number of elements in a static array
template<typename T, size_t N>
constexpr size_t DesireArraySizeHelper(T(&)[N])
{
	return N;
}
#define DESIRE_ASIZEOF(X)					DesireArraySizeHelper(X)

// Compile-time check to make sure the array has the same number of elements as 'count'
#define DESIRE_CHECK_ARRAY_SIZE(X, COUNT)	static_assert(DESIRE_ASIZEOF(X) == (size_t)COUNT, "Array size doesn't match the number of elements");

// The value of this macro represents the maximum length of a file name string
#define DESIRE_MAX_PATH_LEN					512
