#pragma once

#include "Engine/Core/platform.h"

#include <stdint.h>
#include <cmath>

constexpr float _VECTORMATH_SLERP_TOL = 0.999f;

#if defined(DESIRE_USE_SSE)
	#include "Engine/Core/math/vectormath/vectormath_SSE.h"
#elif defined(__ARM_NEON__)
	#include "Engine/Core/math/vectormath/vectormath_NEON.h"
#else
	#include "Engine/Core/math/vectormath/vectormath_scalar.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
//	Vectormath Classes
// --------------------------------------------------------------------------------------------------------------------

#include "Engine/Core/math/vectormath/Vector3.h"
#include "Engine/Core/math/vectormath/Vector4.h"
#include "Engine/Core/math/vectormath/Quat.h"
#include "Engine/Core/math/vectormath/Matrix3.h"
#include "Engine/Core/math/vectormath/Matrix4.h"
