#define CATCH_CONFIG_MAIN
#include "Catch/catch.hpp"

// Custom new/delete is defined here to be able to keep track of memory allocations
extern size_t globalMemoryAllocationCount = 0;
extern size_t globalMemoryDeallocationCount = 0;

void* operator new(size_t size)
{
	++globalMemoryAllocationCount;
	return malloc(size);
}

void* operator new[](size_t size)
{
	++globalMemoryAllocationCount;
	return malloc(size);
}

void operator delete(void* ptr)
{
	++globalMemoryDeallocationCount;
	free(ptr);
}
void operator delete[](void* ptr)
{
	++globalMemoryDeallocationCount;
	free(ptr);
}
