#pragma once

#include "DESIRE_config.h"

// --------------------------------------------------------------------------------------------------------------------
//	Determine platform
// --------------------------------------------------------------------------------------------------------------------
#if defined(_WIN32)
	#define DESIRE_PLATFORM_WINDOWS
#elif defined(__APPLE__) && __APPLE__
	#include <TargetConditionals.h>
	#if TARGET_OS_TV
		#define DESIRE_PLATFORM_TVOS
	#elif TARGET_OS_IPHONE
		#define DESIRE_PLATFORM_IOS
	#elif TARGET_OS_MAC
		#define DESIRE_PLATFORM_OSX
	#endif
#elif defined(__ANDROID__)
	#define DESIRE_PLATFORM_ANDROID
#elif defined(__linux__) || defined(__linux)
	#define DESIRE_PLATFORM_LINUX
#endif

// --------------------------------------------------------------------------------------------------------------------
//	Platform specific stuff
// --------------------------------------------------------------------------------------------------------------------

#if defined(DESIRE_PLATFORM_WINDOWS)
	#define DESIRE_ATTRIBUTE_PACKED
	#define DESIRE_FORCE_INLINE				__forceinline

	#define DESIRE_PRAGMA(X)				__pragma(X)
	#define DESIRE_DISABLE_WARNINGS			DESIRE_PRAGMA(warning(push, 1))
	#define DESIRE_ENABLE_WARNINGS			DESIRE_PRAGMA(warning(pop))

#elif defined(DESIRE_PLATFORM_LINUX) || defined(DESIRE_PLATFORM_OSX) || defined(DESIRE_PLATFORM_IOS) || defined(DESIRE_PLATFORM_TVOS)
	#define DESIRE_ATTRIBUTE_PACKED			__attribute__((packed))
	#define DESIRE_FORCE_INLINE				inline __attribute__ ((always_inline))

	#define DESIRE_PRAGMA(X)				_Pragma(#X)
//	#define DESIRE_DISABLE_WARNINGS			DESIRE_PRAGMA(GCC diagnostic ignored "-Wall")
//	#define DESIRE_ENABLE_WARNINGS			DESIRE_PRAGMA(GCC diagnostic warning "-Wall")
	#define DESIRE_DISABLE_WARNINGS
	#define DESIRE_ENABLE_WARNINGS

#else
	#define DESIRE_ATTRIBUTE_PACKED
	#define DESIRE_FORCE_INLINE				inline

	#define DESIRE_NO_SWITCH_DEFAULT

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

// C++ offers automatic string concatenation of string literals (compilation fails if something other than a literal is passed to this macro)
#define DESIRE_SAFE_LITERAL(STR)			"" STR

// Array sizeof which returns the number of elements in a static array
template<typename T, size_t N>
constexpr size_t DesireArraySizeHelper(T(&)[N])
{
	return N;
}
#define DESIRE_ASIZEOF(X)					DesireArraySizeHelper(X)

// Compile-time check to make sure the array has the same number of elements as the given Enum
#define DESIRE_CHECK_ARRAY_SIZE(X, ENUM)	static_assert(DESIRE_ASIZEOF(X) == (size_t)ENUM::NUM, "Array size doesn't match the number of elements in the Enum");

// The value of this macro represents the maximum length of a file name string
#define DESIRE_MAX_PATH_LEN					512

// --------------------------------------------------------------------------------------------------------------------
//	Constant Expressions
// --------------------------------------------------------------------------------------------------------------------

constexpr float PI = 3.14159265358979323846f;
constexpr float PI_2 = (PI / 2.0f);
constexpr float PI_4 = (PI / 4.0f);
