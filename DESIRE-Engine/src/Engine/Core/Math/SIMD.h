#pragma once

#if DESIRE_USE_SSE
	typedef __m128	simd128_t;
#elif DESIRE_USE_NEON
	typedef float32x4_t	simd128_t;
#else
	struct simd128_t
	{
		union
		{
			float f32[4];
			uint32_t u32[4];
		};

		inline simd128_t()
		{
		}

		inline simd128_t(float x, float y, float z, float w)
		{
			f32[0] = x;
			f32[1] = y;
			f32[2] = z;
			f32[3] = w;
		}

		inline simd128_t(uint32_t x, uint32_t y, uint32_t z, uint32_t w)
		{
			u32[0] = x;
			u32[1] = y;
			u32[2] = z;
			u32[3] = w;
		}
	};
#endif

class SIMD
{
public:
	// Construct from x, y, z, and w elements
	static inline simd128_t Construct(float x, float y, float z, float w = 0.0f)
	{
#if DESIRE_USE_SSE
		return _mm_setr_ps(x, y, z, w);
#elif DESIRE_USE_NEON
		return (float32x4_t) { x, y, z, w };
#else
		return simd128_t(x, y, z, w);
#endif
	}

	// Construct by setting all elements to the same scalar value
	static inline simd128_t Construct(float scalar)
	{
#if DESIRE_USE_SSE
		return _mm_set_ps1(scalar);
#elif DESIRE_USE_NEON
		return vdupq_n_f32(scalar);
#else
		return SIMD::Construct(scalar, scalar, scalar, scalar);
#endif
	}

	// Load x, y, z, and w elements from the first four elements of a float array
	static inline simd128_t LoadXYZW(const float* fptr)
	{
#if DESIRE_USE_SSE
		return _mm_loadu_ps(fptr);
#elif DESIRE_USE_NEON
		return vld1q_f32(fptr);
#else
		return SIMD::Construct(fptr[0], fptr[1], fptr[2], fptr[3]);
#endif
	}

	// Store x, y, z, and w elements in the first four elements of a float array
	static inline void StoreXYZW(simd128_t vec, float* fptr)
	{
#if DESIRE_USE_SSE
		_mm_storeu_ps(fptr, vec);
#elif DESIRE_USE_NEON
		vst1q_f32(fptr, vec);
#else
		memcpy(fptr, vec.f32, 4 * sizeof(float));
#endif
	}

	// Set element
	static inline simd128_t SetX(simd128_t vec, float x)
	{
#if DESIRE_USE_SSE
		return _mm_move_ss(vec, _mm_set_ss(x));
#elif DESIRE_USE_NEON
		return vsetq_lane_f32(x, vec, 0);
#else
		vec.f32[0] = x;
		return vec;
#endif
	}

	static inline simd128_t SetY(simd128_t vec, float y)
	{
#if DESIRE_USE_SSE
		const __m128 y000 = _mm_set_ss(y);
	#if defined(__SSE4_1__)
		return _mm_insert_ps(vec, y000, 0x10);
	#else
		__m128 yxzw = SIMD::Swizzle_YXZW(vec);
		yxzw = _mm_move_ss(yxzw, y000);
		return SIMD::Swizzle_YXZW(yxzw);
	#endif
#elif DESIRE_USE_NEON
		return vsetq_lane_f32(y, vec, 1);
#else
		vec.f32[1] = y;
		return vec;
#endif
	}

	static inline simd128_t SetZ(simd128_t vec, float z)
	{
#if DESIRE_USE_SSE
		const __m128 z000 = _mm_set_ss(z);
	#if defined(__SSE4_1__)
		return _mm_insert_ps(vec, z000, 0x20);
	#else
		__m128 zyxw = SIMD::Swizzle_ZYXW(vec);
		zyxw = _mm_move_ss(zyxw, z000);
		return SIMD::Swizzle_ZYXW(zyxw);
	#endif
#elif DESIRE_USE_NEON
		return vsetq_lane_f32(z, vec, 2);
#else
		vec.f32[2] = z;
		return vec;
#endif
	}

	static inline simd128_t SetW(simd128_t vec, float w)
	{
#if DESIRE_USE_SSE
		const __m128 w000 = _mm_set_ss(w);
	#if defined(__SSE4_1__)
		return _mm_insert_ps(vec, w000, 0x30);
	#else
		__m128 wyzx = SIMD::Swizzle_WYZX(vec);
		wyzx = _mm_move_ss(wyzx, w000);
		return SIMD::Swizzle_WYZX(wyzx);
	#endif
#elif DESIRE_USE_NEON
		return vsetq_lane_f32(w, vec, 3);
#else
		vec.f32[3] = w;
		return vec;
#endif
	}

	// Get element
	static inline float GetX(simd128_t vec)
	{
#if DESIRE_USE_SSE
		return _mm_cvtss_f32(vec);
#elif DESIRE_USE_NEON
		return vgetq_lane_f32(vec, 0);
#else
		return vec.f32[0];
#endif
	}

	static inline float GetY(simd128_t vec)
	{
#if DESIRE_USE_SSE
		return _mm_cvtss_f32(SIMD::Swizzle_YYYY(vec));
#elif DESIRE_USE_NEON
		return vgetq_lane_f32(vec, 1);
#else
		return vec.f32[1];
#endif
	}

	static inline float GetZ(simd128_t vec)
	{
#if DESIRE_USE_SSE
		return _mm_cvtss_f32(SIMD::Swizzle_ZZZZ(vec));
#elif DESIRE_USE_NEON
		return vgetq_lane_f32(vec, 2);
#else
		return vec.f32[2];
#endif
	}

	static inline float GetW(simd128_t vec)
	{
#if DESIRE_USE_SSE
		return _mm_cvtss_f32(SIMD::Swizzle_WWWW(vec));
#elif DESIRE_USE_NEON
		return vgetq_lane_f32(vec, 3);
#else
		return vec.f32[3];
#endif
	}

	// Operator overloads
	static inline simd128_t Negate(simd128_t vec)
	{
#if DESIRE_USE_SSE
		return _mm_xor_ps(vec, SIMD::Construct(-0.0f));
#elif DESIRE_USE_NEON
		return vnegq_f32(vec);
#else
		return SIMD::Construct(
			-SIMD::GetX(vec),
			-SIMD::GetY(vec),
			-SIMD::GetZ(vec),
			-SIMD::GetW(vec)
		);
#endif
	}

	static inline simd128_t Add(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return _mm_add_ps(a, b);
#elif DESIRE_USE_NEON
		return vaddq_f32(a, b); 
#else
		return SIMD::Construct(
			SIMD::GetX(a) + SIMD::GetX(b),
			SIMD::GetY(a) + SIMD::GetY(b),
			SIMD::GetZ(a) + SIMD::GetZ(b),
			SIMD::GetW(a) + SIMD::GetW(b)
		);
#endif
	}

	static inline simd128_t Sub(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return _mm_sub_ps(a, b);
#elif DESIRE_USE_NEON
		return vsubq_f32(a, b);
#else
		return SIMD::Construct(
			SIMD::GetX(a) - SIMD::GetX(b),
			SIMD::GetY(a) - SIMD::GetY(b),
			SIMD::GetZ(a) - SIMD::GetZ(b),
			SIMD::GetW(a) - SIMD::GetW(b)
		);
#endif
	}

	static inline simd128_t Mul(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return _mm_mul_ps(a, b);
#elif DESIRE_USE_NEON
		return vmulq_f32(a, b);
#else
		return SIMD::Construct(
			SIMD::GetX(a) * SIMD::GetX(b),
			SIMD::GetY(a) * SIMD::GetY(b),
			SIMD::GetZ(a) * SIMD::GetZ(b),
			SIMD::GetW(a) * SIMD::GetW(b)
		);
#endif
	}

	static inline simd128_t Mul(simd128_t vec, float scalar)
	{
#if DESIRE_USE_SSE
		return _mm_mul_ps(vec, SIMD::Construct(scalar));
#elif DESIRE_USE_NEON
		return vmulq_n_f32(vec, scalar);
#else
		return SIMD::Construct(
			SIMD::GetX(vec) * scalar,
			SIMD::GetY(vec) * scalar,
			SIMD::GetZ(vec) * scalar,
			SIMD::GetW(vec) * scalar
		);
#endif
	}

	// Per component multiplication and addition of the three inputs: c + (a * b)
	static inline simd128_t MulAdd(simd128_t a, simd128_t b, simd128_t c)
	{
#if DESIRE_USE_NEON
		return vfmaq_f32(c, a, b);
#else
		return SIMD::Add(c, SIMD::Mul(a, b));
#endif
	}

	// Per component multiplication and subtraction of the three inputs: c - (a * b)
	static inline simd128_t MulSub(simd128_t a, simd128_t b, simd128_t c)
	{
#if DESIRE_USE_NEON
		return vfmsq_f32(c, a, b);
#else
		return SIMD::Sub(c, SIMD::Mul(a, b));
#endif
	}

	static inline simd128_t Div(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return _mm_div_ps(a, b);
#elif DESIRE_USE_NEON
		return vdivq_f32(a, b);
#else
		return SIMD::Construct(
			SIMD::GetX(a) / SIMD::GetX(b),
			SIMD::GetY(a) / SIMD::GetY(b),
			SIMD::GetZ(a) / SIMD::GetZ(b),
			SIMD::GetW(a) / SIMD::GetW(b)
		);
#endif
	}

	// Comparison operators
	static inline bool OpCmp_GT(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return _mm_movemask_ps(_mm_cmpgt_ps(a, b)) == 0xF;
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcgtq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) == 0xFFFFFFFF;
#else
		return (
			SIMD::GetX(a) > SIMD::GetX(b) &&
			SIMD::GetY(a) > SIMD::GetY(b) &&
			SIMD::GetZ(a) > SIMD::GetZ(b) &&
			SIMD::GetW(a) > SIMD::GetW(b)
		);
#endif
	}

	static inline bool OpCmp3_GT(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) & 0x7) == 0x7;
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcgtq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFF) == 0x00FFFFFF;
#else
		return (
			SIMD::GetX(a) > SIMD::GetX(b) &&
			SIMD::GetY(a) > SIMD::GetY(b) &&
			SIMD::GetZ(a) > SIMD::GetZ(b)
		);
#endif
	}

	static inline bool OpCmp_LT(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) == 0xF);
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcltq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) == 0xFFFFFFFF;
#else
		return (
			SIMD::GetX(a) < SIMD::GetX(b) &&
			SIMD::GetY(a) < SIMD::GetY(b) &&
			SIMD::GetZ(a) < SIMD::GetZ(b) &&
			SIMD::GetW(a) < SIMD::GetW(b)
		);
#endif
	}

	static inline bool OpCmp3_LT(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) & 0x7) == 0x7;
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcltq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFF) == 0x00FFFFFF;
#else
		return (
			SIMD::GetX(a) < SIMD::GetX(b) &&
			SIMD::GetY(a) < SIMD::GetY(b) &&
			SIMD::GetZ(a) < SIMD::GetZ(b)
		);
#endif
	}

	static inline bool OpCmp_GE(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) == 0xF);
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcgeq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) == 0xFFFFFFFF;
#else
		return (
			SIMD::GetX(a) >= SIMD::GetX(b) &&
			SIMD::GetY(a) >= SIMD::GetY(b) &&
			SIMD::GetZ(a) >= SIMD::GetZ(b) && 
			SIMD::GetW(a) >= SIMD::GetW(b)
		);
#endif
	}

	static inline bool OpCmp3_GE(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return (_mm_movemask_ps(_mm_cmpge_ps(a, b)) & 0x7) == 0x7;
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcgeq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFF) == 0x00FFFFFF;
#else
		return (
			SIMD::GetX(a) >= SIMD::GetX(b) &&
			SIMD::GetY(a) >= SIMD::GetY(b) &&
			SIMD::GetZ(a) >= SIMD::GetZ(b)
		);
#endif
	}

	static inline bool OpCmp_LE(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return (_mm_movemask_ps(_mm_cmple_ps(a, b)) == 0xF);
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcleq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) == 0xFFFFFFFF; 
#else
		return (
			SIMD::GetX(a) <= SIMD::GetX(b) &&
			SIMD::GetY(a) <= SIMD::GetY(b) &&
			SIMD::GetZ(a) <= SIMD::GetZ(b) &&
			SIMD::GetW(a) <= SIMD::GetW(b)
		);
#endif
	}

	static inline bool OpCmp3_LE(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return (_mm_movemask_ps(_mm_cmple_ps(a, b)) & 0x7) == 0x7;
#elif DESIRE_USE_NEON
		uint32x4_t mask = vcleq_f32(a, b);
		uint8x8x2_t mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15 = vzip_u8(vget_low_u8(mask), vget_high_u8(mask));
		uint16x4x2_t mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15 = vzip_u16(mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[0], mask_0_8_1_9_2_10_3_11_4_12_5_13_6_14_7_15.val[1]);
		return (vget_lane_u32(mask_0_8_4_12_1_9_5_13_2_10_6_14_3_11_7_15.val[0], 0) & 0x00FFFFFF) == 0x00FFFFFF;
#else
		return (
			SIMD::GetX(a) <= SIMD::GetX(b) &&
			SIMD::GetY(a) <= SIMD::GetY(b) &&
			SIMD::GetZ(a) <= SIMD::GetZ(b)
		);
#endif
	}

	// Compute the dot product of two 3-D vectors
	static inline simd128_t Dot3(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
	#if defined(__SSE4_1__)
		return _mm_dp_ps(a, b, 0x7F);
	#else
		__m128 ab = SIMD::Mul(a, b);
		alignas(16) const uint32_t mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 };
		ab = _mm_and_ps(ab, _mm_load_ps(reinterpret_cast<const float*>(mask_xyz));
		__m128 xy_z_xy_z = _mm_hadd_ps(ab, ab);
		return _mm_hadd_ps(xy_z_xy_z, xy_z_xy_z);
	#endif
#elif DESIRE_USE_NEON
		float32x4_t ab = SIMD::Mul(a, b);
		float32x2_t xy = vget_low_f32(ab);
		float32x2_t xy_xy = vpadd_f32(xy, xy);
		float32x2_t z_z = vdup_lane_f32(vget_high_f32(ab), 0);
		float32x2_t xyz_xyz = vadd_f32(xy_xy, z_z);
		return vcombine_f32(xyz_xyz, xyz_xyz);
#else
		return SIMD::Construct(
			SIMD::GetX(a) * SIMD::GetX(b) +
			SIMD::GetY(a) * SIMD::GetY(b) +
			SIMD::GetZ(a) * SIMD::GetZ(b)
		);
#endif
	}

	// Compute the dot product of two 4-D vectors
	static inline simd128_t Dot4(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
	#if defined(__SSE4_1__)
		return _mm_dp_ps(a, b, 0xFF);
	#else
		__m128 ab = SIMD::Mul(a, b);
		__m128 xy_zw_xy_zw = _mm_hadd_ps(ab, ab);
		return _mm_hadd_ps(xy_zw_xy_zw, xy_zw_xy_zw);
	#endif
#elif DESIRE_USE_NEON
		float32x4_t ab = SIMD::Mul(a, b);
		float32x2_t xz_yw = vadd_f32(vget_low_f32(ab), vget_high_f32(ab));
		float32x2_t xyzw_xyzw = vpadd_f32(xz_yw, xz_yw);
		return vcombine_f32(xyzw_xyzw, xyzw_xyzw);
#else
		return SIMD::Construct(
			SIMD::GetX(a) * SIMD::GetX(b) +
			SIMD::GetY(a) * SIMD::GetY(b) +
			SIMD::GetZ(a) * SIMD::GetZ(b) +
			SIMD::GetW(a) * SIMD::GetW(b)
		);
#endif
	}

	// Compute cross product of two 3-D vectors
	static inline simd128_t Cross(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE || DESIRE_USE_NEON
		simd128_t yzxA = SIMD::Swizzle_YZXY(a);
		simd128_t yzxB = SIMD::Swizzle_YZXY(b);
		simd128_t result = SIMD::MulSub(yzxA, b, SIMD::Mul(yzxB, a));
		return SIMD::Swizzle_YZXY(result);
#else
		return SIMD::Construct(
			SIMD::GetY(a) * SIMD::GetZ(b) - SIMD::GetZ(a) * SIMD::GetY(b),
			SIMD::GetZ(a) * SIMD::GetX(b) - SIMD::GetX(a) * SIMD::GetZ(b),
			SIMD::GetX(a) * SIMD::GetY(b) - SIMD::GetY(a) * SIMD::GetX(b)
		);
#endif
	}

	// Compute the absolute value per element
	static inline simd128_t AbsPerElem(simd128_t vec)
	{
#if DESIRE_USE_SSE
		const uint32_t mask = 0x7fffffff;
		return _mm_and_ps(vec, SIMD::Construct(*reinterpret_cast<const float*>(&mask)));
#elif DESIRE_USE_NEON
		return vabsq_f32(vec);
#else
		return SIMD::Construct(
			std::abs(SIMD::GetX(vec)),
			std::abs(SIMD::GetY(vec)),
			std::abs(SIMD::GetZ(vec)),
			std::abs(SIMD::GetW(vec))
		);
#endif
	}

	// Maximum of two vectors per element
	static inline simd128_t MaxPerElem(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return _mm_max_ps(a, b);
#elif DESIRE_USE_NEON
		return vmaxq_f32(a, b);
#else
		return SIMD::Construct(
			(SIMD::GetX(a) > SIMD::GetX(b)) ? SIMD::GetX(a) : SIMD::GetX(b),
			(SIMD::GetY(a) > SIMD::GetY(b)) ? SIMD::GetY(a) : SIMD::GetY(b),
			(SIMD::GetZ(a) > SIMD::GetZ(b)) ? SIMD::GetZ(a) : SIMD::GetZ(b),
			(SIMD::GetW(a) > SIMD::GetW(b)) ? SIMD::GetW(a) : SIMD::GetW(b)
		);
#endif
	}

	// Minimum of two vectors per element
	static inline simd128_t MinPerElem(simd128_t a, simd128_t b)
	{
#if DESIRE_USE_SSE
		return _mm_min_ps(a, b);
#elif DESIRE_USE_NEON
		return vminq_f32(a, b);
#else
		return SIMD::Construct(
			(SIMD::GetX(a) < SIMD::GetX(b)) ? SIMD::GetX(a) : SIMD::GetX(b),
			(SIMD::GetY(a) < SIMD::GetY(b)) ? SIMD::GetY(a) : SIMD::GetY(b),
			(SIMD::GetZ(a) < SIMD::GetZ(b)) ? SIMD::GetZ(a) : SIMD::GetZ(b),
			(SIMD::GetW(a) < SIMD::GetW(b)) ? SIMD::GetW(a) : SIMD::GetW(b)
		);
#endif
	}

	// Get maximum element
	static inline simd128_t MaxElem(simd128_t vec)
	{
		const simd128_t xz_yw_xz_yw = SIMD::MaxPerElem(vec, SIMD::Swizzle_ZWXY(vec));
		return SIMD::MaxPerElem(xz_yw_xz_yw, SIMD::Swizzle_YXWZ(xz_yw_xz_yw));
	}

	// Get minimum element
	static inline simd128_t MinElem(simd128_t vec)
	{
		const simd128_t xz_yw_xz_yw = SIMD::MinPerElem(vec, SIMD::Swizzle_ZWXY(vec));
		return SIMD::MinPerElem(xz_yw_xz_yw, SIMD::Swizzle_YXWZ(xz_yw_xz_yw));
	}

	// Compute the square root
	static inline simd128_t Sqrt(simd128_t vec)
	{
#if DESIRE_USE_SSE
		return _mm_sqrt_ps(vec);
#elif DESIRE_USE_NEON
		return SIMD::Mul(vec, SIMD::InvSqrt(vec));
#else
		return SIMD::Construct(
			std::sqrt(SIMD::GetX(vec)),
			std::sqrt(SIMD::GetY(vec)),
			std::sqrt(SIMD::GetZ(vec)),
			std::sqrt(SIMD::GetW(vec))
		);
#endif
	}

	// Compute the inverse/reciprocal square root
	static inline simd128_t InvSqrt(simd128_t vec)
	{
#if DESIRE_USE_NEON
		// Get an initial estimate then perform two Newton-Raphson iterations
		const float32x4_t invSqrt = vrsqrteq_f32(vec);
		invSqrt = SIMD::Mul(invSqrt, vrsqrtsq_f32(SIMD::Mul(vec, invSqrt), invSqrt));
		invSqrt = SIMD::Mul(invSqrt, vrsqrtsq_f32(SIMD::Mul(vec, invSqrt), invSqrt));
		return invSqrt;
#else
		return SIMD::Div(SIMD::Construct(1.0f), SIMD::Sqrt(vec));
#endif
	}

	// Blend (select) elements from a and b using the mask
	static inline simd128_t Blend(simd128_t a, simd128_t b, simd128_t mask)
	{
#if DESIRE_USE_SSE
	#if defined(__SSE4_1__)
		return _mm_blendv_ps(a, b, mask);
	#else
		return _mm_or_ps(_mm_and_ps(mask, b), _mm_andnot_ps(mask, a));
	#endif
#elif DESIRE_USE_NEON
		return vbslq_f32(reinterpret_cast<uint32x4_t>(mask), b, a);
#else
		return simd128_t(
			(mask.u32[0] & b.u32[0]) | (~mask.u32[0] & a.u32[0]),
			(mask.u32[1] & b.u32[1]) | (~mask.u32[1] & a.u32[1]),
			(mask.u32[2] & b.u32[2]) | (~mask.u32[2] & a.u32[2]),
			(mask.u32[3] & b.u32[3]) | (~mask.u32[3] & a.u32[3])
		);
#endif
	}

	// Special blends for one single precision floating-point value
	static inline simd128_t Blend_X(simd128_t to, simd128_t from)
	{
#if DESIRE_USE_SSE
		return _mm_blend_ps(to, from, 0b0001);
#elif DESIRE_USE_NEON
		return SIMD::Blend(to, from, (uint32x4_t) { 0xffffffff, 0, 0, 0 });
#else
		return SIMD::SetX(to, SIMD::GetX(from));
#endif
	}

	static inline simd128_t Blend_Y(simd128_t to, simd128_t from)
	{
#if DESIRE_USE_SSE
		return _mm_blend_ps(to, from, 0b0010);
#elif DESIRE_USE_NEON
		return SIMD::Blend(to, from, (uint32x4_t) { 0, 0xffffffff, 0, 0 });
#else
		return SIMD::SetY(to, SIMD::GetY(from));
#endif
	}

	static inline simd128_t Blend_Z(simd128_t to, simd128_t from)
	{
#if DESIRE_USE_SSE
		return _mm_blend_ps(to, from, 0b0100);
#elif DESIRE_USE_NEON
		return SIMD::Blend(to, from, (uint32x4_t) { 0, 0, 0xffffffff, 0 });
#else
		return SIMD::SetZ(to, SIMD::GetZ(from));
#endif
	}

	static inline simd128_t Blend_W(simd128_t to, simd128_t from)
	{
#if DESIRE_USE_SSE
		return _mm_blend_ps(to, from, 0b1000);
#elif DESIRE_USE_NEON
		return SIMD::Blend(to, from, (uint32x4_t) { 0, 0, 0, 0xffffffff });
#else
		return SIMD::SetW(to, SIMD::GetW(from));
#endif
	}

	// Swizzle
#if DESIRE_USE_SSE
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
	static inline simd128_t Swizzle_YXXY(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 0, 0, 1)); }
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
	static inline simd128_t Swizzle_WXZW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 2, 0, 3)); }
	static inline simd128_t Swizzle_WYZX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 2, 1, 3)); }
	static inline simd128_t Swizzle_WZXY(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 0, 2, 3)); }
	static inline simd128_t Swizzle_WZYX(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 1, 2, 3)); }
	static inline simd128_t Swizzle_WZZW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 2, 2, 3)); }
	static inline simd128_t Swizzle_WZWZ(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 2, 3)); }
	static inline simd128_t Swizzle_WWWW(simd128_t vec)		{ return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 3, 3, 3)); }
#elif DESIRE_USE_NEON
	static inline simd128_t Swizzle_XXXX(simd128_t vec)		{ return vdupq_laneq_f32(vec, 0); }
	static inline simd128_t Swizzle_XXYY(simd128_t vec)		{ return vzip1q_f32(vec, vec); }
	static inline simd128_t Swizzle_XXZZ(simd128_t vec)		{ return vtrn1q_f32(vec, vec); }
	static inline simd128_t Swizzle_XXZW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 0, 2, 3 }); }
	static inline simd128_t Swizzle_XYXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 0, 0 }); }
	static inline simd128_t Swizzle_XYXY(simd128_t vec)		{ const float32x2_t xy = vget_low_f32(vec); return vcombine_f32(xy, xy); }
	static inline simd128_t Swizzle_XYXW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 0, 3 }); }
	static inline simd128_t Swizzle_XYYX(simd128_t vec)		{ const float32x2_t xy = vget_low_f32(vec); return vcombine_f32(xy, vrev64_f32(xy)); }
	static inline simd128_t Swizzle_XYZX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 1, 2, 0 }); }
	static inline simd128_t Swizzle_XYWZ(simd128_t vec)		{ return vcombine_f32(vget_low_f32(vec), vrev64_f32(vget_high_f32(vec))); }
	static inline simd128_t Swizzle_XZXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 2, 0, 0 }); }
	static inline simd128_t Swizzle_XZXZ(simd128_t vec)		{ return vuzp1q_f32(vec, vec); }
	static inline simd128_t Swizzle_XZYX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 0, 2, 1, 0 }); }

	static inline simd128_t Swizzle_YXXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 1, 0, 0, 0 }); }
	static inline simd128_t Swizzle_YXXY(simd128_t vec)		{ const float32x2_t xy = vget_low_f32(vec); return vcombine_f32(vrev64_f32(xy), xy); }
	static inline simd128_t Swizzle_YXYX(simd128_t vec)		{ const float32x2_t yx = vrev64_f32(vget_low_f32(vec)); return vcombine_f32(yx, yx); }
	static inline simd128_t Swizzle_YXZW(simd128_t vec)		{ return vcombine_f32(vrev64_f32(vget_low_f32(vec)), vget_high_f32(vec)); }
	static inline simd128_t Swizzle_YXWZ(simd128_t vec)		{ return vrev64q_f32(vec); }
	static inline simd128_t Swizzle_YYYY(simd128_t vec)		{ return vdupq_laneq_f32(vec, 1); }
	static inline simd128_t Swizzle_YYWW(simd128_t vec)		{ return vtrn2q_f32(vec, vec); }
	static inline simd128_t Swizzle_YZXY(simd128_t vec)		{ const float32x2_t xy = vget_low_f32(vec); return vcombine_f32(vext_f32(xy, vget_high_f32(vec), 1), xy); }
	static inline simd128_t Swizzle_YZXW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 1, 2, 0, 3 }); }
	static inline simd128_t Swizzle_YZWX(simd128_t vec)		{ return vextq_f32(vec, vec, 1); }
	static inline simd128_t Swizzle_YWYW(simd128_t vec)		{ return vuzp2q_f32(vec, vec); }

	static inline simd128_t Swizzle_ZXXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 0, 0 }); }
	static inline simd128_t Swizzle_ZXYZ(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 1, 2 }); }
	static inline simd128_t Swizzle_ZXYW(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 1, 3 }); }
	static inline simd128_t Swizzle_ZXWY(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 0, 3, 1 }); }
	static inline simd128_t Swizzle_ZZYX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 2, 1, 0 }); }
	static inline simd128_t Swizzle_ZZZZ(simd128_t vec)		{ return vdupq_laneq_f32(vec, 2); }
	static inline simd128_t Swizzle_ZZWX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 2, 3, 0 }); }
	static inline simd128_t Swizzle_ZZWW(simd128_t vec)		{ return vzip2q_f32(vec, vec); }
	static inline simd128_t Swizzle_ZWXX(simd128_t vec)		{ return __builtin_shuffle(vec, (uint32x4_t){ 2, 3, 0, 0 }); }
	static inline simd128_t Swizzle_ZWXY(simd128_t vec)		{ return vextq_f32(vec, vec, 2); }
	static inline simd128_t Swizzle_ZWYX(simd128_t vec)		{ return vcombine_f32(vget_high_f32(vec), vrev64_f32(vget_low_f32(vec))); }
	static inline simd128_t Swizzle_ZWZW(simd128_t vec)		{ const float32x2_t zw = vget_high_f32(vec); return vcombine_f32(zw, zw); }
	static inline simd128_t Swizzle_ZWWZ(simd128_t vec)		{ const float32x2_t zw = vget_high_f32(vec); return vcombine_f32(zw, vrev64_f32(zw)); }

	static inline simd128_t Swizzle_WXYZ(simd128_t vec)		{ return vextq_f32(vec, vec, 3); }
	static inline simd128_t Swizzle_WXZW(simd128_t vec)		{ const float32x2_t zw = vget_high_f32(vec); return vcombine_f32(vext_f32(zw, vget_low_f32(vec), 1), zw); }
	static inline simd128_t Swizzle_WZXY(simd128_t vec)		{ return vcombine_f32(vrev64_f32(vget_high_f32(vec)), vget_low_f32(vec)); }
	static inline simd128_t Swizzle_WZYX(simd128_t vec)		{ return Swizzle_ZWXY(Swizzle_YXWZ(vec)); }
	static inline simd128_t Swizzle_WZZW(simd128_t vec)		{ const float32x2_t zw = vget_high_f32(vec); return vcombine_f32(vrev64_f32(zw), zw); }
	static inline simd128_t Swizzle_WZWZ(simd128_t vec)		{ const float32x2_t wz = vrev64_f32(vget_high_f32(vec)); return vcombine_f32(wz, wz); }
	static inline simd128_t Swizzle_WWWW(simd128_t vec)		{ return vdupq_laneq_f32(vec, 3); }
#else
	static inline simd128_t Swizzle_XXXX(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[0], vec.f32[0], vec.f32[0]); }
	static inline simd128_t Swizzle_XXYY(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[0], vec.f32[1], vec.f32[1]); }
	static inline simd128_t Swizzle_XXZZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[0], vec.f32[2], vec.f32[2]); }
	static inline simd128_t Swizzle_XXZW(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[0], vec.f32[2], vec.f32[3]); }
	static inline simd128_t Swizzle_XYXX(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[1], vec.f32[0], vec.f32[0]); }
	static inline simd128_t Swizzle_XYXY(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[1], vec.f32[0], vec.f32[1]); }
	static inline simd128_t Swizzle_XYXW(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[1], vec.f32[0], vec.f32[3]); }
	static inline simd128_t Swizzle_XYYX(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[1], vec.f32[1], vec.f32[0]); }
	static inline simd128_t Swizzle_XYZX(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[1], vec.f32[2], vec.f32[0]); }
	static inline simd128_t Swizzle_XYWZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[1], vec.f32[3], vec.f32[2]); }
	static inline simd128_t Swizzle_XZXX(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[2], vec.f32[0], vec.f32[0]); }
	static inline simd128_t Swizzle_XZXZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[2], vec.f32[0], vec.f32[2]); }
	static inline simd128_t Swizzle_XZYX(simd128_t vec)		{ return SIMD::Construct(vec.f32[0], vec.f32[2], vec.f32[1], vec.f32[0]); }

	static inline simd128_t Swizzle_YXXX(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[0], vec.f32[0], vec.f32[0]); }
	static inline simd128_t Swizzle_YXXY(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[0], vec.f32[0], vec.f32[1]); }
	static inline simd128_t Swizzle_YXYX(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[0], vec.f32[1], vec.f32[0]); }
	static inline simd128_t Swizzle_YXZW(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[0], vec.f32[2], vec.f32[3]); }
	static inline simd128_t Swizzle_YXWZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[0], vec.f32[3], vec.f32[2]); }
	static inline simd128_t Swizzle_YYYY(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[1], vec.f32[1], vec.f32[1]); }
	static inline simd128_t Swizzle_YYWW(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[1], vec.f32[3], vec.f32[3]); }
	static inline simd128_t Swizzle_YZXY(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[2], vec.f32[0], vec.f32[1]); }
	static inline simd128_t Swizzle_YZXW(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[2], vec.f32[0], vec.f32[3]); }
	static inline simd128_t Swizzle_YZWX(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[2], vec.f32[3], vec.f32[0]); }
	static inline simd128_t Swizzle_YWYW(simd128_t vec)		{ return SIMD::Construct(vec.f32[1], vec.f32[3], vec.f32[1], vec.f32[3]); }

	static inline simd128_t Swizzle_ZXXX(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[0], vec.f32[0], vec.f32[0]); }
	static inline simd128_t Swizzle_ZXYZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[0], vec.f32[1], vec.f32[2]); }
	static inline simd128_t Swizzle_ZXYW(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[0], vec.f32[1], vec.f32[3]); }
	static inline simd128_t Swizzle_ZXWY(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[0], vec.f32[3], vec.f32[1]); }
	static inline simd128_t Swizzle_ZZYX(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[2], vec.f32[1], vec.f32[0]); }
	static inline simd128_t Swizzle_ZZZZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[2], vec.f32[2], vec.f32[2]); }
	static inline simd128_t Swizzle_ZZWX(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[2], vec.f32[3], vec.f32[0]); }
	static inline simd128_t Swizzle_ZZWW(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[2], vec.f32[3], vec.f32[3]); }
	static inline simd128_t Swizzle_ZWXX(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[3], vec.f32[0], vec.f32[0]); }
	static inline simd128_t Swizzle_ZWXY(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[3], vec.f32[0], vec.f32[1]); }
	static inline simd128_t Swizzle_ZWYX(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[3], vec.f32[1], vec.f32[0]); }
	static inline simd128_t Swizzle_ZWZW(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[3], vec.f32[2], vec.f32[3]); }
	static inline simd128_t Swizzle_ZWWZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[2], vec.f32[3], vec.f32[3], vec.f32[2]); }

	static inline simd128_t Swizzle_WXYZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[3], vec.f32[0], vec.f32[1], vec.f32[2]); }
	static inline simd128_t Swizzle_WXZW(simd128_t vec)		{ return SIMD::Construct(vec.f32[3], vec.f32[0], vec.f32[2], vec.f32[3]); }
	static inline simd128_t Swizzle_WZXY(simd128_t vec)		{ return SIMD::Construct(vec.f32[3], vec.f32[2], vec.f32[0], vec.f32[1]); }
	static inline simd128_t Swizzle_WZYX(simd128_t vec)		{ return SIMD::Construct(vec.f32[3], vec.f32[2], vec.f32[1], vec.f32[0]); }
	static inline simd128_t Swizzle_WZZW(simd128_t vec)		{ return SIMD::Construct(vec.f32[3], vec.f32[2], vec.f32[2], vec.f32[3]); }
	static inline simd128_t Swizzle_WZWZ(simd128_t vec)		{ return SIMD::Construct(vec.f32[3], vec.f32[2], vec.f32[3], vec.f32[2]); }
	static inline simd128_t Swizzle_WWWW(simd128_t vec)		{ return SIMD::Construct(vec.f32[3], vec.f32[3], vec.f32[3], vec.f32[3]); }
#endif
};

// --------------------------------------------------------------------------------------------------------------------
//	Helper functions
// --------------------------------------------------------------------------------------------------------------------

#if DESIRE_USE_SSE

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
	return SIMD::Blend(SIMD::Negate(res), res, _mm_cmpeq_ps(_mm_and_ps(offset, toM128(0x2)), _mm_setzero_ps()));
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

	*s = SIMD::Blend(SIMD::Negate(*s), *s, sinMask);
	*c = SIMD::Blend(SIMD::Negate(*c), *c, cosMask);
}

#endif
