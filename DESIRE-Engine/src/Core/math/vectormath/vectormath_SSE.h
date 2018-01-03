#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	SSE vector types
// --------------------------------------------------------------------------------------------------------------------

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

	static DESIRE_FORCE_INLINE __m128 _mm_blendv_ps(__m128 a, __m128 b, __m128 mask)
	{
		return _mm_or_ps(_mm_and_ps(mask, b), _mm_andnot_ps(mask, a));
	}
#endif

typedef __m128	vec_float3_t;
typedef __m128	vec_float4_t;

static DESIRE_FORCE_INLINE __m128 toM128(uint32_t x)
{
	return _mm_set1_ps(*(float*)&x);
}


// --------------------------------------------------------------------------------------------------------------------
//	SSE implementation of the SIMD functions
// --------------------------------------------------------------------------------------------------------------------

class SIMD
{
public:
	// Construct from x, y, z, and w elements
	static DESIRE_FORCE_INLINE void Construct(__m128& vec, float x, float y, float z, float w = 0.0f)
	{
		vec = _mm_setr_ps(x, y, z, w);
	}

	// Construct by setting all elements to the same scalar value
	static DESIRE_FORCE_INLINE void Construct(__m128& vec, float scalar)
	{
		vec = _mm_set1_ps(scalar);
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static DESIRE_FORCE_INLINE void LoadXYZW(__m128& vec, const float *fptr)
	{
		vec = _mm_castsi128_ps(_mm_lddqu_si128((const __m128i*)fptr));
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static DESIRE_FORCE_INLINE void StoreXYZW(__m128 vec, float *fptr)
	{
		_mm_storeu_ps(fptr, vec);
	}

	// Set the x, y, and z elements (does not change the w element)
	static DESIRE_FORCE_INLINE void SetXYZ(__m128& vec, __m128 xyz)
	{
		alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
		const __m128 mask_w = _mm_load_ps((float*)select_w);
		vec = SIMD::Blend(xyz, vec, mask_w);
	}

	// Set element
	static DESIRE_FORCE_INLINE void SetX(__m128& vec, float x)
	{
		vec = _mm_move_ss(vec, _mm_set_ss(x));
	}

	static DESIRE_FORCE_INLINE void SetY(__m128& vec, float y)
	{
		__m128 t = _mm_move_ss(vec, _mm_set_ss(y));
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
		vec = _mm_move_ss(t, vec);
	}

	static DESIRE_FORCE_INLINE void SetZ(__m128& vec, float z)
	{
		__m128 t = _mm_move_ss(vec, _mm_set_ss(z));
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
		vec = _mm_move_ss(t, vec);
	}

	static DESIRE_FORCE_INLINE void SetW(__m128& vec, float w)
	{
		__m128 t = _mm_move_ss(vec, _mm_set_ss(w));
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 2, 1, 0));
		vec = _mm_move_ss(t, vec);
	}

	// Get element
	static DESIRE_FORCE_INLINE float GetX(__m128 vec)		{ return _mm_cvtss_f32(vec); }
	static DESIRE_FORCE_INLINE float GetY(__m128 vec)		{ return _mm_cvtss_f32(SIMD::Shuffle_YYYY(vec)); }
	static DESIRE_FORCE_INLINE float GetZ(__m128 vec)		{ return _mm_cvtss_f32(SIMD::Shuffle_ZZZZ(vec)); }
	static DESIRE_FORCE_INLINE float GetW(__m128 vec)		{ return _mm_cvtss_f32(SIMD::Shuffle_WWWW(vec)); }

	// Compute the conjugate of a quaternion
	static DESIRE_FORCE_INLINE __m128 Conjugate(__m128 quat)
	{
		alignas(16) const uint32_t mask[4] = { 0x80000000, 0x80000000, 0x80000000, 0 };
		return _mm_xor_ps(quat, _mm_load_ps((float*)mask));
	}

	// Operator overloads
	static DESIRE_FORCE_INLINE __m128 Negate(__m128 vec)
	{
		return _mm_xor_ps(vec, toM128(0x80000000));
	}

	static DESIRE_FORCE_INLINE __m128 Add(__m128 a, __m128 b)
	{
		return _mm_add_ps(a, b);
	}

	static DESIRE_FORCE_INLINE __m128 Sub(__m128 a, __m128 b)
	{
		return _mm_sub_ps(a, b);
	}

	static DESIRE_FORCE_INLINE __m128 Mul(__m128 vec, float scalar)
	{
		return _mm_mul_ps(vec, _mm_set1_ps(scalar));
	}

	// Comparison operators
	static DESIRE_FORCE_INLINE bool OpCmpGE(__m128 a, __m128 b)
	{
		return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0x7) == 0x7;
	}

	static DESIRE_FORCE_INLINE bool OpCmpLE(__m128 a, __m128 b)
	{
		return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0x7) == 0x7;
	}

	// Compute the dot product of two 3-D vectors
	static DESIRE_FORCE_INLINE __m128 Dot3(__m128 a, __m128 b)
	{
#if defined(__SSE4_1__)
		return _mm_dp_ps(a, b, 0x77);
#else
		__m128 result = SIMD::MulPerElem(a, b);
		return SIMD::Add(	SIMD::Shuffle_XXXX(result),
							SIMD::Add(	SIMD::Shuffle_YYYY(result),
										SIMD::Shuffle_ZZZZ(result)));
#endif
	}

	// Compute the dot product of two 4-D vectors
	static DESIRE_FORCE_INLINE __m128 Dot4(__m128 a, __m128 b)
	{
#if defined(__SSE4_1__)
		return _mm_dp_ps(a, b, 0xff);
#else
		__m128 result = SIMD::MulPerElem(a, b);
		return SIMD::Add(	SIMD::Shuffle_XXXX(result),
							SIMD::Add(	SIMD::Shuffle_YYYY(result),
										SIMD::Add(	SIMD::Shuffle_ZZZZ(result),
													SIMD::Shuffle_WWWW(result))));
#endif
	}

	// Compute cross product of two 3-D vectors
	static DESIRE_FORCE_INLINE __m128 Cross(__m128 a, __m128 b)
	{
		__m128 yzx0 = SIMD::Shuffle_YZXW(a);
		__m128 yzx1 = SIMD::Shuffle_YZXW(b);
		__m128 result = SIMD::Sub(SIMD::MulPerElem(yzx1, a), SIMD::MulPerElem(yzx0, b));
		return SIMD::Shuffle_YZXW(result);
	}

	// Multiply vectors per element
	static DESIRE_FORCE_INLINE __m128 MulPerElem(__m128 a, __m128 b)
	{
		return _mm_mul_ps(a, b);
	}

	// Divide vectors per element
	static DESIRE_FORCE_INLINE __m128 DivPerElem(__m128 a, __m128 b)
	{
		return _mm_div_ps(a, b);
	}

	// Compute the absolute value per element
	static DESIRE_FORCE_INLINE __m128 AbsPerElem(__m128 vec)
	{
		return _mm_and_ps(vec, toM128(0x7fffffff));
	}

	// Maximum of two vectors per element
	static DESIRE_FORCE_INLINE __m128 MaxPerElem(__m128 a, __m128 b)
	{
		return _mm_max_ps(a, b);
	}

	// Minimum of two vectors per element
	static DESIRE_FORCE_INLINE __m128 MinPerElem(__m128 a, __m128 b)
	{
		return _mm_min_ps(a, b);
	}

	// Get maximum element
	static DESIRE_FORCE_INLINE __m128 MaxElem3(__m128 vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Shuffle_YYYY(vec)), SIMD::Shuffle_ZZZZ(vec));
	}

	static DESIRE_FORCE_INLINE __m128 MaxElem4(__m128 vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Shuffle_YYYY(vec)), SIMD::MaxPerElem(SIMD::Shuffle_ZZZZ(vec), SIMD::Shuffle_WWWW(vec)));
	}

	// Get minimum element
	static DESIRE_FORCE_INLINE __m128 MinElem3(__m128 vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Shuffle_YYYY(vec)), SIMD::Shuffle_ZZZZ(vec));
	}

	static DESIRE_FORCE_INLINE __m128 MinElem4(__m128 vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Shuffle_YYYY(vec)), SIMD::MinPerElem(SIMD::Shuffle_ZZZZ(vec), SIMD::Shuffle_WWWW(vec)));
	}

	// Select packed single precision floating-point values from a and b using the mask
	static DESIRE_FORCE_INLINE __m128 Blend(__m128 a, __m128 b, __m128 mask)
	{
		return _mm_blendv_ps(a, b, mask);
	}

	// Shuffle
	static DESIRE_FORCE_INLINE __m128 Shuffle_XXXX(__m128 vec)
	{
		return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 0));
	}

	static DESIRE_FORCE_INLINE __m128 Shuffle_YYYY(__m128 vec)
	{
		return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1));
	}

	static DESIRE_FORCE_INLINE __m128 Shuffle_ZZZZ(__m128 vec)
	{
		return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2));
	}

	static DESIRE_FORCE_INLINE __m128 Shuffle_WWWW(__m128 vec)
	{
		return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 3, 3, 3));
	}

	static DESIRE_FORCE_INLINE __m128 Shuffle_YZXW(__m128 vec)
	{
		return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 0, 2, 1));
	}
};

// --------------------------------------------------------------------------------------------------------------------
//	SSE Helper functions
// --------------------------------------------------------------------------------------------------------------------

#define _mm_ror_ps(vec, i)	_mm_shuffle_ps(vec, vec, _MM_SHUFFLE((uint8_t)(i + 3) % 4,(uint8_t)(i + 2) % 4,(uint8_t)(i + 1) % 4,(uint8_t)(i + 0) % 4))
#define vec_madd(a, b, c)	_mm_add_ps(c, _mm_mul_ps(a, b))
#define vec_nmsub(a, b, c)	_mm_sub_ps(c, _mm_mul_ps(a, b))

static DESIRE_FORCE_INLINE __m128 newtonrapson_rsqrt4(const __m128 v)
{
	const __m128 approx = _mm_rsqrt_ps(v);
	const __m128 muls = _mm_mul_ps(_mm_mul_ps(v, approx), approx);
	return _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(0.5f), approx), _mm_sub_ps(_mm_set1_ps(3.0f), muls));
}

static DESIRE_FORCE_INLINE __m128 acosf4(__m128 x)
{
	__m128 xabs = _mm_and_ps(x, toM128(0x7fffffff));
	__m128 select = _mm_cmplt_ps(x, _mm_setzero_ps());
	__m128 t1 = _mm_sqrt_ps(_mm_sub_ps(_mm_set1_ps(1.0f), xabs));

	/* Instruction counts can be reduced if the polynomial was
	* computed entirely from nested (dependent) fma's. However,
	* to reduce the number of pipeline stalls, the polygon is evaluated
	* in two halves (hi amd lo).
	*/
	__m128 xabs2 = _mm_mul_ps(xabs, xabs);
	__m128 xabs4 = _mm_mul_ps(xabs2, xabs2);
	__m128 hi = vec_madd(vec_madd(vec_madd(_mm_set1_ps(-0.0012624911f),
		xabs, _mm_set1_ps(0.0066700901f)),
		xabs, _mm_set1_ps(-0.0170881256f)),
		xabs, _mm_set1_ps(0.0308918810f));
	__m128 lo = vec_madd(vec_madd(vec_madd(_mm_set1_ps(-0.0501743046f),
		xabs, _mm_set1_ps(0.0889789874f)),
		xabs, _mm_set1_ps(-0.2145988016f)),
		xabs, _mm_set1_ps(1.5707963050f));

	__m128 result = vec_madd(hi, xabs4, lo);

	// Adjust the result if x is negactive.
	return SIMD::Blend(
		_mm_mul_ps(t1, result),									// Positive
		vec_nmsub(t1, result, _mm_set1_ps(3.1415926535898f)),	// Negative
		select);
}

static DESIRE_FORCE_INLINE __m128 sinf4(__m128 x)
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
	xl = _mm_mul_ps(x, _mm_set1_ps(0.63661977236f));

	// Find the quadrant the angle falls in
	// using:  q = (int)(ceil(abs(xl)) * sign(xl))
	const __m128i q = _mm_cvtps_epi32(xl);

	// Compute the offset based on the quadrant that the angle falls in
	const __m128 offset = _mm_and_ps(_mm_castsi128_ps(q), toM128(0x3));

	// Remainder in range [-pi/4..pi/4]
	__m128 qf = _mm_cvtepi32_ps(q);
	xl = vec_nmsub(qf, _mm_set1_ps(_SINCOS_KC2), vec_nmsub(qf, _mm_set1_ps(_SINCOS_KC1), x));

	// Compute x^2 and x^3
	xl2 = _mm_mul_ps(xl, xl);
	xl3 = _mm_mul_ps(xl2, xl);

	// Compute both the sin and cos of the angles
	// using a polynomial expression:
	//   cx = 1.0f + xl2 * ((C0 * xl2 + C1) * xl2 + C2), and
	//   sx = xl + xl3 * ((S0 * xl2 + S1) * xl2 + S2)
	__m128 cx =
		vec_madd(
			vec_madd(
				vec_madd(_mm_set1_ps(_SINCOS_CC0), xl2, _mm_set1_ps(_SINCOS_CC1)), xl2, _mm_set1_ps(_SINCOS_CC2)), xl2, _mm_set1_ps(1.0f));
	__m128 sx =
		vec_madd(
			vec_madd(
				vec_madd(_mm_set1_ps(_SINCOS_SC0), xl2, _mm_set1_ps(_SINCOS_SC1)), xl2, _mm_set1_ps(_SINCOS_SC2)), xl3, xl);

	// Use the cosine when the offset is odd and the sin
	// when the offset is even
	res = SIMD::Blend(cx, sx, _mm_cmpeq_ps(_mm_and_ps(offset, toM128(0x1)), _mm_setzero_ps()));

	// Flip the sign of the result when (offset mod 4) = 1 or 2
	return SIMD::Blend(
		_mm_xor_ps(toM128(0x80000000U), res),	// Negative
		res,									// Positive
		_mm_cmpeq_ps(_mm_and_ps(offset, toM128(0x2)), _mm_setzero_ps()));
}

static DESIRE_FORCE_INLINE void sincosf4(__m128 x, __m128 *s, __m128 *c)
{
	__m128 xl, xl2, xl3;

	// Range reduction using : xl = angle * TwoOverPi;
	xl = _mm_mul_ps(x, _mm_set1_ps(0.63661977236f));

	// Find the quadrant the angle falls in
	// using:  q = (int)(ceil(abs(xl)) * sign(xl))
	//	const __m128i q = _mm_cvtps_epi32(_mm_add_ps(xl, SIMD::Blend(_mm_set1_ps(0.5f), xl, 0x80000000)));
	const __m128i q = _mm_cvtps_epi32(xl);

	// Compute the offset based on the quadrant that the angle falls in.
	// Add 1 to the offset for the cosine. 
	const __m128 offsetSin = _mm_and_ps(_mm_castsi128_ps(q), toM128(0x3));
	const __m128 offsetCos = _mm_castsi128_ps(_mm_add_epi32(_mm_set1_epi32(1), (__m128i&)offsetSin));

	// Remainder in range [-pi/4..pi/4]
	__m128 qf = _mm_cvtepi32_ps(q);
	xl = vec_nmsub(qf, _mm_set1_ps(_SINCOS_KC2), vec_nmsub(qf, _mm_set1_ps(_SINCOS_KC1), x));

	// Compute x^2 and x^3
	xl2 = _mm_mul_ps(xl, xl);
	xl3 = _mm_mul_ps(xl2, xl);

	// Compute both the sin and cos of the angles
	// using a polynomial expression:
	//   cx = 1.0f + xl2 * ((C0 * xl2 + C1) * xl2 + C2), and
	//   sx = xl + xl3 * ((S0 * xl2 + S1) * xl2 + S2)
	__m128 cx =
		vec_madd(
			vec_madd(
				vec_madd(_mm_set1_ps(_SINCOS_CC0), xl2, _mm_set1_ps(_SINCOS_CC1)), xl2, _mm_set1_ps(_SINCOS_CC2)), xl2, _mm_set1_ps(1.0f));
	__m128 sx =
		vec_madd(
			vec_madd(
				vec_madd(_mm_set1_ps(_SINCOS_SC0), xl2, _mm_set1_ps(_SINCOS_SC1)), xl2, _mm_set1_ps(_SINCOS_SC2)), xl3, xl);

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
