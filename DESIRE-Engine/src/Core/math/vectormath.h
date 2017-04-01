#pragma once

#include "Core/platform.h"

#include <stdint.h>
#include <math.h>

#define _VECTORMATH_SLERP_TOL 0.999f

#if defined(DESIRE_USE_SSE)
	#include "Core/math/vectormath/vectormath_SSE.h"
#elif defined(__ARM_NEON__)
	#include "Core/math/vectormath/vectormath_NEON.h"
#else
	#include "Core/math/vectormath/vectormath_scalar.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
//	Vectormath Classes
// --------------------------------------------------------------------------------------------------------------------

#include "Core/math/vectormath/Vector3.h"
#include "Core/math/vectormath/Vector4.h"
#include "Core/math/vectormath/Quat.h"
#include "Core/math/vectormath/Matrix3.h"
#include "Core/math/vectormath/Matrix4.h"

#if defined(DESIRE_USE_SSE)
	#include "Core/math/vectormath/quaternion_SSE.hpp"
	#include "Core/math/vectormath/matrix_SSE.hpp"
#elif defined(__ARM_NEON__)
	#include "Core/math/vectormath/quaternion_scalar.hpp"
	#include "Core/math/vectormath/matrix_NEON.hpp"
#else
	#include "Core/math/vectormath/quaternion_scalar.hpp"
	#include "Core/math/vectormath/matrix_scalar.hpp"
#endif
