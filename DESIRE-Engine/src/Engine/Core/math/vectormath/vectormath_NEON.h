#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	NEON vector types
// --------------------------------------------------------------------------------------------------------------------

#define ARM_NEON_GCC_COMPATIBILITY 1
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
	#include <arm64_neon.h>
#else
	#include <arm_neon.h>
#endif

typedef float32x4_t	vec_float3_t;
typedef float32x4_t	vec_float4_t;

// --------------------------------------------------------------------------------------------------------------------
//	NEON implementation of the SIMD functions
// --------------------------------------------------------------------------------------------------------------------

class SIMD
{
public:
	// Construct from x, y, z, and w elements
	static inline float32x4_t Construct(float x, float y, float z, float w = 0.0f)
	{
		return (float32x4_t){ x, y, z, w };
	}

	// Construct by setting all elements to the same scalar value
	static inline void Construct(float32x4_t& vec, float scalar)
	{
		vec = vdupq_n_f32(scalar);
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static inline void LoadXYZW(float32x4_t& vec, const float *fptr)
	{
		vec = vld1q_f32(fptr);
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static inline void StoreXYZW(float32x4_t vec, float *fptr)
	{
		vst1q_f32(fptr, vec);
	}

	// Set element
	static inline void SetX(float32x4_t& vec, float x)		{ vec = vsetq_lane_f32(x, vec, 0); }
	static inline void SetY(float32x4_t& vec, float y)		{ vec = vsetq_lane_f32(y, vec, 1); }
	static inline void SetZ(float32x4_t& vec, float z)		{ vec = vsetq_lane_f32(z, vec, 2); }
	static inline void SetZ(float32x4_t& vec, float w)		{ vec = vsetq_lane_f32(w, vec, 3); }

	// Get element
	static inline float GetX(float32x4_t vec)				{ return vgetq_lane_f32(vec, 0); }
	static inline float GetY(float32x4_t vec)				{ return vgetq_lane_f32(vec, 1); }
	static inline float GetZ(float32x4_t vec)				{ return vgetq_lane_f32(vec, 2); }
	static inline float GetW(float32x4_t vec)				{ return vgetq_lane_f32(vec, 3); }

	// Operator overloads
	static inline float32x4_t Negate(float32x4_t vec)
	{
		return vnegq_f32(vec);
	}

	static inline float32x4_t Add(float32x4_t a, float32x4_t b)
	{
		return vaddq_f32(a, b);
	}

	static inline float32x4_t Sub(float32x4_t a, float32x4_t b)
	{
		return vsubq_f32(a, b);
	}

	static inline float32x4_t Mul(float32x4_t vec, float scalar)
	{
		return vmulq_n_f32(vec, scalar);
	}

	static inline float32x4_t Mul(float32x4_t a, float32x4_t b)
	{
		return vmulq_f32(a, b);
	}

	static inline float32x4_t MulAdd(float32x4_t a, float32x4_t b, float32x4_t c)
	{
		return vmlaq_f32(c, a, b);
	}
	
	static inline float32x4_t MulSub(float32x4_t a, float32x4_t b, float32x4_t c)
	{
		return vmlsq_f32(c, a, b);
	}

	static inline float32x4_t Div(float32x4_t a, float32x4_t b)
	{
		// Get an initial estimate of 1/vec
		float32x4_t reciprocal = vrecpeq_f32(b);
		// Use a couple Newton-Raphson steps to refine the estimate
		reciprocal = SIMD::Mul(vrecpsq_f32(b, reciprocal), reciprocal);
		reciprocal = SIMD::Mul(vrecpsq_f32(b, reciprocal), reciprocal);

		return SIMD::Mul(a, reciprocal);
	}

	// Comparison operators
	static inline bool OpCmpGE(float32x4_t a, float32x4_t b)
	{
//		uint32x4_t result = vcgeq_f32(a, b);
		return (
			a.GetX() >= b.GetX() &&
			a.GetY() >= b.GetY() &&
			a.GetZ() >= b.GetZ()
		);
	}

	static inline bool OpCmpLE(float32x4_t a, float32x4_t b)
	{
//		uint32x4_t result = vcleq_f32(a, b);
		return (
			a.GetX() <= b.GetX() &&
			a.GetY() <= b.GetY() &&
			a.GetZ() <= b.GetZ()
		);
	}

	// Compute the dot product of two 3-D vectors
	static inline float32x4_t Dot3(float32x4_t a, float32x4_t b)
	{
		float32x4_t vd = SIMD::Mul(a, b);
		float32x2_t x = vpadd_f32(vget_low_f32(vd), vget_low_f32(vd));
		return vadd_f32(x, vget_high_f32(vd));
	}

	// Compute the dot product of two 4-D vectors
	static inline float32x4_t Dot4(float32x4_t a, float32x4_t b)
	{
		float32x4_t vd = SIMD::Mul(a, b);
		float32x2_t x = vpadd_f32(vget_low_f32(vd), vget_high_f32(vd));
		return vpadd_f32(x, x);
	}

	// Compute cross product of two 3-D vectors
	static inline float32x4_t Cross(float32x4_t a, float32x4_t b)
	{
		float32x2_t xy0 = vget_low_f32(a);
		float32x2_t xy1 = vget_low_f32(b);
		float32x2_t yzx0 = vcombine_f32(vext_f32(xy0, vget_high_f32(a), 1), xy0);
		float32x2_t yzx1 = vcombine_f32(vext_f32(xy1, vget_high_f32(b), 1), xy1);

		float32x2_t result = vmlsq_f32(SIMD::Mul(yzx1, a), yzx0, b);
		// form (Y, Z, X, _)
		xy1 = vget_low_f32(result);
		return vcombine_f32(vext_f32(xy1, vget_high_f32(result), 1), xy1);
	}

	// Compute the absolute value per element
	static inline float32x4_t AbsPerElem(float32x4_t vec)
	{
		return vabsq_f32(vec);
	}

	// Maximum of two vectors per element
	static inline float32x4_t MaxPerElem(float32x4_t a, float32x4_t b)
	{
		return vmaxq_f32(a, b);
	}

	// Minimum of two vectors per element
	static inline float32x4_t MinPerElem(float32x4_t a, float32x4_t b)
	{
		return vminq_f32(a, b);
	}

	// Get maximum element
	static inline float32x4_t MaxElem3(float32x4_t vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static inline float32x4_t MaxElem4(float32x4_t vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MaxPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Get minimum element
	static inline float32x4_t MinElem3(float32x4_t vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static inline float32x4_t MinElem4(float32x4_t vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MinPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Blend (select) elements from a and b using the mask
	static inline float32x4_t Blend(float32x4_t a, float32x4_t b, uint32x4_t mask)
	{
		return vbslq_f32(mask, b, a);
	}

	// Special blends for one single precision floating-point value
	static inline float32x4_t Blend_X(float32x4_t to, float32x4_t from)		{ return SIMD::Blend(to, from, (uint32x4_t){ 0xffffffff, 0, 0, 0 }); }
	static inline float32x4_t Blend_Y(float32x4_t to, float32x4_t from)		{ return SIMD::Blend(to, from, (uint32x4_t){ 0, 0xffffffff, 0, 0 }); }
	static inline float32x4_t Blend_Z(float32x4_t to, float32x4_t from)		{ return SIMD::Blend(to, from, (uint32x4_t){ 0, 0, 0xffffffff, 0 }); }
	static inline float32x4_t Blend_W(float32x4_t to, float32x4_t from)		{ return SIMD::Blend(to, from, (uint32x4_t){ 0, 0, 0, 0xffffffff }); }

	// Swizzle
	static inline float32x4_t Swizzle_XXXX(float32x4_t vec)		{ return vdupq_lane_f32(vget_low_f32(vec), 0); }
	static inline float32x4_t Swizzle_XXYY(float32x4_t vec)		{ return vzipq_f32(vec, vec).val[0]; }
	static inline float32x4_t Swizzle_XXZZ(float32x4_t vec)		{ return vtrnq_f32(vec, vec).val[0]; }
	static inline float32x4_t Swizzle_XXZW(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 0, 2, 3 }); }
	static inline float32x4_t Swizzle_XYXY(float32x4_t vec)		{ const float32x2_t v = vget_low_f32(vec); return vcombine_f32(v, v); }
	static inline float32x4_t Swizzle_XYXW(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 0, 3 }); }
	static inline float32x4_t Swizzle_XYYX(float32x4_t vec)		{ const float32x2_t v = vget_low_f32(vec); return vcombine_f32(v, vrev64_f32(v)); }
	static inline float32x4_t Swizzle_XYZX(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 2, 0 }); }
	static inline float32x4_t Swizzle_XYWZ(float32x4_t vec)		{ return vcombine_f32(vget_low_f32(vec), vrev64_f32(vget_high_f32(vec))); }
	static inline float32x4_t Swizzle_XZXX(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 2, 0, 0 }); }
	static inline float32x4_t Swizzle_XZXZ(float32x4_t vec)		{ return vuzpq_f32(vec, vec).val[0]; }

	static inline float32x4_t Swizzle_YXXX(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 1, 0, 0, 0 }); }
	static inline float32x4_t Swizzle_YXYX(float32x4_t vec)		{ const float32x2_t v = vrev64_f32(vget_low_f32(vec)); return vcombine_f32(v, v); }
	static inline float32x4_t Swizzle_YXZW(float32x4_t vec)		{ return vcombine_f32(vrev64_f32(vget_low_f32(vec)), vget_high_f32(vec)); }
	static inline float32x4_t Swizzle_YXWZ(float32x4_t vec)		{ return vcombine_f32(vrev64_f32(vget_low_f32(vec)), vrev64_f32(vget_high_f32(vec))); }
	static inline float32x4_t Swizzle_YYYY(float32x4_t vec)		{ return vdupq_lane_f32(vget_low_f32(vec), 1); }
	static inline float32x4_t Swizzle_YYWW(float32x4_t vec)		{ return vtrnq_f32(vec, vec).val[1]; }
	static inline float32x4_t Swizzle_YZXW(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 1, 2, 0, 3 }); }
	static inline float32x4_t Swizzle_YZWX(float32x4_t vec)		{ return vextq_f32(vec, vec, 1); }
	static inline float32x4_t Swizzle_YWYW(float32x4_t vec)		{ return vuzpq_f32(vec, vec).val[1]; }

	static inline float32x4_t Swizzle_ZXYW(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 1, 3 }); }
	static inline float32x4_t Swizzle_ZXWY(float32x4_t vec)		{ return vrev64q_f32(vec); }
	static inline float32x4_t Swizzle_ZZZZ(float32x4_t vec)		{ return vdupq_lane_f32(vget_high_f32(vec), 0); }
	static inline float32x4_t Swizzle_ZZWX(float32x4_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 2, 3, 0 }); }
	static inline float32x4_t Swizzle_ZZWW(float32x4_t vec)		{ return vzipq_f32(vec, vec).val[1]; }
	static inline float32x4_t Swizzle_ZWXY(float32x4_t vec)		{ return vextq_f32(vec, vec, 2); }
	static inline float32x4_t Swizzle_ZWYX(float32x4_t vec)		{ return vcombine_f32(vget_high_f32(vec), vrev64_f32(vget_low_f32(vec))); }
	static inline float32x4_t Swizzle_ZWZW(float32x4_t vec)		{ const float32x2_t v = vget_high_f32(vec); return vcombine_f32(v, v); }
	static inline float32x4_t Swizzle_ZWWZ(float32x4_t vec)		{ const float32x2_t v = vget_high_f32(vec); return vcombine_f32(v, vrev64_f32(v)); }

	static inline float32x4_t Swizzle_WXYZ(float32x4_t vec)		{ return vextq_f32(vec, vec, 3); }
	static inline float32x4_t Swizzle_WZXY(float32x4_t vec)		{ return vcombine_f32(vrev64_f32(vget_high_f32(vec)), vget_low_f32(vec)); }
	static inline float32x4_t Swizzle_WZYX(float32x4_t vec)		{ return vcombine_f32(vrev64_f32(vget_high_f32(vec)), vrev64_f32(vget_low_f32(vec))); }
	static inline float32x4_t Swizzle_WZWZ(float32x4_t vec)		{ const float32x2_t v = vrev64_f32(vget_high_f32(vec)); return vcombine_f32(v, v); }
	static inline float32x4_t Swizzle_WWWW(float32x4_t vec)		{ return vdupq_lane_f32(vget_high_f32(vec), 1); }
};

// --------------------------------------------------------------------------------------------------------------------
//	NEON Helper functions
// --------------------------------------------------------------------------------------------------------------------

static inline float32x4_t newtonrapson_rsqrt4(float32x4_t vec)
{
	float32x4_t result = vrsqrteq_f32(vec);
	result = SIMD::Mul(vrsqrtsq_f32(SIMD::Mul(result, result), vec), result);
	result = SIMD::Mul(vrsqrtsq_f32(SIMD::Mul(result, result), vec), result);
	return result;
}
