#pragma once

#include "Engine/Core/platform.h"

#include <stdint.h>
#include <cmath>

#if defined(DESIRE_USE_SSE)
	#include "Engine/Core/math/vectormath/vectormath_SSE.h"
#elif defined(__ARM_NEON__)
	#include "Engine/Core/math/vectormath/vectormath_NEON.h"
#else
	#include "Engine/Core/math/vectormath/vectormath_scalar.h"
#endif
