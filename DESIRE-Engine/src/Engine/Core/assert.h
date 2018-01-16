#pragma once

#if defined(_DEBUG)
	#include <assert.h>
	#define ASSERT(COND)		assert(COND)
#else
	#define ASSERT(COND)		((void)0)
#endif
