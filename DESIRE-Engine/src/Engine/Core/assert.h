#pragma once

#if defined(_DEBUG)
	#include <assert.h>
	#define ASSERT(COND)		assert(COND)
#else
	#define ASSERT(COND)		{ if(false) { ((void)(COND)); } }
#endif
