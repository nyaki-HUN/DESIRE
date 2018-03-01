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

	static inline __m128 _mm_blendv_ps(__m128 a, __m128 b, __m128 mask)
	{
		return _mm_or_ps(_mm_and_ps(mask, b), _mm_andnot_ps(mask, a));
	}
#endif

typedef __m128	vec_float3_t;
typedef __m128	vec_float4_t;

static inline __m128 toM128(uint32_t x)
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
	static inline __m128 Construct(float x, float y, float z, float w = 0.0f)
	{
		return _mm_setr_ps(x, y, z, w);
	}

	// Construct by setting all elements to the same scalar value
	static inline void Construct(__m128& vec, float scalar)
	{
		vec = _mm_set1_ps(scalar);
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static inline void LoadXYZW(__m128& vec, const float *fptr)
	{
		vec = _mm_castsi128_ps(_mm_lddqu_si128((const __m128i*)fptr));
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static inline void StoreXYZW(__m128 vec, float *fptr)
	{
		_mm_storeu_ps(fptr, vec);
	}

	// Set element
	static inline void SetX(__m128& vec, float x)
	{
		vec = _mm_move_ss(vec, _mm_set_ss(x));
	}

	static inline void SetY(__m128& vec, float y)
	{
		__m128 t = _mm_move_ss(vec, _mm_set_ss(y));
		t = SIMD::Swizzle_XXZW(t);
		vec = _mm_move_ss(t, vec);
	}

	static inline void SetZ(__m128& vec, float z)
	{
		__m128 t = _mm_move_ss(vec, _mm_set_ss(z));
		t = SIMD::Swizzle_XYXW(t);
		vec = _mm_move_ss(t, vec);
	}

	static inline void SetW(__m128& vec, float w)
	{
		__m128 t = _mm_move_ss(vec, _mm_set_ss(w));
		t = SIMD::Swizzle_XYZX(t);
		vec = _mm_move_ss(t, vec);
	}

	// Get element
	static inline float GetX(__m128 vec)		{ return _mm_cvtss_f32(vec); }
	static inline float GetY(__m128 vec)		{ return _mm_cvtss_f32(SIMD::Swizzle_YYYY(vec)); }
	static inline float GetZ(__m128 vec)		{ return _mm_cvtss_f32(SIMD::Swizzle_ZZZZ(vec)); }
	static inline float GetW(__m128 vec)		{ return _mm_cvtss_f32(SIMD::Swizzle_WWWW(vec)); }

	// Operator overloads
	static inline __m128 Negate(__m128 vec)
	{
		return _mm_xor_ps(vec, toM128(0x80000000));
	}

	static inline __m128 Add(__m128 a, __m128 b)
	{
		return _mm_add_ps(a, b);
	}

	static inline __m128 Sub(__m128 a, __m128 b)
	{
		return _mm_sub_ps(a, b);
	}

	static inline __m128 Mul(__m128 vec, float scalar)
	{
		return _mm_mul_ps(vec, _mm_set1_ps(scalar));
	}

	static inline __m128 Mul(__m128 a, __m128 b)
	{
		return _mm_mul_ps(a, b);
	}

	static inline __m128 MulAdd(__m128 a, __m128 b, __m128 c)
	{
		return SIMD::Add(c, SIMD::Mul(a, b));
	}

	static inline __m128 MulSub(__m128 a, __m128 b, __m128 c)
	{
		return SIMD::Sub(c, SIMD::Mul(a, b));
	}

	static inline __m128 Div(__m128 a, __m128 b)
	{
		return _mm_div_ps(a, b);
	}

	// Comparison operators
	static inline bool OpCmpGE(__m128 a, __m128 b)
	{
		return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0x7) == 0x7;
	}

	static inline bool OpCmpLE(__m128 a, __m128 b)
	{
		return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0x7) == 0x7;
	}

	// Compute the dot product of two 3-D vectors
	static inline __m128 Dot3(__m128 a, __m128 b)
	{
#if defined(__SSE4_1__)
		return _mm_dp_ps(a, b, 0x77);
#else
		__m128 result = SIMD::Mul(a, b);
		return SIMD::Add(	SIMD::Swizzle_XXXX(result),
							SIMD::Add(	SIMD::Swizzle_YYYY(result),
										SIMD::Swizzle_ZZZZ(result)));
#endif
	}

	// Compute the dot product of two 4-D vectors
	static inline __m128 Dot4(__m128 a, __m128 b)
	{
#if defined(__SSE4_1__)
		return _mm_dp_ps(a, b, 0xff);
#else
		__m128 result = SIMD::Mul(a, b);
		return SIMD::Add(	SIMD::Swizzle_XXXX(result),
							SIMD::Add(	SIMD::Swizzle_YYYY(result),
										SIMD::Add(	SIMD::Swizzle_ZZZZ(result),
													SIMD::Swizzle_WWWW(result))));
#endif
	}

	// Compute cross product of two 3-D vectors
	static inline __m128 Cross(__m128 a, __m128 b)
	{
		__m128 yzx0 = SIMD::Swizzle_YZXW(a);
		__m128 yzx1 = SIMD::Swizzle_YZXW(b);
		__m128 result = SIMD::Sub(SIMD::Mul(yzx1, a), SIMD::Mul(yzx0, b));
		return SIMD::Swizzle_YZXW(result);
	}

	// Compute the absolute value per element
	static inline __m128 AbsPerElem(__m128 vec)
	{
		return _mm_and_ps(vec, toM128(0x7fffffff));
	}

	// Maximum of two vectors per element
	static inline __m128 MaxPerElem(__m128 a, __m128 b)
	{
		return _mm_max_ps(a, b);
	}

	// Minimum of two vectors per element
	static inline __m128 MinPerElem(__m128 a, __m128 b)
	{
		return _mm_min_ps(a, b);
	}

	// Get maximum element
	static inline __m128 MaxElem3(__m128 vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static inline __m128 MaxElem4(__m128 vec)
	{
		return SIMD::MaxPerElem(SIMD::MaxPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MaxPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Get minimum element
	static inline __m128 MinElem3(__m128 vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::Swizzle_ZZZZ(vec));
	}

	static inline __m128 MinElem4(__m128 vec)
	{
		return SIMD::MinPerElem(SIMD::MinPerElem(vec, SIMD::Swizzle_YYYY(vec)), SIMD::MinPerElem(SIMD::Swizzle_ZZZZ(vec), SIMD::Swizzle_WWWW(vec)));
	}

	// Blend (select) elements from a and b using the mask
	static inline __m128 Blend(__m128 a, __m128 b, __m128 mask)
	{
		return _mm_blendv_ps(a, b, mask);
	}

	// Special blends for one single precision floating-point value
	static inline __m128 Blend_X(__m128 to, __m128 from)	{ return _mm_blend_ps(to, from, 0b0001); }
	static inline __m128 Blend_Y(__m128 to, __m128 from)	{ return _mm_blend_ps(to, from, 0b0010); }
	static inline __m128 Blend_Z(__m128 to, __m128 from)	{ return _mm_blend_ps(to, from, 0b0100); }
	static inline __m128 Blend_W(__m128 to, __m128 from)	{ return _mm_blend_ps(to, from, 0b1000); }

	// Swizzle
	static inline __m128 Swizzle_XXXX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 0)); }
	static inline __m128 Swizzle_XXYY(__m128 vec)		{ return _mm_unpacklo_ps(vec, vec); }
	static inline __m128 Swizzle_XXZZ(__m128 vec)		{ return _mm_moveldup_ps(vec); }
	static inline __m128 Swizzle_XXZW(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 2, 0, 0)); }
	static inline __m128 Swizzle_XYXY(__m128 vec)		{ return _mm_movelh_ps(vec, vec); }
	static inline __m128 Swizzle_XYXW(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 0, 1, 0)); }
	static inline __m128 Swizzle_XYYX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 1, 0)); }
	static inline __m128 Swizzle_XYZX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 2, 1, 0)); }
	static inline __m128 Swizzle_XYWZ(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 1, 0)); }
	static inline __m128 Swizzle_XZXX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 2, 0)); }
	static inline __m128 Swizzle_XZXZ(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 0, 2, 0)); }

	static inline __m128 Swizzle_YXXX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 1)); }
	static inline __m128 Swizzle_YXYX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 0, 1)); }
	static inline __m128 Swizzle_YXZW(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 2, 0, 1)); }
	static inline __m128 Swizzle_YXWZ(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 0, 1)); }
	static inline __m128 Swizzle_YYYY(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1)); }
	static inline __m128 Swizzle_YYWW(__m128 vec)		{ return _mm_movehdup_ps(vec); }
	static inline __m128 Swizzle_YZXW(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 0, 2, 1)); }
	static inline __m128 Swizzle_YZWX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 3, 2, 1)); }
	static inline __m128 Swizzle_YWYW(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 1, 3, 1)); }

	static inline __m128 Swizzle_ZXYW(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 1, 0, 2)); }
	static inline __m128 Swizzle_ZXWY(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 3, 0, 2)); }
	static inline __m128 Swizzle_ZZZZ(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2)); }
	static inline __m128 Swizzle_ZZYX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 2, 2)); }
	static inline __m128 Swizzle_ZZWX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 3, 2, 2)); }
	static inline __m128 Swizzle_ZZWW(__m128 vec)		{ return _mm_unpackhi_ps(vec, vec); }
	static inline __m128 Swizzle_ZWXY(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 0, 3, 2)); }
	static inline __m128 Swizzle_ZWYX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 3, 2)); }
	static inline __m128 Swizzle_ZWZW(__m128 vec)		{ return _mm_movehl_ps(vec, vec); }
	static inline __m128 Swizzle_ZWWZ(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 3, 2)); }

	static inline __m128 Swizzle_WXYZ(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 1, 0, 3)); }
	static inline __m128 Swizzle_WZXY(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 0, 2, 3)); }
	static inline __m128 Swizzle_WZYX(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 2, 3)); }
	static inline __m128 Swizzle_WZWZ(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 2, 3)); }
	static inline __m128 Swizzle_WWWW(__m128 vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 3, 3, 3)); }
};

// --------------------------------------------------------------------------------------------------------------------
//	SSE Helper functions
// --------------------------------------------------------------------------------------------------------------------

#define _mm_ror_ps(vec, i)	_mm_shuffle_ps(vec, vec, _MM_SHUFFLE((uint8_t)(i + 3) % 4,(uint8_t)(i + 2) % 4,(uint8_t)(i + 1) % 4,(uint8_t)(i + 0) % 4))

static inline __m128 newtonrapson_rsqrt4(const __m128 v)
{
	const __m128 approx = _mm_rsqrt_ps(v);
	const __m128 muls = SIMD::Mul(SIMD::Mul(v, approx), approx);
	return SIMD::Mul(SIMD::Mul(approx, 0.5f), SIMD::Sub(_mm_set1_ps(3.0f), muls));
}

static inline __m128 acosf4(__m128 x)
{
	__m128 xabs = _mm_and_ps(x, toM128(0x7fffffff));
	__m128 select = _mm_cmplt_ps(x, _mm_setzero_ps());
	__m128 t1 = _mm_sqrt_ps(SIMD::Sub(_mm_set1_ps(1.0f), xabs));

	/* Instruction counts can be reduced if the polynomial was
	* computed entirely from nested (dependent) fma's. However,
	* to reduce the number of pipeline stalls, the polygon is evaluated
	* in two halves (hi amd lo).
	*/
	__m128 xabs2 = SIMD::Mul(xabs, xabs);
	__m128 xabs4 = SIMD::Mul(xabs2, xabs2);
	__m128 hi = SIMD::MulAdd(SIMD::MulAdd(SIMD::MulAdd(_mm_set1_ps(-0.0012624911f),
		xabs, _mm_set1_ps(0.0066700901f)),
		xabs, _mm_set1_ps(-0.0170881256f)),
		xabs, _mm_set1_ps(0.0308918810f));
	__m128 lo = SIMD::MulAdd(SIMD::MulAdd(SIMD::MulAdd(_mm_set1_ps(-0.0501743046f),
		xabs, _mm_set1_ps(0.0889789874f)),
		xabs, _mm_set1_ps(-0.2145988016f)),
		xabs, _mm_set1_ps(1.5707963050f));

	__m128 result = SIMD::MulAdd(hi, xabs4, lo);

	// Adjust the result if x is negactive.
	return SIMD::Blend(
		SIMD::Mul(t1, result),									// Positive
		SIMD::MulSub(t1, result, _mm_set1_ps(3.1415926535898f)),	// Negative
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
	xl = SIMD::Mul(x, _mm_set1_ps(0.63661977236f));

	// Find the quadrant the angle falls in
	// using:  q = (int)(ceil(abs(xl)) * sign(xl))
	const __m128i q = _mm_cvtps_epi32(xl);

	// Compute the offset based on the quadrant that the angle falls in
	const __m128 offset = _mm_and_ps(_mm_castsi128_ps(q), toM128(0x3));

	// Remainder in range [-pi/4..pi/4]
	__m128 qf = _mm_cvtepi32_ps(q);
	xl = SIMD::MulSub(qf, _mm_set1_ps(_SINCOS_KC2), SIMD::MulSub(qf, _mm_set1_ps(_SINCOS_KC1), x));

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
				SIMD::MulAdd(_mm_set1_ps(_SINCOS_CC0), xl2, _mm_set1_ps(_SINCOS_CC1)), xl2, _mm_set1_ps(_SINCOS_CC2)), xl2, _mm_set1_ps(1.0f));
	__m128 sx =
		SIMD::MulAdd(
			SIMD::MulAdd(
				SIMD::MulAdd(_mm_set1_ps(_SINCOS_SC0), xl2, _mm_set1_ps(_SINCOS_SC1)), xl2, _mm_set1_ps(_SINCOS_SC2)), xl3, xl);

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
	xl = SIMD::Mul(x, _mm_set1_ps(0.63661977236f));

	// Find the quadrant the angle falls in
	// using:  q = (int)(ceil(abs(xl)) * sign(xl))
//	const __m128i q = _mm_cvtps_epi32(SIMD::Add(xl, SIMD::Blend(_mm_set1_ps(0.5f), xl, 0x80000000)));
	const __m128i q = _mm_cvtps_epi32(xl);

	// Compute the offset based on the quadrant that the angle falls in.
	// Add 1 to the offset for the cosine. 
	const __m128 offsetSin = _mm_and_ps(_mm_castsi128_ps(q), toM128(0x3));
	const __m128 offsetCos = _mm_castsi128_ps(_mm_add_epi32(_mm_set1_epi32(1), (__m128i&)offsetSin));

	// Remainder in range [-pi/4..pi/4]
	__m128 qf = _mm_cvtepi32_ps(q);
	xl = SIMD::MulSub(qf, _mm_set1_ps(_SINCOS_KC2), SIMD::MulSub(qf, _mm_set1_ps(_SINCOS_KC1), x));

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
				SIMD::MulAdd(_mm_set1_ps(_SINCOS_CC0), xl2, _mm_set1_ps(_SINCOS_CC1)), xl2, _mm_set1_ps(_SINCOS_CC2)), xl2, _mm_set1_ps(1.0f));
	__m128 sx =
		SIMD::MulAdd(
			SIMD::MulAdd(
				SIMD::MulAdd(_mm_set1_ps(_SINCOS_SC0), xl2, _mm_set1_ps(_SINCOS_SC1)), xl2, _mm_set1_ps(_SINCOS_SC2)), xl3, xl);

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
