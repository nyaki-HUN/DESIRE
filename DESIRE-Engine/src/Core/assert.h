#pragma once

#if defined(DESIRE_DEBUG)
	#include <assert.h>
	#define ASSERT(COND)		assert(COND)
#else
	#define ASSERT(COND)		__noop
#endif
