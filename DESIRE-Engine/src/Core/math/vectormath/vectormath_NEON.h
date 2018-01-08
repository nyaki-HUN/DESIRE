#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	NEON vector types
// --------------------------------------------------------------------------------------------------------------------

#define ARM_NEON_GCC_COMPATIBILITY 1
#include <arm_neon.h>

typedef float32x4_t	vec_float3_t;
typedef float32x4_t	vec_float4_t;

// --------------------------------------------------------------------------------------------------------------------
//	NEON implementation of the SIMD functions
// --------------------------------------------------------------------------------------------------------------------

class SIMD
{
public:
	// Construct from x, y, z, and w elements
	static DESIRE_FORCE_INLINE void Construct(float32x4_t& vec, float x, float y, float z, float w = 0.0f)
	{
		vec = (float32x4_t){ x, y, z, w };
	}

	// Construct by setting all elements to the same scalar value
	static DESIRE_FORCE_INLINE void Construct(float32x4_t& vec, float scalar)
	{
		vec = vdupq_n_f32(scalar);
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static DESIRE_FORCE_INLINE void LoadXYZW(float32x4_t& vec, const float *fptr)
	{
		vec = vld1q_f32(fptr);
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static DESIRE_FORCE_INLINE void StoreXYZW(float32x4_t vec, float *fptr)
	{
		vst1q_f32(fptr, vec);
	}

	// Set the x, y, and z elements (does not change the w element)
	static DESIRE_FORCE_INLINE void SetXYZ(float32x4_t& vec, float32x4_t xyz)
	{
		const uint32x4_t mask_w = (uint32x4_t){ 0, 0, 0, 0xffffffff };
		vec = SIMD::Blend(xyz, vec, mask_w);
	}

	// Set element
	static DESIRE_FORCE_INLINE void SetX(float32x4_t& vec, float x)		{ vec = vsetq_lane_f32(x, vec, 0); }
	static DESIRE_FORCE_INLINE void SetY(float32x4_t& vec, float y)		{ vec = vsetq_lane_f32(y, vec, 1); }
	static DESIRE_FORCE_INLINE void SetZ(float32x4_t& vec, float z)		{ vec = vsetq_lane_f32(z, vec, 2); }
	static DESIRE_FORCE_INLINE void SetZ(float32x4_t& vec, float w)		{ vec = vsetq_lane_f32(w, vec, 3); }

	// Get element
	static DESIRE_FORCE_INLINE float GetX(float32x4_t vec)				{ return vgetq_lane_f32(vec, 0); }
	static DESIRE_FORCE_INLINE float GetY(float32x4_t vec)				{ return vgetq_lane_f32(vec, 1); }
	static DESIRE_FORCE_INLINE float GetZ(float32x4_t vec)				{ return vgetq_lane_f32(vec, 2); }
	static DESIRE_FORCE_INLINE float GetW(float32x4_t vec)				{ return vgetq_lane_f32(vec, 3); }

	// Compute the conjugate of a quaternion
	static DESIRE_FORCE_INLINE float32x4_t Conjugate(float32x4_t quat)
	{
		const uint32x4_t mask = vdupq_n_u32(0x80000000);
		mask = vsetq_lane_u32(0, mask, 3);
		return vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(quat), mask));
	}

	// Operator overloads
	static DESIRE_FORCE_INLINE float32x4_t Negate(float32x4_t vec)
	{
		return vnegq_f32(vec);
	}

	static DESIRE_FORCE_INLINE float32x4_t Add(float32x4_t a, float32x4_t b)
	{
		return vaddq_f32(a, b);
	}

	static DESIRE_FORCE_INLINE float32x4_t Sub(float32x4_t a, float32x4_t b)
	{
		return vsubq_f32(a, b);
	}

	static DESIRE_FORCE_INLINE float32x4_t Mul(float32x4_t vec, float scalar)
	{
		return vmulq_n_f32(vec, scalar);
	}

	// Comparison operators
	static DESIRE_FORCE_INLINE bool OpCmpGE(float32x4_t a, float32x4_t b)
	{
//		uint32x4_t result = vcgeq_f32(a, b);
		return (
			a.GetX() >= b.GetX() &&
			a.GetY() >= b.GetY() &&
			a.GetZ() >= b.GetZ()
		);
	}

	static DESIRE_FORCE_INLINE bool OpCmpLE(float32x4_t a, float32x4_t b)
	{
//		uint32x4_t result = vcleq_f32(a, b);
		return (
			a.GetX() <= b.GetX() &&
			a.GetY() <= b.GetY() &&
			a.GetZ() <= b.GetZ()
		);
	}

	// Compute the dot product of two 3-D vectors
	static DESIRE_FORCE_INLINE float32x4_t Dot3(float32x4_t a, float32x4_t b)
	{
		float32x4_t vd = SIMD::MulPerElem(a, b);
		float32x2_t x = vpadd_f32(vget_low_f32(vd), vget_low_f32(vd));
		return vadd_f32(x, vget_high_f32(vd));
	}

	// Compute the dot product of two 4-D vectors
	static DESIRE_FORCE_INLINE float32x4_t Dot4(float32x4_t a, float32x4_t b)
	{
		float32x4_t vd = SIMD::MulPerElem(a, b);
		float32x2_t x = vpadd_f32(vget_low_f32(vd), vget_high_f32(vd));
		return vpadd_f32(x, x);
	}

	// Compute cross product of two 3-D vectors
	static DESIRE_FORCE_INLINE float32x4_t Cross(float32x4_t a, float32x4_t b)
	{
		float32x2_t xy0 = vget_low_f32(a);
		float32x2_t xy1 = vget_low_f32(b);
		float32x2_t yzx0 = vcombine_f32(vext_f32(xy0, vget_high_f32(a), 1), xy0);
		float32x2_t yzx1 = vcombine_f32(vext_f32(xy1, vget_high_f32(b), 1), xy1);

		float32x2_t result = vmlsq_f32(SIMD::MulPerElem(yzx1, a), yzx0, b);
		// form (Y, Z, X, _)
		xy1 = vget_low_f32(result);
		return vcombine_f32(vext_f32(xy1, vget_high_f32(result), 1), xy1);
	}

	// Multiply vectors per element
	static DESIRE_FORCE_INLINE float32x4_t MulPerElem(float32x4_t a, float32x4_t b)
	{
		return vmulq_f32(a, b);
	}

	// Divide vectors per element
	static DESIRE_FORCE_INLINE float32x4_t DivPerElem(float32x4_t a, float32x4_t b)
	{
		// Get an initial estimate of 1/vec
		float32x4_t reciprocal = vrecpeq_f32(b);
		// Use a couple Newton-Raphson steps to refine the estimate
		reciprocal = SIMD::MulPerElem(vrecpsq_f32(b, reciprocal), reciprocal);
		reciprocal = SIMD::MulPerElem(vrecpsq_f32(b, reciprocal), reciprocal);

		return SIMD::MulPerElem(a, reciprocal);
	}

	// Compute the absolute value per element
	static DESIRE_FORCE_INLINE float32x4_t AbsPerElem(float32x4_t vec)
	{
		return vabsq_f32(vec);
	}

	// Maximum of two vectors per element
	static DESIRE_FORCE_INLINE float32x4_t MaxPerElem(float32x4_t a, float32x4_t b)
	{
		return vmaxq_f32(a, b);
	}

	// Minimum of two vectors per element
	static DESIRE_FORCE_INLINE float32x4_t MinPerElem(float32x4_t a, float32x4_t b)
	{
		return vminq_f32(a, b);
	}

	// Get maximum element
	static DESIRE_FORCE_INLINE float32x4_t MaxElem3(float32x4_t vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static DESIRE_FORCE_INLINE float32x4_t MaxElem4(float32x4_t vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MaxPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Get minimum element
	static DESIRE_FORCE_INLINE float32x4_t MinElem3(float32x4_t vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static DESIRE_FORCE_INLINE float32x4_t MinElem4(float32x4_t vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MinPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Select packed single precision floating-point values from a and b using the mask
	static DESIRE_FORCE_INLINE float32x4_t Blend(float32x4_t a, float32x4_t b, uint32x4_t mask)
	{
		return vbslq_f32(mask, b, a);
	}

	// Shuffle
	static DESIRE_FORCE_INLINE float32x4_t Shuffle_XXXX(float32x4_t vec)
	{
		return __builtin_shuffle(vec, (uint32x4_t){ 0, 0, 0, 0 });
	}

	static DESIRE_FORCE_INLINE float32x4_t Shuffle_YYYY(float32x4_t vec)
	{
		return __builtin_shuffle(vec, (uint32x4_t){ 1, 1, 1, 1 });
	}

	static DESIRE_FORCE_INLINE float32x4_t Shuffle_ZZZZ(float32x4_t vec)
	{
		return __builtin_shuffle(vec, (uint32x4_t){ 2, 2, 2, 2 });
	}

	static DESIRE_FORCE_INLINE float32x4_t Shuffle_WWWW(float32x4_t vec)
	{
		return __builtin_shuffle(vec, (uint32x4_t){ 3, 3, 3, 3 });
	}
};

// --------------------------------------------------------------------------------------------------------------------
//	NEON Helper functions
// --------------------------------------------------------------------------------------------------------------------

static DESIRE_FORCE_INLINE float32x4_t newtonrapson_rsqrt4(float32x4_t vec)
{
	float32x4_t result = vrsqrteq_f32(vec);
	result = vmulq_f32(vrsqrtsq_f32(vmulq_f32(result, result), vec), result);
	result = vmulq_f32(vrsqrtsq_f32(vmulq_f32(result, result), vec), result);
	return result;
}
