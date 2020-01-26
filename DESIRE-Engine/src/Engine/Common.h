#pragma once

#include "Engine/Core/platform.h"
#include "Engine/Core/assert.h"
#include "Engine/Core/Log/Log.h"

#include "Engine/Modules.h"

// Use stack memory to allocate an array with the give type and size
#define DESIRE_STACKALLOCATE_ARRAY(T, NAME, SIZE)	T* NAME(reinterpret_cast<T*>(reinterpret_cast<size_t>(alloca(sizeof(T) * (SIZE)	+ alignof(T))) & ~(alignof(T) - 1)))

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
