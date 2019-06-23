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
//	Macros
// --------------------------------------------------------------------------------------------------------------------

// Enable SSE on desktop as all 64-bit CPU has at least SSE2
#if DESIRE_PLATFORM_WINDOWS || DESIRE_PLATFORM_LINUX || DESIRE_PLATFORM_OSX
	#define DESIRE_USE_SSE

	// Enable SSE 4.1
	#if !defined(__SSE4_1__)
		#define __SSE4_1__
	#endif
#endif

// Stringify and concat macro magic
#define _DESIRE_STRINGIFY(STR)				#STR
#define DESIRE_STRINGIFY(STR)				_DESIRE_STRINGIFY(STR)
#define _DESIRE_CONCAT_MACRO(A, B)			A ## B
#define DESIRE_CONCAT_MACRO(A, B)			_DESIRE_CONCAT_MACRO(A, B)

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
