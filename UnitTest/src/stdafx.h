#pragma once

#include "Catch/catch.hpp"

// --------------------------------------------------------------------------------------------------------------------

// STD library
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <float.h>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

// Containers
#include <vector>
#include <array>

// Multi-threading
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

// --------------------------------------------------------------------------------------------------------------------

// Variables used to keep track of memory allocations/deallocations
extern size_t globalMemoryAllocationCount;
extern size_t globalMemoryDeallocationCount;

// Check helper macros
#define CHECK_NO_ALLOCATION_SINCE(NUM_ALLOC_BEGIN)				\
	{															\
		const size_t numAllocEnd = globalMemoryAllocationCount;	\
		CHECK((NUM_ALLOC_BEGIN) == numAllocEnd);				\
	}
