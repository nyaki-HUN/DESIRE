#pragma once

#include "Catch/catch.hpp"

// --------------------------------------------------------------------------------------------------------------------

// STD library
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <cfloat>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <functional>
#include <utility>
#include <memory>

// Containers
#include <array>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <unordered_map>

// Thread support
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>

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
