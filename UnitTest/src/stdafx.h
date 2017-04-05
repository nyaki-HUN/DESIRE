#pragma once

#include "Catch/catch.hpp"

// --------------------------------------------------------------------------------------------------------------------

// STD library
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <cctype>
#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

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

#define CHECK_FLOATS(NUM1, NUM2)			CHECK(std::fabsf((NUM1) - (NUM2)) < FLT_EPSILON)
