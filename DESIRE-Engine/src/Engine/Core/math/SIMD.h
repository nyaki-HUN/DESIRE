#pragma once

#include "Engine/Core/platform.h"

#include <stdint.h>
#include <cmath>

#if defined(DESIRE_USE_SSE)
	// --------------------------------------------------------------------------------------------------------------------
	// Header files for SIMD intrinsics:
	//	<immintrin.h>	AVX			Intel(R) Architecture intrinsic functions
	//	<wmmintrin.h>	AES			Intel(R) AES and PCLMULQDQ intrinsics
	//	<ammintrin.h>	SSE4A		AMD-specific intrinsics
	//
	//	<nmmintrin.h>	SSE4.2
	//	<smmintrin.h>	SSE4.1
	//	<tmmintrin.h>	SSSE3
	//	<pmmintrin.h>	SSE3
	//	<emmintrin.h>	SSE2
	//	<xmmintrin.h>	SSE
	//	<mmintrin.h>	MMX
	// --------------------------------------------------------------------------------------------------------------------

	#if defined(__SSE4_1__)
		#include <smmintrin.h>
	#else
		// We are safe to use SSE3 because an x64 CPU with no SSE3 support is the 'first-generation' 64-bit which
		// isn't supported by Windows 8.1 x64 native due to the requirements for CMPXCHG16b, PrefetchW, and LAHF/SAHF
		#include <pmmintrin.h>

		static inline __m128 _mm_blendv_ps(__m128 a, __m128 b, __m128 mask)
		{
			return _mm_or_ps(_mm_and_ps(mask, b), _mm_andnot_ps(mask, a));
		}
	#endif

	typedef __m128	simd128_t;

#elif defined(__ARM_NEON__)

#define ARM_NEON_GCC_COMPATIBILITY 1
	#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
		#include <arm64_neon.h>
	#else
		#include <arm_neon.h>
	#endif

	typedef float32x4_t	simd128_t;

#else

	struct simd128_t
	{
		float x;
		float y;
		float z;
		float w;

		inline simd128_t() {}
		inline simd128_t(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	};
#endif

class SIMD
{
public:
	// Construct from x, y, z, and w elements
	static inline simd128_t Construct(float x, float y, float z, float w = 0.0f)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_setr_ps(x, y, z, w);
#elif defined(__ARM_NEON__)
		return (float32x4_t) { x, y, z, w };
#else
		return simd128_t(x, y, z, w);
#endif
	}

	// Construct by setting all elements to the same scalar value
	static inline simd128_t Construct(float scalar)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_set_ps1(scalar);
#elif defined(__ARM_NEON__)
		return vdupq_n_f32(scalar);
#else
		return SIMD::Construct(scalar, scalar, scalar, scalar);
#endif
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static inline simd128_t LoadXYZW(const float* fptr)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_loadu_ps(fptr);
#elif defined(__ARM_NEON__)
		return vld1q_f32(fptr);
#else
		return SIMD::Construct(fptr[0], fptr[1], fptr[2], fptr[3]);
#endif
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static inline void StoreXYZW(simd128_t vec, float* fptr)
	{
#if defined(DESIRE_USE_SSE)
		_mm_storeu_ps(fptr, vec);
#elif defined(__ARM_NEON__)
		vst1q_f32(fptr, vec);
#else
		fptr[0] = vec.x;
		fptr[1] = vec.y;
		fptr[2] = vec.z;
		fptr[3] = vec.w;
#endif
	}

	// Set element
	static inline simd128_t SetX(simd128_t vec, float x)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_move_ss(vec, _mm_set_ss(x));
#elif defined(__ARM_NEON__)
		return vsetq_lane_f32(x, vec, 0);
#else
		return SIMD::Construct(x, vec.y, vec.z, vec.w);
#endif
	}

	static inline simd128_t SetY(simd128_t vec, float y)
	{
#if defined(DESIRE_USE_SSE)
		const __m128 y000 = _mm_set_ss(y);
	#if defined(__SSE4_1__)
		return _mm_insert_ps(vec, y000, 0x10);
	#else
		__m128 yxzw = SIMD::Swizzle_YXZW(vec);
		yxzw = _mm_move_ss(yxzw, y000);
		return SIMD::Swizzle_YXZW(yxzw);
	#endif
#elif defined(__ARM_NEON__)
		return vsetq_lane_f32(y, vec, 1);
#else
		return SIMD::Construct(vec.x, y, vec.z, vec.w);
#endif
	}

	static inline simd128_t SetZ(simd128_t vec, float z)
	{
#if defined(DESIRE_USE_SSE)
		const __m128 z000 = _mm_set_ss(z);
	#if defined(__SSE4_1__)
		return _mm_insert_ps(vec, z000, 0x20);
	#else
		__m128 zyxw = SIMD::Swizzle_ZYXW(vec);
		zyxw = _mm_move_ss(zyxw, z000);
		return SIMD::Swizzle_ZYXW(zyxw);
	#endif
#elif defined(__ARM_NEON__)
		return vsetq_lane_f32(z, vec, 2);
#else
		return SIMD::Construct(vec.x, vec.y, z, vec.w);
#endif
	}

	static inline simd128_t SetW(simd128_t vec, float w)
	{
#if defined(DESIRE_USE_SSE)
		const __m128 w000 = _mm_set_ss(w);
	#if defined(__SSE4_1__)
		return _mm_insert_ps(vec, w000, 0x30);
	#else
		__m128 wyzx = SIMD::Swizzle_WYZX(vec);
		wyzx = _mm_move_ss(wyzx, w000);
		return SIMD::Swizzle_WYZX(wyzx);
	#endif
#elif defined(__ARM_NEON__)
		return vsetq_lane_f32(w, vec, 3);
#else
		return SIMD::Construct(vec.x, vec.y, vec.z, w);
#endif
	}

	// Get element
	static inline float GetX(simd128_t vec)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_cvtss_f32(vec);
#elif defined(__ARM_NEON__)
		return vgetq_lane_f32(vec, 0);
#else
		return vec.x;
#endif
	}

	static inline float GetY(simd128_t vec)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_cvtss_f32(SIMD::Swizzle_YYYY(vec));
#elif defined(__ARM_NEON__)
		return vgetq_lane_f32(vec, 1);
#else
		return vec.y;
#endif
	}

	static inline float GetZ(simd128_t vec)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_cvtss_f32(SIMD::Swizzle_ZZZZ(vec));
#elif defined(__ARM_NEON__)
		return vgetq_lane_f32(vec, 2);
#else
		return vec.z;
#endif
	}

	static inline float GetW(simd128_t vec)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_cvtss_f32(SIMD::Swizzle_WWWW(vec));
#elif defined(__ARM_NEON__)
		return vgetq_lane_f32(vec, 3);
#else
		return vec.w;
#endif
	}

	// Operator overloads
	static inline simd128_t Negate(simd128_t vec)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_xor_ps(vec, SIMD::Construct(-0.0f));
#elif defined(__ARM_NEON__)
		return vnegq_f32(vec);
#else
		return SIMD::Construct(-vec.x, -vec.y, -vec.z, -vec.w);
#endif
	}

	static inline simd128_t Add(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_add_ps(a, b);
#elif defined(__ARM_NEON__)
		return vaddq_f32(a, b); 
#else
		return SIMD::Construct(
			a.x + b.x,
			a.y + b.y,
			a.z + b.z,
			a.w + b.w
		);
#endif
	}

	static inline simd128_t Sub(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_sub_ps(a, b);
#elif defined(__ARM_NEON__)
		return vsubq_f32(a, b);
#else
		return SIMD::Construct(
			a.x - b.x,
			a.y - b.y,
			a.z - b.z,
			a.w - b.w
		);
#endif
	}

	static inline simd128_t Mul(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_mul_ps(a, b);
#elif defined(__ARM_NEON__)
		return vmulq_f32(a, b);
#else
		return SIMD::Construct(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z,
			a.w * b.w
		);
#endif
	}

	static inline simd128_t Mul(simd128_t vec, float scalar)
	{
#if defined(DESIRE_USE_SSE)
		return Mul(vec, SIMD::Construct(scalar));
#elif defined(__ARM_NEON__)
		return vmulq_n_f32(vec, scalar);
#else
		return SIMD::Construct(
			vec.x * scalar,
			vec.y * scalar,
			vec.z * scalar,
			vec.w * scalar
		);
#endif
	}

	static inline simd128_t MulAdd(simd128_t a, simd128_t b, simd128_t c)
	{
#if defined(__ARM_NEON__)
		return vmlaq_f32(c, a, b);
#else
		return SIMD::Add(c, SIMD::Mul(a, b));
#endif
	}

	static inline simd128_t MulSub(simd128_t a, simd128_t b, simd128_t c)
	{
#if defined(__ARM_NEON__)
		return vmlsq_f32(c, a, b);
#else
		return SIMD::Sub(c, SIMD::Mul(a, b));
#endif
	}

	static inline simd128_t Div(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_div_ps(a, b);
#elif defined(__ARM_NEON__)
		// Get an initial estimate then perform two Newton-Raphson iterations
		float32x4_t reciprocal = vrecpeq_f32(b);
		reciprocal = SIMD::Mul(vrecpsq_f32(b, reciprocal), reciprocal);
		reciprocal = SIMD::Mul(vrecpsq_f32(b, reciprocal), reciprocal);

		return SIMD::Mul(a, reciprocal);
#else
		return SIMD::Construct(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z,
			a.w / b.w
		);
#endif
	}

	// Comparison operators
	static inline bool OpCmpGT(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) & 0x7) == 0x7;
#elif defined(__ARM_NEON__)
//		uint32x4_t result = vcgtq_f32(a, b);
		return (
			a.GetX() > b.GetX() &&
			a.GetY() > b.GetY() &&
			a.GetZ() > b.GetZ()
		);
#else
		return (
			a.GetX() > b.GetX() &&
			a.GetY() > b.GetY() &&
			a.GetZ() > b.GetZ()
		);
#endif
	}

	static inline bool OpCmpLT(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) & 0x7) == 0x7;
#elif defined(__ARM_NEON__)
//		uint32x4_t result = vcltq_f32(a, b);
		return (
			a.GetX() < b.GetX() &&
			a.GetY() < b.GetY() &&
			a.GetZ() < b.GetZ()
		);
#else
		return (
			a.GetX() < b.GetX() &&
			a.GetY() < b.GetY() &&
			a.GetZ() < b.GetZ()
		);
#endif
	}

	static inline bool OpCmpGE(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0x7) == 0x7;
#elif defined(__ARM_NEON__)
//		uint32x4_t result = vcgeq_f32(a, b);
		return (
			a.GetX() >= b.GetX() &&
			a.GetY() >= b.GetY() &&
			a.GetZ() >= b.GetZ()
		);
#else
		return (
			a.GetX() >= b.GetX() &&
			a.GetY() >= b.GetY() &&
			a.GetZ() >= b.GetZ()
		);
#endif
	}

	static inline bool OpCmpLE(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0x7) == 0x7;
#elif defined(__ARM_NEON__)
//		uint32x4_t result = vcleq_f32(a, b);
		return (
			a.GetX() <= b.GetX() &&
			a.GetY() <= b.GetY() &&
			a.GetZ() <= b.GetZ()
		);
#else
		return (
			a.GetX() <= b.GetX() &&
			a.GetY() <= b.GetY() &&
			a.GetZ() <= b.GetZ()
		);
#endif
	}

	// Compute the dot product of two 3-D vectors
	static inline simd128_t Dot3(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
	#if defined(__SSE4_1__) && 0	// SSE4 dot product instruction isn't precise enough
		return _mm_dp_ps(a, b, 0x7F);
	#else
		__m128 ab = SIMD::Mul(a, b);
		alignas(16) static const uint32_t _mask3[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
		const __m128 mask3 = _mm_load_ps(reinterpret_cast<const float*>(_mask3));
		ab = _mm_and_ps(ab, mask3);
		__m128 xy_z_xy_z = _mm_hadd_ps(ab, ab);
		return _mm_hadd_ps(xy_z_xy_z, xy_z_xy_z);
	#endif
#elif defined(__ARM_NEON__)
		float32x4_t ab = SIMD::Mul(a, b);
		float32x2_t xy = vget_low_f32(ab);
		float32x2_t xy_xy = vpadd_f32(xy, xy);
		float32x2_t z_z = vdup_lane_f32(vget_high_f32(ab), 0);
		float32x2_t xyz_xyz = vadd_f32(xy_xy, z_z);
		return vcombine_f32(xyz_xyz, xyz_xyz);
#else
		return SIMD::Construct(a.x * b.x + a.y * b.y + a.z * b.z);
#endif
	}

	// Compute the dot product of two 4-D vectors
	static inline simd128_t Dot4(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
	#if defined(__SSE4_1__) && 0	// SSE4 dot product instruction isn't precise enough
		return _mm_dp_ps(a, b, 0xFF);
	#else
		__m128 ab = SIMD::Mul(a, b);
		__m128 xy_zw_xy_zw = _mm_hadd_ps(ab, ab);
		return _mm_hadd_ps(xy_zw_xy_zw, xy_zw_xy_zw);
	#endif
#elif defined(__ARM_NEON__)
		float32x4_t ab = SIMD::Mul(a, b);
		float32x2_t xz_yw = vadd_f32(vget_low_f32(ab), vget_high_f32(ab));
		float32x2_t xyzw_xyzw = vpadd_f32(xz_yw, xz_yw);
		return vcombine_f32(xyzw_xyzw, xyzw_xyzw);
#else
		return SIMD::Construct(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
#endif
	}

	// Compute cross product of two 3-D vectors
	static inline simd128_t Cross(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		__m128 yzx0 = SIMD::Swizzle_YZXW(a);
		__m128 yzx1 = SIMD::Swizzle_YZXW(b);
		__m128 result = SIMD::Sub(SIMD::Mul(yzx1, a), SIMD::Mul(yzx0, b));
		return SIMD::Swizzle_YZXW(result);
#elif defined(__ARM_NEON__)
		float32x2_t xy0 = vget_low_f32(a);
		float32x2_t xy1 = vget_low_f32(b);
		float32x2_t yzx0 = vcombine_f32(vext_f32(xy0, vget_high_f32(a), 1), xy0);
		float32x2_t yzx1 = vcombine_f32(vext_f32(xy1, vget_high_f32(b), 1), xy1);

		float32x2_t result = vmlsq_f32(SIMD::Mul(yzx1, a), yzx0, b);
		// form (Y, Z, X, _)
		xy1 = vget_low_f32(result);
		return vcombine_f32(vext_f32(xy1, vget_high_f32(result), 1), xy1);
#else
		return SIMD::Construct(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x,
			0.0f
		);
#endif
	}

	// Compute the absolute value per element
	static inline simd128_t AbsPerElem(simd128_t vec)
	{
#if defined(DESIRE_USE_SSE)
		const uint32_t mask = 0x7fffffff;
		return _mm_and_ps(vec, SIMD::Construct(*reinterpret_cast<const float*>(&mask)));
#elif defined(__ARM_NEON__)
		return vabsq_f32(vec);
#else
		return SIMD::Construct(
			std::abs(vec.x),
			std::abs(vec.y),
			std::abs(vec.z),
			std::abs(vec.w)
		);
#endif
	}

	// Maximum of two vectors per element
	static inline simd128_t MaxPerElem(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_max_ps(a, b);
#elif defined(__ARM_NEON__)
		return vmaxq_f32(a, b);
#else
		return SIMD::Construct(
			(a.x > b.x) ? a.x : b.x,
			(a.y > b.y) ? a.y : b.y,
			(a.z > b.z) ? a.z : b.z,
			(a.w > b.w) ? a.w : b.w
		);
#endif
	}

	// Minimum of two vectors per element
	static inline simd128_t MinPerElem(simd128_t a, simd128_t b)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_min_ps(a, b);
#elif defined(__ARM_NEON__)
		return vminq_f32(a, b);
#else
		return SIMD::Construct(
			(a.x < b.x) ? a.x : b.x,
			(a.y < b.y) ? a.y : b.y,
			(a.z < b.z) ? a.z : b.z,
			(a.w < b.w) ? a.w : b.w
		);
#endif
	}

	// Get maximum element
	static inline simd128_t MaxElem3(simd128_t vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static inline simd128_t MaxElem4(simd128_t vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MaxPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Get minimum element
	static inline simd128_t MinElem3(simd128_t vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static inline simd128_t MinElem4(simd128_t vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MinPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Compute the inverse/reciprocal square root
	static inline simd128_t InvSqrt(simd128_t vec)
	{
#if defined(DESIRE_USE_SSE)
		// Get an initial estimate then perform two Newton-Raphson iterations
		const __m128 invSqrt = _mm_rsqrt_ps(vec);
		const __m128 muls = SIMD::Mul(SIMD::Mul(vec, invSqrt), invSqrt);
		return SIMD::Mul(SIMD::Mul(invSqrt, 0.5f), SIMD::Sub(SIMD::Construct(3.0f), muls));
#elif defined(__ARM_NEON__)
		// Get an initial estimate then perform two Newton-Raphson iterations
		float32x4_t invSqrt = vrsqrteq_f32(vec);
		invSqrt = SIMD::Mul(vrsqrtsq_f32(SIMD::Mul(invSqrt, invSqrt), vec), result);
		invSqrt = SIMD::Mul(vrsqrtsq_f32(SIMD::Mul(invSqrt, invSqrt), vec), result);
		return invSqrt;
#else
		const float result = 1.0f / std::sqrt(vec.x);
		return SIMD::Construct(result);
#endif
	}

	// Blend (select) elements from a and b using the mask
	static inline simd128_t Blend(simd128_t a, simd128_t b, simd128_t mask)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_blendv_ps(a, b, mask);
#elif defined(__ARM_NEON__)
		return vbslq_f32(reinterpret_cast<uint32x4_t>(mask), b, a);
#else
#endif
	}

	// Special blends for one single precision floating-point value
	static inline simd128_t Blend_X(simd128_t to, simd128_t from)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_blend_ps(to, from, 0b0001);
#elif defined(__ARM_NEON__)
		return SIMD::Blend(to, from, (uint32x4_t) { 0xffffffff, 0, 0, 0 });
#else
		return SIMD::Construct(from.x, to.y, to.z, to.w);
#endif
	}

	static inline simd128_t Blend_Y(simd128_t to, simd128_t from)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_blend_ps(to, from, 0b0010);
#elif defined(__ARM_NEON__)
		return SIMD::Blend(to, from, (uint32x4_t) { 0, 0xffffffff, 0, 0 });
#else
		return SIMD::Construct(to.x, from.y, to.z, to.w);
#endif
	}

	static inline simd128_t Blend_Z(simd128_t to, simd128_t from)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_blend_ps(to, from, 0b0100);
#elif defined(__ARM_NEON__)
		return SIMD::Blend(to, from, (uint32x4_t) { 0, 0, 0xffffffff, 0 });
#else
		return SIMD::Construct(to.x, to.y, from.z, to.w);
#endif
	}

	static inline simd128_t Blend_W(simd128_t to, simd128_t from)
	{
#if defined(DESIRE_USE_SSE)
		return _mm_blend_ps(to, from, 0b1000);
#elif defined(__ARM_NEON__)
		return SIMD::Blend(to, from, (uint32x4_t) { 0, 0, 0, 0xffffffff });
#else
		return SIMD::Construct(to.x, to.y, to.z, from.w);
#endif
	}

	// Swizzle
#if defined(DESIRE_USE_SSE)
	static inline simd128_t Swizzle_XXXX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 0)); }
	static inline simd128_t Swizzle_XXYY(simd128_t vec)		{ return _mm_unpacklo_ps(vec, vec); }
	static inline simd128_t Swizzle_XXZZ(simd128_t vec)		{ return _mm_moveldup_ps(vec); }
	static inline simd128_t Swizzle_XXZW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 2, 0, 0)); }
	static inline simd128_t Swizzle_XYXX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 1, 0)); }
	static inline simd128_t Swizzle_XYXY(simd128_t vec)		{ return _mm_movelh_ps(vec, vec); }
	static inline simd128_t Swizzle_XYXW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 0, 1, 0)); }
	static inline simd128_t Swizzle_XYYX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 1, 0)); }
	static inline simd128_t Swizzle_XYZX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 2, 1, 0)); }
	static inline simd128_t Swizzle_XYWZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 1, 0)); }
	static inline simd128_t Swizzle_XZXX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 2, 0)); }
	static inline simd128_t Swizzle_XZXZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 0, 2, 0)); }
	static inline simd128_t Swizzle_XZYX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 2, 0)); }

	static inline simd128_t Swizzle_YXXX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 1)); }
	static inline simd128_t Swizzle_YXYX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 0, 1)); }
	static inline simd128_t Swizzle_YXZW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 2, 0, 1)); }
	static inline simd128_t Swizzle_YXWZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 0, 1)); }
	static inline simd128_t Swizzle_YYYY(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1)); }
	static inline simd128_t Swizzle_YYWW(simd128_t vec)		{ return _mm_movehdup_ps(vec); }
	static inline simd128_t Swizzle_YZXY(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 0, 2, 1)); }
	static inline simd128_t Swizzle_YZXW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 0, 2, 1)); }
	static inline simd128_t Swizzle_YZWX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 3, 2, 1)); }
	static inline simd128_t Swizzle_YWYW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 1, 3, 1)); }

	static inline simd128_t Swizzle_ZXXX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 2)); }
	static inline simd128_t Swizzle_ZXYZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 1, 0, 2)); }
	static inline simd128_t Swizzle_ZXYW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 1, 0, 2)); }
	static inline simd128_t Swizzle_ZXWY(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 3, 0, 2)); }
	static inline simd128_t Swizzle_ZYXW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 0, 1, 2)); }
	static inline simd128_t Swizzle_ZZYX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 2, 2)); }
	static inline simd128_t Swizzle_ZZZZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2)); }
	static inline simd128_t Swizzle_ZZWX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 3, 2, 2)); }
	static inline simd128_t Swizzle_ZZWW(simd128_t vec)		{ return _mm_unpackhi_ps(vec, vec); }
	static inline simd128_t Swizzle_ZWXX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 3, 2)); }
	static inline simd128_t Swizzle_ZWXY(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 0, 3, 2)); }
	static inline simd128_t Swizzle_ZWYX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 3, 2)); }
	static inline simd128_t Swizzle_ZWZW(simd128_t vec)		{ return _mm_movehl_ps(vec, vec); }
	static inline simd128_t Swizzle_ZWWZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 3, 2)); }

	static inline simd128_t Swizzle_WXYZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 1, 0, 3)); }
	static inline simd128_t Swizzle_WYZX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 2, 1, 3)); }
	static inline simd128_t Swizzle_WZXY(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 0, 2, 3)); }
	static inline simd128_t Swizzle_WZYX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 2, 3)); }
	static inline simd128_t Swizzle_WZWZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 2, 3)); }
	static inline simd128_t Swizzle_WWWW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 3, 3, 3)); }
#elif defined(__ARM_NEON__)
	static inline simd128_t Swizzle_XXXX(simd128_t vec)		{ return vdupq_lane_f32(vget_low_f32(vec), 0); }
	static inline simd128_t Swizzle_XXYY(simd128_t vec)		{ return vzipq_f32(vec, vec).val[0]; }
	static inline simd128_t Swizzle_XXZZ(simd128_t vec)		{ return vtrnq_f32(vec, vec).val[0]; }
	static inline simd128_t Swizzle_XXZW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 0, 2, 3 }); }
	static inline simd128_t Swizzle_XYXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 0, 0 }); }
	static inline simd128_t Swizzle_XYXY(simd128_t vec)		{ const float32x2_t xy = vget_low_f32(vec); return vcombine_f32(xy, xy); }
	static inline simd128_t Swizzle_XYXW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 0, 3 }); }
	static inline simd128_t Swizzle_XYYX(simd128_t vec)		{ const float32x2_t xy = vget_low_f32(vec); return vcombine_f32(xy, vrev64_f32(xy)); }
	static inline simd128_t Swizzle_XYZX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 2, 0 }); }
	static inline simd128_t Swizzle_XYWZ(simd128_t vec)		{ return vcombine_f32(vget_low_f32(vec), vrev64_f32(vget_high_f32(vec))); }
	static inline simd128_t Swizzle_XZXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 2, 0, 0 }); }
	static inline simd128_t Swizzle_XZXZ(simd128_t vec)		{ return vuzpq_f32(vec, vec).val[0]; }
	static inline simd128_t Swizzle_XZYX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 2, 1, 0 }); }

	static inline simd128_t Swizzle_YXXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 1, 0, 0, 0 }); }
	static inline simd128_t Swizzle_YXYX(simd128_t vec)		{ const float32x2_t yx = vrev64_f32(vget_low_f32(vec)); return vcombine_f32(yx, yx); }
	static inline simd128_t Swizzle_YXZW(simd128_t vec)		{ return vcombine_f32(vrev64_f32(vget_low_f32(vec)), vget_high_f32(vec)); }
	static inline simd128_t Swizzle_YXWZ(simd128_t vec)		{ return vrev64q_f32(vec); }
	static inline simd128_t Swizzle_YYYY(simd128_t vec)		{ return vdupq_lane_f32(vget_low_f32(vec), 1); }
	static inline simd128_t Swizzle_YYWW(simd128_t vec)		{ return vtrnq_f32(vec, vec).val[1]; }
	static inline simd128_t Swizzle_YZXY(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 1, 2, 0, 1 }); }
	static inline simd128_t Swizzle_YZXW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 1, 2, 0, 3 }); }
	static inline simd128_t Swizzle_YZWX(simd128_t vec)		{ return vextq_f32(vec, vec, 1); }
	static inline simd128_t Swizzle_YWYW(simd128_t vec)		{ return vuzpq_f32(vec, vec).val[1]; }

	static inline simd128_t Swizzle_ZXXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 0, 0 }); }
	static inline simd128_t Swizzle_ZXYZ(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 1, 2 }); }
	static inline simd128_t Swizzle_ZXYW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 1, 3 }); }
	static inline simd128_t Swizzle_ZXWY(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 3, 1 }); }
	static inline simd128_t Swizzle_ZZYX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 2, 1, 0 }); }
	static inline simd128_t Swizzle_ZZZZ(simd128_t vec)		{ return vdupq_lane_f32(vget_high_f32(vec), 0); }
	static inline simd128_t Swizzle_ZZWX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 2, 3, 0 }); }
	static inline simd128_t Swizzle_ZZWW(simd128_t vec)		{ return vzipq_f32(vec, vec).val[1]; }
	static inline simd128_t Swizzle_ZWXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 3, 0, 0 }); }
	static inline simd128_t Swizzle_ZWXY(simd128_t vec)		{ return vextq_f32(vec, vec, 2); }
	static inline simd128_t Swizzle_ZWYX(simd128_t vec)		{ return vcombine_f32(vget_high_f32(vec), vrev64_f32(vget_low_f32(vec))); }
	static inline simd128_t Swizzle_ZWZW(simd128_t vec)		{ const float32x2_t zw = vget_high_f32(vec); return vcombine_f32(zw, zw); }
	static inline simd128_t Swizzle_ZWWZ(simd128_t vec)		{ const float32x2_t zw = vget_high_f32(vec); return vcombine_f32(zw, vrev64_f32(zw)); }

	static inline simd128_t Swizzle_WXYZ(simd128_t vec)		{ return vextq_f32(vec, vec, 3); }
	static inline simd128_t Swizzle_WZXY(simd128_t vec)		{ return vcombine_f32(vrev64_f32(vget_high_f32(vec)), vget_low_f32(vec)); }
	static inline simd128_t Swizzle_WZYX(simd128_t vec)		{ return Swizzle_ZWXY(Swizzle_YXWZ(vec)); }
	static inline simd128_t Swizzle_WZWZ(simd128_t vec)		{ const float32x2_t wz = vrev64_f32(vget_high_f32(vec)); return vcombine_f32(wz, wz); }
	static inline simd128_t Swizzle_WWWW(simd128_t vec)		{ return vdupq_lane_f32(vget_high_f32(vec), 1); }
#else
	static inline simd128_t Swizzle_XXXX(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.x, vec.x, vec.x); }
	static inline simd128_t Swizzle_XXYY(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.x, vec.y, vec.y); }
	static inline simd128_t Swizzle_XXZZ(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.x, vec.z, vec.z); }
	static inline simd128_t Swizzle_XXZW(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.x, vec.z, vec.w); }
	static inline simd128_t Swizzle_XYXX(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.y, vec.x, vec.x); }
	static inline simd128_t Swizzle_XYXY(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.y, vec.x, vec.y); }
	static inline simd128_t Swizzle_XYXW(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.y, vec.x, vec.w); }
	static inline simd128_t Swizzle_XYYX(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.y, vec.y, vec.x); }
	static inline simd128_t Swizzle_XYZX(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.y, vec.z, vec.x); }
	static inline simd128_t Swizzle_XYWZ(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.y, vec.w, vec.z); }
	static inline simd128_t Swizzle_XZXX(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.z, vec.x, vec.x); }
	static inline simd128_t Swizzle_XZXZ(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.z, vec.x, vec.z); }
	static inline simd128_t Swizzle_XZYX(simd128_t vec)		{ return SIMD::Construct(vec.x, vec.z, vec.y, vec.x); }

	static inline simd128_t Swizzle_YXXX(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.x, vec.x, vec.x); }
	static inline simd128_t Swizzle_YXYX(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.x, vec.y, vec.x); }
	static inline simd128_t Swizzle_YXZW(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.x, vec.z, vec.w); }
	static inline simd128_t Swizzle_YXWZ(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.x, vec.w, vec.z); }
	static inline simd128_t Swizzle_YYYY(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.y, vec.y, vec.y); }
	static inline simd128_t Swizzle_YYWW(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.y, vec.w, vec.w); }
	static inline simd128_t Swizzle_YZXY(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.z, vec.x, vec.y); }
	static inline simd128_t Swizzle_YZXW(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.z, vec.x, vec.w); }
	static inline simd128_t Swizzle_YZWX(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.z, vec.w, vec.x); }
	static inline simd128_t Swizzle_YWYW(simd128_t vec)		{ return SIMD::Construct(vec.y, vec.w, vec.y, vec.w); }

	static inline simd128_t Swizzle_ZXXX(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.x, vec.x, vec.x); }
	static inline simd128_t Swizzle_ZXYZ(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.x, vec.y, vec.z); }
	static inline simd128_t Swizzle_ZXYW(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.x, vec.y, vec.w); }
	static inline simd128_t Swizzle_ZXWY(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.x, vec.w, vec.y); }
	static inline simd128_t Swizzle_ZZYX(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.z, vec.y, vec.x); }
	static inline simd128_t Swizzle_ZZZZ(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.z, vec.z, vec.z); }
	static inline simd128_t Swizzle_ZZWX(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.z, vec.w, vec.x); }
	static inline simd128_t Swizzle_ZZWW(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.z, vec.w, vec.w); }
	static inline simd128_t Swizzle_ZWXX(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.w, vec.x, vec.x); }
	static inline simd128_t Swizzle_ZWXY(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.w, vec.x, vec.y); }
	static inline simd128_t Swizzle_ZWYX(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.w, vec.y, vec.x); }
	static inline simd128_t Swizzle_ZWZW(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.w, vec.z, vec.w); }
	static inline simd128_t Swizzle_ZWWZ(simd128_t vec)		{ return SIMD::Construct(vec.z, vec.w, vec.w, vec.z); }

	static inline simd128_t Swizzle_WXYZ(simd128_t vec)		{ return SIMD::Construct(vec.w, vec.x, vec.y, vec.z); }
	static inline simd128_t Swizzle_WZXY(simd128_t vec)		{ return SIMD::Construct(vec.w, vec.z, vec.x, vec.y); }
	static inline simd128_t Swizzle_WZYX(simd128_t vec)		{ return SIMD::Construct(vec.w, vec.z, vec.y, vec.x); }
	static inline simd128_t Swizzle_WZWZ(simd128_t vec)		{ return SIMD::Construct(vec.w, vec.z, vec.w, vec.z); }
	static inline simd128_t Swizzle_WWWW(simd128_t vec)		{ return SIMD::Construct(vec.w, vec.w, vec.w, vec.w); }
#endif
};

// --------------------------------------------------------------------------------------------------------------------
//	Helper functions
// --------------------------------------------------------------------------------------------------------------------

#if defined(DESIRE_USE_SSE)

#define _mm_ror_ps(vec, i)	_mm_shuffle_ps(vec, vec, _MM_SHUFFLE((uint8_t)(i + 3) % 4,(uint8_t)(i + 2) % 4,(uint8_t)(i + 1) % 4,(uint8_t)(i + 0) % 4))

static inline __m128 toM128(uint32_t x)
{
	return SIMD::Construct(*(float*)&x);
}

static inline __m128 acosf4(__m128 x)
{
	__m128 xabs = SIMD::AbsPerElem(x);
	__m128 select = _mm_cmplt_ps(x, _mm_setzero_ps());
	__m128 t1 = _mm_sqrt_ps(SIMD::Sub(SIMD::Construct(1.0f), xabs));

	/* Instruction counts can be reduced if the polynomial was
	* computed entirely from nested (dependent) fma's. However,
	* to reduce the number of pipeline stalls, the polygon is evaluated
	* in two halves (hi amd lo).
	*/
	__m128 xabs2 = SIMD::Mul(xabs, xabs);
	__m128 xabs4 = SIMD::Mul(xabs2, xabs2);
	__m128 hi = SIMD::MulAdd(SIMD::MulAdd(SIMD::MulAdd(SIMD::Construct(-0.0012624911f),
		xabs, SIMD::Construct(0.0066700901f)),
		xabs, SIMD::Construct(-0.0170881256f)),
		xabs, SIMD::Construct(0.0308918810f));
	__m128 lo = SIMD::MulAdd(SIMD::MulAdd(SIMD::MulAdd(SIMD::Construct(-0.0501743046f),
		xabs, SIMD::Construct(0.0889789874f)),
		xabs, SIMD::Construct(-0.2145988016f)),
		xabs, SIMD::Construct(1.5707963050f));

	__m128 result = SIMD::MulAdd(hi, xabs4, lo);

	// Adjust the result if x is negactive.
	return SIMD::Blend(
		SIMD::Mul(t1, result),									// Positive
		SIMD::MulSub(t1, result, SIMD::Construct(3.1415926535898f)),	// Negative
		select);
}

static inline __m128 sinf4(__m128 x)
{
	// Common constants used to evaluate sinf4/cosf4/tanf4
#define _SINCOS_CC0  -0.0013602249f
#define _SINCOS_CC1   0.0416566950f
#define _SINCOS_CC2  -0.4999990225f
#define _SINCOS_SC0  -0.0001950727f
#define _SINCOS_SC1   0.0083320758f
#define _SINCOS_SC2  -0.1666665247f

#define _SINCOS_KC1  1.57079625129f
#define _SINCOS_KC2  7.54978995489e-8f

	__m128 xl, xl2, xl3, res;

	// Range reduction using : xl = angle * TwoOverPi;
	xl = SIMD::Mul(x, SIMD::Construct(0.63661977236f));

	// Find the quadrant the angle falls in
	// using:  q = (int)(ceil(abs(xl)) * sign(xl))
	const __m128i q = _mm_cvtps_epi32(xl);

	// Compute the offset based on the quadrant that the angle falls in
	const __m128 offset = _mm_and_ps(_mm_castsi128_ps(q), toM128(0x3));

	// Remainder in range [-pi/4..pi/4]
	__m128 qf = _mm_cvtepi32_ps(q);
	xl = SIMD::MulSub(qf, SIMD::Construct(_SINCOS_KC2), SIMD::MulSub(qf, SIMD::Construct(_SINCOS_KC1), x));

	// Compute x^2 and x^3
	xl2 = SIMD::Mul(xl, xl);
	xl3 = SIMD::Mul(xl2, xl);

	// Compute both the sin and cos of the angles
	// using a polynomial expression:
	//   cx = 1.0f + xl2 * ((C0 * xl2 + C1) * xl2 + C2), and
	//   sx = xl + xl3 * ((S0 * xl2 + S1) * xl2 + S2)
	__m128 cx =
		SIMD::MulAdd(
			SIMD::MulAdd(
				SIMD::MulAdd(SIMD::Construct(_SINCOS_CC0), xl2, SIMD::Construct(_SINCOS_CC1)), xl2, SIMD::Construct(_SINCOS_CC2)), xl2, SIMD::Construct(1.0f));
	__m128 sx =
		SIMD::MulAdd(
			SIMD::MulAdd(
				SIMD::MulAdd(SIMD::Construct(_SINCOS_SC0), xl2, SIMD::Construct(_SINCOS_SC1)), xl2, SIMD::Construct(_SINCOS_SC2)), xl3, xl);

	// Use the cosine when the offset is odd and the sin
	// when the offset is even
	res = SIMD::Blend(cx, sx, _mm_cmpeq_ps(_mm_and_ps(offset, toM128(0x1)), _mm_setzero_ps()));

	// Flip the sign of the result when (offset mod 4) = 1 or 2
	return SIMD::Blend(
		_mm_xor_ps(toM128(0x80000000U), res),	// Negative
		res,									// Positive
		_mm_cmpeq_ps(_mm_and_ps(offset, toM128(0x2)), _mm_setzero_ps()));
}

static inline void sincosf4(__m128 x, __m128 *s, __m128 *c)
{
	__m128 xl, xl2, xl3;

	// Range reduction using : xl = angle * TwoOverPi;
	xl = SIMD::Mul(x, SIMD::Construct(0.63661977236f));

	// Find the quadrant the angle falls in
	// using:  q = (int)(ceil(abs(xl)) * sign(xl))
//	const __m128i q = _mm_cvtps_epi32(SIMD::Add(xl, SIMD::Blend(SIMD::Construct(0.5f), xl, 0x80000000)));
	const __m128i q = _mm_cvtps_epi32(xl);

	// Compute the offset based on the quadrant that the angle falls in.
	// Add 1 to the offset for the cosine. 
	const __m128 offsetSin = _mm_and_ps(_mm_castsi128_ps(q), toM128(0x3));
	const __m128 offsetCos = _mm_castsi128_ps(_mm_add_epi32(_mm_set1_epi32(1), (__m128i&)offsetSin));

	// Remainder in range [-pi/4..pi/4]
	__m128 qf = _mm_cvtepi32_ps(q);
	xl = SIMD::MulSub(qf, SIMD::Construct(_SINCOS_KC2), SIMD::MulSub(qf, SIMD::Construct(_SINCOS_KC1), x));

	// Compute x^2 and x^3
	xl2 = SIMD::Mul(xl, xl);
	xl3 = SIMD::Mul(xl2, xl);

	// Compute both the sin and cos of the angles
	// using a polynomial expression:
//   cx = 1.0f + xl2 * ((C0 * xl2 + C1) * xl2 + C2), and
//   sx = xl + xl3 * ((S0 * xl2 + S1) * xl2 + S2)
	__m128 cx =
		SIMD::MulAdd(
			SIMD::MulAdd(
				SIMD::MulAdd(SIMD::Construct(_SINCOS_CC0), xl2, SIMD::Construct(_SINCOS_CC1)), xl2, SIMD::Construct(_SINCOS_CC2)), xl2, SIMD::Construct(1.0f));
	__m128 sx =
		SIMD::MulAdd(
			SIMD::MulAdd(
				SIMD::MulAdd(SIMD::Construct(_SINCOS_SC0), xl2, SIMD::Construct(_SINCOS_SC1)), xl2, SIMD::Construct(_SINCOS_SC2)), xl3, xl);

	// Use the cosine when the offset is odd and the sin
	// when the offset is even
	__m128 sinMask = _mm_cmpeq_ps(_mm_and_ps(offsetSin, toM128(0x1)), _mm_setzero_ps());
	__m128 cosMask = _mm_cmpeq_ps(_mm_and_ps(offsetCos, toM128(0x1)), _mm_setzero_ps());
	*s = SIMD::Blend(cx, sx, sinMask);
	*c = SIMD::Blend(cx, sx, cosMask);

	// Flip the sign of the result when (offset mod 4) = 1 or 2
	sinMask = _mm_cmpeq_ps(_mm_and_ps(offsetSin, toM128(0x2)), _mm_setzero_ps());
	cosMask = _mm_cmpeq_ps(_mm_and_ps(offsetCos, toM128(0x2)), _mm_setzero_ps());

	*s = SIMD::Blend(_mm_xor_ps(toM128(0x80000000), *s), *s, sinMask);
	*c = SIMD::Blend(_mm_xor_ps(toM128(0x80000000), *c), *c, cosMask);
}

#endif
