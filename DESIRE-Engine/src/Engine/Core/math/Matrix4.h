#pragma once

#include "Engine/Core/math/Vector4.h"
#include "Engine/Core/math/Matrix3.h"

class Matrix4
{
public:
	inline Matrix4()
	{
		// No initialization
	}

	inline Matrix4(const Matrix4& mat)
		: col0(mat.col0)
		, col1(mat.col1)
		, col2(mat.col2)
		, col3(mat.col3)
	{
	}

	inline Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3)
		: col0(col0)
		, col1(col1)
		, col2(col2)
		, col3(col3)
	{
	}

	inline Matrix4(const Matrix3& mat, const Vector3& translateVec)
		: col0(mat.col0, 0.0f)
		, col1(mat.col1, 0.0f)
		, col2(mat.col2, 0.0f)
		, col3(translateVec, 1.0f)
	{
	}

	inline Matrix4(const Quat& unitQuat, const Vector3& translateVec)
		: col3(translateVec, 1.0f)
	{
		Matrix3 mat = Matrix3(unitQuat);
		col0 = Vector4(mat.col0, 0.0f);
		col1 = Vector4(mat.col1, 0.0f);
		col2 = Vector4(mat.col2, 0.0f);
	}

	inline Matrix4(const float(&fptr)[16])
	{
		col0.LoadXYZW(&fptr[0]);
		col1.LoadXYZW(&fptr[4]);
		col2.LoadXYZW(&fptr[8]);
		col3.LoadXYZW(&fptr[12]);
	}

	inline void Store(float(&fptr)[16]) const
	{
		col0.StoreXYZW(&fptr[0]);
		col1.StoreXYZW(&fptr[4]);
		col2.StoreXYZW(&fptr[8]);
		col3.StoreXYZW(&fptr[12]);
	}

	inline Matrix4& SetUpper3x3(const Matrix3& mat3)
	{
		col0.SetXYZ(mat3.col0);
		col1.SetXYZ(mat3.col1);
		col2.SetXYZ(mat3.col2);
		return *this;
	}

	inline Matrix3 GetUpper3x3() const
	{
		return Matrix3(
			col0.GetXYZ(),
			col1.GetXYZ(),
			col2.GetXYZ()
		);
	}

	inline Matrix4& SetTranslation(const Vector3& translateVec)	{ col3.SetXYZ(translateVec); return *this; }
	inline Vector3 GetTranslation() const						{ return col3.GetXYZ(); }

	inline void SetCol(int idx, const Vector4& vec)				{ *(&col0 + idx) = vec; }
	inline const Vector4& GetCol(int idx) const					{ return *(&col0 + idx); }

	inline void SetRow0(const Vector4& vec)
	{
		col0.SetX(vec.GetX());
		col1.SetX(vec.GetY());
		col2.SetX(vec.GetZ());
		col3.SetX(vec.GetW());
	}

	inline Vector4 GetRow0() const								{ return Vector4(col0.GetX(), col1.GetX(), col2.GetX(), col3.GetX()); }
	inline Vector4 GetRow1() const								{ return Vector4(col0.GetY(), col1.GetY(), col2.GetY(), col3.GetY()); }
	inline Vector4 GetRow2() const								{ return Vector4(col0.GetZ(), col1.GetZ(), col2.GetZ(), col3.GetZ()); }
	inline Vector4 GetRow3() const								{ return Vector4(col0.GetW(), col1.GetW(), col2.GetW(), col3.GetW()); }

	inline Matrix4& operator =(const Matrix4& mat)
	{
		col0 = mat.col0;
		col1 = mat.col1;
		col2 = mat.col2;
		col3 = mat.col3;
		return *this;
	}

	inline Matrix4 operator -() const							{ return Matrix4(-col0, -col1, -col2, -col3); }
	inline Matrix4 operator +(const Matrix4& mat) const			{ return Matrix4(col0 + mat.col0, col1 + mat.col1, col2 + mat.col2, col3 + mat.col3); }
	inline Matrix4 operator -(const Matrix4& mat) const			{ return Matrix4(col0 - mat.col0, col1 - mat.col1, col2 - mat.col2, col3 - mat.col3); }
	inline Matrix4 operator *(float scalar) const				{ return Matrix4(col0 * scalar, col1 * scalar, col2 * scalar, col3 * scalar); }
	inline Vector4 operator *(const Vector4& vec) const;
	inline Vector4 operator *(const Vector3& vec) const;
	inline Matrix4 operator *(const Matrix4& mat) const			{ return Matrix4( *this * mat.col0, *this * mat.col1, *this * mat.col2, *this * mat.col3); }

	inline Matrix4& operator +=(const Matrix4& mat)				{ *this = *this + mat;		return *this; }
	inline Matrix4& operator -=(const Matrix4& mat)				{ *this = *this - mat;		return *this; }
	inline Matrix4& operator *=(float scalar)					{ *this = *this * scalar;	return *this; }
	inline Matrix4& operator *=(const Matrix4& mat)				{ *this = *this * mat;		return *this; }

	// Append (post-multiply) a scale transformation
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	inline Matrix4& AppendScale(const Vector3& scaleVec)
	{
		col0 *= scaleVec.GetX();
		col1 *= scaleVec.GetY();
		col2 *= scaleVec.GetZ();
		return *this;
	}

	// Prepend (pre-multiply) a scale transformation
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	inline Matrix4& PrependScale(const Vector3& scaleVec)
	{
		const Vector4 scaleVec4(scaleVec, 1.0f);
		col0 *= scaleVec4;
		col1 *= scaleVec4;
		col2 *= scaleVec4;
		col3 *= scaleVec4;
		return *this;
	}

	inline void Transpose();

	// Compute the inverse of a 4x4 matrix
	// NOTE: Result is unpredictable when the determinant of mat is equal to or near 0
	inline void Invert();

	// Compute the inverse of a 4x4 matrix, which is expected to be an affine matrix
	// NOTE: This can be used to achieve better performance than a general inverse when the specified 4x4 matrix meets the given restrictions. The result is unpredictable when the determinant of mat is equal to or near 0
	inline void AffineInvert();

	// Compute the inverse of a 4x4 matrix, which is expected to be an affine matrix with an orthogonal upper-left 3x3 submatrix
	// NOTE: This can be used to achieve better performance than a general inverse when the specified 4x4 matrix meets the given restrictions
	inline void OrthoInvert();

	inline float CalculateDeterminant() const;

	static inline Matrix4 Identity()							{ return Matrix4(Vector4::AxisX(), Vector4::AxisY(), Vector4::AxisZ(), Vector4::AxisW()); }

	static inline Matrix4 CreateTranslation(const Vector3& translateVec)
	{
		return Matrix4(
			Vector4::AxisX(),
			Vector4::AxisY(),
			Vector4::AxisZ(),
			Vector4(translateVec, 1.0f)
		);
	}

	static inline Matrix4 CreateRotationX(float radians);
	static inline Matrix4 CreateRotationY(float radians);
	static inline Matrix4 CreateRotationZ(float radians);
	static inline Matrix4 CreateRotationZYX(const Vector3& radiansXYZ);

	// Construct a 4x4 matrix to rotate around a unit-length 3-D vector
	static inline Matrix4 CreateRotation(float radians, const Vector3& unitVec);

	static inline Matrix4 CreateScale(const Vector3& scaleVec)
	{
		const Vector4 zero(0.0f);
		return Matrix4(
			Vector4(SIMD::Blend_X(zero, scaleVec)),
			Vector4(SIMD::Blend_Y(zero, scaleVec)),
			Vector4(SIMD::Blend_Z(zero, scaleVec)),
			Vector4::AxisW()
		);
	}

	Vector4 col0;
	Vector4 col1;
	Vector4 col2;
	Vector4 col3;
};

inline Matrix4 operator *(float scalar, const Matrix4& mat)
{
	return mat * scalar;
}

// --------------------------------------------------------------------------------------------------------------------

inline Vector4 Matrix4::operator *(const Vector4& vec) const
{
#if defined(DESIRE_USE_SSE)
	__m128 result;
	result = SIMD::Mul(col0, SIMD::Swizzle_XXXX(vec));
	result = SIMD::MulAdd(col1, SIMD::Swizzle_YYYY(vec), result);
	result = SIMD::MulAdd(col2, SIMD::Swizzle_ZZZZ(vec), result);
	result = SIMD::MulAdd(col3, SIMD::Swizzle_WWWW(vec), result);
	return result;
#elif defined(__ARM_NEON__)
	float32x4_t result;
	const float32x2_t vecLow = vget_low_f32(vec);
	result = vmulq_lane_f32(col0, vecLow, 0);
	result = vmlaq_lane_f32(result, col1, vecLow, 1);
	const float32x2_t vecHigh = vget_high_f32(vec);
	result = vmlaq_lane_f32(result, col2, vecHigh, 0);
	result = vmlaq_lane_f32(result, col3, vecHigh, 1);
	return result;
#else
	return Vector4(
		col0.GetX() * vec.GetX() + col1.GetX() * vec.GetY() + col2.GetX() * vec.GetZ() + col3.GetX() * vec.GetW(),
		col0.GetY() * vec.GetX() + col1.GetY() * vec.GetY() + col2.GetY() * vec.GetZ() + col3.GetY() * vec.GetW(),
		col0.GetZ() * vec.GetX() + col1.GetZ() * vec.GetY() + col2.GetZ() * vec.GetZ() + col3.GetZ() * vec.GetW(),
		col0.GetW() * vec.GetX() + col1.GetW() * vec.GetY() + col2.GetW() * vec.GetZ() + col3.GetW() * vec.GetW()
	);
#endif
}

inline Vector4 Matrix4::operator *(const Vector3& vec) const
{
#if defined(DESIRE_USE_SSE)
	__m128 result;
	result = SIMD::Mul(col0, SIMD::Swizzle_XXXX(vec));
	result = SIMD::MulAdd(col1, SIMD::Swizzle_YYYY(vec), result);
	result = SIMD::MulAdd(col2, SIMD::Swizzle_ZZZZ(vec), result);
	return result;
#elif defined(__ARM_NEON__)
	float32x4_t result;
	const float32x2_t vecLow = vget_low_f32(vec);
	result = vmulq_lane_f32(col0, vecLow, 0);
	result = vmlaq_lane_f32(result, col1, vecLow, 1);
	const float32x2_t vecHigh = vget_high_f32(vec);
	result = vmlaq_lane_f32(result, col2, vecHigh, 0);
	return result;
#else
	return Vector4(
		col0.GetX() * vec.GetX() + col1.GetX() * vec.GetY() + col2.GetX() * vec.GetZ(),
		col0.GetY() * vec.GetX() + col1.GetY() * vec.GetY() + col2.GetY() * vec.GetZ(),
		col0.GetZ() * vec.GetX() + col1.GetZ() * vec.GetY() + col2.GetZ() * vec.GetZ(),
		col0.GetW() * vec.GetX() + col1.GetW() * vec.GetY() + col2.GetW() * vec.GetZ()
	);
#endif
}

inline void Matrix4::Transpose()
{
#if defined(DESIRE_USE_SSE)
	__m128 tmp0 = _mm_unpacklo_ps(col0, col2);
	__m128 tmp1 = _mm_unpacklo_ps(col1, col3);
	__m128 tmp2 = _mm_unpackhi_ps(col0, col2);
	__m128 tmp3 = _mm_unpackhi_ps(col1, col3);
	col0 = _mm_unpacklo_ps(tmp0, tmp1);
	col1 = _mm_unpackhi_ps(tmp0, tmp1);
	col2 = _mm_unpacklo_ps(tmp2, tmp3);
	col3 = _mm_unpackhi_ps(tmp2, tmp3);
#else
	const Vector4 tmp0(col0.GetX(), col1.GetX(), col2.GetX(), col3.GetX());
	const Vector4 tmp1(col0.GetY(), col1.GetY(), col2.GetY(), col3.GetY());
	const Vector4 tmp2(col0.GetZ(), col1.GetZ(), col2.GetZ(), col3.GetZ());
	const Vector4 tmp3(col0.GetW(), col1.GetW(), col2.GetW(), col3.GetW());
	col0 = tmp0;
	col1 = tmp1;
	col2 = tmp2;
	col3 = tmp3;
#endif
}

inline void Matrix4::Invert()
{
#if defined(DESIRE_USE_SSE)
	// Calculating the minterms for the first line
	__m128 tt2 = _mm_ror_ps(col2, 1);
	__m128 Vc = SIMD::Mul(tt2, col3);									// V3' dot V4
	__m128 Va = SIMD::Mul(tt2, _mm_ror_ps(col3, 2));					// V3' dot V4"
	__m128 Vb = SIMD::Mul(tt2, _mm_ror_ps(col3, 3));					// V3' dot V4^

	__m128 r1 = SIMD::Sub(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));		// V3" dot V4^ - V3^ dot V4"
	__m128 r2 = SIMD::Sub(_mm_ror_ps(Vb, 2), Vb);						// V3^ dot V4' - V3' dot V4^
	__m128 r3 = SIMD::Sub(Va, _mm_ror_ps(Vc, 1));						// V3' dot V4" - V3" dot V4'

	__m128 sum;
	Va = _mm_ror_ps(col1, 1);
	sum = SIMD::Mul(Va, r1);
	Vb = _mm_ror_ps(col1, 2);
	sum = SIMD::MulAdd(Vb, r2, sum);
	Vc = _mm_ror_ps(col1, 3);
	sum = SIMD::MulAdd(Vc, r3, sum);

	// Calculating the determinant
	__m128 det = SIMD::Mul(sum, col0);
	det = SIMD::Add(det, _mm_movehl_ps(det, det));

	// Testing the determinant
	det = _mm_sub_ss(det, _mm_shuffle_ps(det, det, 1));

	alignas(16) static const uint32_t _vmathPNPN[4] = { 0x00000000, 0x80000000, 0x00000000, 0x80000000 };
	alignas(16) static const uint32_t _vmathNPNP[4] = { 0x80000000, 0x00000000, 0x80000000, 0x00000000 };
	const __m128 Sign_PNPN = _mm_load_ps((float*)_vmathPNPN);
	const __m128 Sign_NPNP = _mm_load_ps((float*)_vmathNPNP);
	__m128 mtL1 = _mm_xor_ps(sum, Sign_PNPN);

	// Calculating the minterms of the second line (using previous results)
	__m128 tt = _mm_ror_ps(col0, 1);
	sum = SIMD::Mul(tt, r1);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r2, sum);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r3, sum);
	__m128 mtL2 = _mm_xor_ps(sum, Sign_NPNP);

	// Calculating the minterms of the third line
	tt = _mm_ror_ps(col0, 1);
	Va = SIMD::Mul(tt, Vb);												// V1' dot V2"
	Vb = SIMD::Mul(tt, Vc);												// V1' dot V2^
	Vc = SIMD::Mul(tt, col1);											// V1' dot V2

	r1 = SIMD::Sub(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));				// V1" dot V2^ - V1^ dot V2"
	r2 = SIMD::Sub(_mm_ror_ps(Vb, 2), Vb);								// V1^ dot V2' - V1' dot V2^
	r3 = SIMD::Sub(Va, _mm_ror_ps(Vc, 1));								// V1' dot V2" - V1" dot V2'

	tt = _mm_ror_ps(col3, 1);
	sum = SIMD::Mul(tt, r1);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r2, sum);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r3, sum);
	__m128 mtL3 = _mm_xor_ps(sum, Sign_PNPN);

	// Dividing is FASTER than rcp_nr! (Because rcp_nr causes many register-memory RWs)
	const float one = 1.0f;
	__m128 rDet = _mm_div_ss(_mm_load_ss(&one), det);
	rDet = SIMD::Swizzle_XXXX(rDet);

	// Devide the first 12 minterms with the determinant
	mtL1 = SIMD::Mul(mtL1, rDet);
	mtL2 = SIMD::Mul(mtL2, rDet);
	mtL3 = SIMD::Mul(mtL3, rDet);

	// Calculate the minterms of the forth line and devide by the determinant
	tt = _mm_ror_ps(col2, 1);
	sum = SIMD::Mul(tt, r1);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r2, sum);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r3, sum);
	__m128 mtL4 = _mm_xor_ps(sum, Sign_NPNP);
	mtL4 = SIMD::Mul(mtL4, rDet);

	// Now we just have to transpose the minterms matrix
	__m128 trns0 = _mm_unpacklo_ps(mtL1, mtL2);
	__m128 trns1 = _mm_unpacklo_ps(mtL3, mtL4);
	__m128 trns2 = _mm_unpackhi_ps(mtL1, mtL2);
	__m128 trns3 = _mm_unpackhi_ps(mtL3, mtL4);
	col0 = _mm_movelh_ps(trns0, trns1);
	col1 = _mm_movehl_ps(trns1, trns0);
	col2 = _mm_movelh_ps(trns2, trns3);
	col3 = _mm_movehl_ps(trns3, trns2);
#else
	float tmp0 = col2.GetZ() * col0.GetW() - col0.GetZ() * col2.GetW();
	float tmp1 = col3.GetZ() * col1.GetW() - col1.GetZ() * col3.GetW();
	float tmp2 = col0.GetY() * col2.GetZ() - col2.GetY() * col0.GetZ();
	float tmp3 = col1.GetY() * col3.GetZ() - col3.GetY() * col1.GetZ();
	float tmp4 = col2.GetY() * col0.GetW() - col0.GetY() * col2.GetW();
	float tmp5 = col3.GetY() * col1.GetW() - col1.GetY() * col3.GetW();
	const Vector4 res0(
		col2.GetY() * tmp1 - col2.GetW() * tmp3 - col2.GetZ() * tmp5,
		col3.GetY() * tmp0 - col3.GetW() * tmp2 - col3.GetZ() * tmp4,
		col0.GetW() * tmp3 + col0.GetZ() * tmp5 - col0.GetY() * tmp1,
		col1.GetW() * tmp2 + col1.GetZ() * tmp4 - col1.GetY() * tmp0
	);
	const float invDet = 1.0f / ((((col0.GetX() * res0.GetX()) + (col1.GetX() * res0.GetY())) + (col2.GetX() * res0.GetZ())) + (col3.GetX() * res0.GetW()));
	Vector4 res1(col2.GetX() * tmp1, col3.GetX() * tmp0, col0.GetX() * tmp1, col1.GetX() * tmp0);
	Vector4 res3(col2.GetX() * tmp3, col3.GetX() * tmp2, col0.GetX() * tmp3, col1.GetX() * tmp2);
	Vector4 res2(col2.GetX() * tmp5, col3.GetX() * tmp4, col0.GetX() * tmp5, col1.GetX() * tmp4);
	tmp0 = col2.GetX() * col0.GetY() - col0.GetX() * col2.GetY();
	tmp1 = col3.GetX() * col1.GetY() - col1.GetX() * col3.GetY();
	tmp2 = col2.GetX() * col0.GetW() - col0.GetX() * col2.GetW();
	tmp3 = col3.GetX() * col1.GetW() - col1.GetX() * col3.GetW();
	tmp4 = col2.GetX() * col0.GetZ() - col0.GetX() * col2.GetZ();
	tmp5 = col3.GetX() * col1.GetZ() - col1.GetX() * col3.GetZ();
	res2.SetX(col2.GetW() * tmp1 - col2.GetY() * tmp3 + res2.GetX());
	res2.SetY(col3.GetW() * tmp0 - col3.GetY() * tmp2 + res2.GetY());
	res2.SetZ(col0.GetY() * tmp3 - col0.GetW() * tmp1 - res2.GetZ());
	res2.SetW(col1.GetY() * tmp2 - col1.GetW() * tmp0 - res2.GetW());
	res3.SetX(col2.GetY() * tmp5 - col2.GetZ() * tmp1 + res3.GetX());
	res3.SetY(col3.GetY() * tmp4 - col3.GetZ() * tmp0 + res3.GetY());
	res3.SetZ(col0.GetZ() * tmp1 - col0.GetY() * tmp5 - res3.GetZ());
	res3.SetW(col1.GetZ() * tmp0 - col1.GetY() * tmp4 - res3.GetW());
	res1.SetX(col2.GetZ() * tmp3 - col2.GetW() * tmp5 - res1.GetX());
	res1.SetY(col3.GetZ() * tmp2 - col3.GetW() * tmp4 - res1.GetY());
	res1.SetZ(col0.GetW() * tmp5 - col0.GetZ() * tmp3 + res1.GetZ());
	res1.SetW(col1.GetW() * tmp4 - col1.GetZ() * tmp2 + res1.GetW());
	col0 = res0 * invDet;
	col1 = res1 * invDet;
	col2 = res2 * invDet;
	col3 = res3 * invDet;
#endif
}

inline void Matrix4::AffineInvert()
{
#if defined(DESIRE_USE_SSE)
	__m128 inv0, inv1, inv2, inv3;
	const __m128 tmp2 = col0.GetXYZ().Cross(col1.GetXYZ());
	const __m128 tmp0 = col1.GetXYZ().Cross(col2.GetXYZ());
	const __m128 tmp1 = col2.GetXYZ().Cross(col0.GetXYZ());
	inv3 = SIMD::Negate(col3);
	__m128 dot = SIMD::Dot3(tmp2, col2);
	dot = SIMD::Swizzle_XXXX(dot);
	const __m128 invDet = _mm_rcp_ps(dot);
	const __m128 tmp3 = _mm_unpacklo_ps(tmp0, tmp2);
	const __m128 tmp4 = _mm_unpackhi_ps(tmp0, tmp2);
	inv0 = _mm_unpacklo_ps(tmp3, tmp1);
	const __m128 xxxx = SIMD::Swizzle_XXXX(inv3);
	inv1 = SIMD::Swizzle_ZZWX(tmp3);
	inv1 = SIMD::Blend_Y(inv1, tmp1);
	inv2 = SIMD::Swizzle_XYYX(tmp4);
	inv2 = SIMD::Blend_Y(inv2, SIMD::Swizzle_ZZZZ(tmp1));
	const __m128 yyyy = SIMD::Swizzle_YYYY(inv3);
	const __m128 zzzz = SIMD::Swizzle_ZZZZ(inv3);
	inv3 = SIMD::Mul(inv0, xxxx);
	inv3 = SIMD::MulAdd(inv1, yyyy, inv3);
	inv3 = SIMD::MulAdd(inv2, zzzz, inv3);
	inv0 = SIMD::Mul(inv0, invDet);
	inv1 = SIMD::Mul(inv1, invDet);
	inv2 = SIMD::Mul(inv2, invDet);
	inv3 = SIMD::Mul(inv3, invDet);
	col0 = Vector4(Vector3(inv0), 0.0f);
	col1 = Vector4(Vector3(inv1), 0.0f);
	col2 = Vector4(Vector3(inv2), 0.0f);
	col3 = Vector4(Vector3(inv3), 1.0f);
#else
	const Vector3 tmp0 = col1.GetXYZ().Cross(col2.GetXYZ());
	const Vector3 tmp1 = col2.GetXYZ().Cross(col0.GetXYZ());
	const Vector3 tmp2 = col0.GetXYZ().Cross(col1.GetXYZ());
	const float invDet = 1.0f / col2.GetXYZ().Dot(tmp2);
	const Vector3 inv0(tmp0.GetX() * invDet, tmp1.GetX() * invDet, tmp2.GetX() * invDet);
	const Vector3 inv1(tmp0.GetY() * invDet, tmp1.GetY() * invDet, tmp2.GetY() * invDet);
	const Vector3 inv2(tmp0.GetZ() * invDet, tmp1.GetZ() * invDet, tmp2.GetZ() * invDet);
	col0 = Vector4(inv0, 0.0f);
	col1 = Vector4(inv1, 0.0f);
	col2 = Vector4(inv2, 0.0f);
	col3 = Vector4(-((inv0 * col3.GetX()) + ((inv1 * col3.GetY()) + (inv2 * col3.GetZ()))), 1.0f);
#endif
}

inline void Matrix4::OrthoInvert()
{
	Vector3 inv0, inv1, inv2, inv3;

#if defined(DESIRE_USE_SSE)
	__m128 tmp0 = _mm_unpacklo_ps(col0, col2);
	__m128 tmp1 = _mm_unpackhi_ps(col0, col2);
	inv3 = SIMD::Negate(col3);
	inv0 = _mm_unpacklo_ps(tmp0, col1);
	const __m128 xxxx = SIMD::Swizzle_XXXX(inv3);
	inv1 = SIMD::Swizzle_ZZWX(tmp0);
	inv1 = SIMD::Blend_Y(inv1, col1);
	inv2 = SIMD::Swizzle_XYYX(tmp1);
	inv2 = SIMD::Blend_Y(inv2, SIMD::Swizzle_ZZZZ(col1));
	const __m128 yyyy = SIMD::Swizzle_YYYY(inv3);
	const __m128 zzzz = SIMD::Swizzle_ZZZZ(inv3);
	inv3 = SIMD::Mul(inv0, xxxx);
	inv3 = SIMD::MulAdd(inv1, yyyy, inv3);
	inv3 = SIMD::MulAdd(inv2, zzzz, inv3);
#else
	inv0 = Vector3(col0.GetX(), col1.GetX(), col2.GetX());
	inv1 = Vector3(col0.GetY(), col1.GetY(), col2.GetY());
	inv2 = Vector3(col0.GetZ(), col1.GetZ(), col2.GetZ());
	inv3 = Vector3(-(inv0 * col3.GetX() + inv1 * col3.GetY() + inv2 * col3.GetZ()));
#endif

	col0 = Vector4(inv0, 0.0f);
	col1 = Vector4(inv1, 0.0f);
	col2 = Vector4(inv2, 0.0f);
	col3 = Vector4(inv3, 1.0f);
}

inline float Matrix4::CalculateDeterminant() const
{
#if defined(DESIRE_USE_SSE)
	// Calculating the minterms for the first line
	__m128 tt2 = _mm_ror_ps(col2, 1);
	__m128 Vc = SIMD::Mul(tt2, col3);									// V3' dot V4
	__m128 Va = SIMD::Mul(tt2, _mm_ror_ps(col3, 2));					// V3' dot V4"
	__m128 Vb = SIMD::Mul(tt2, _mm_ror_ps(col3, 3));					// V3' dot V4^

	__m128 r1 = SIMD::Sub(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));		// V3" dot V4^ - V3^ dot V4"
	__m128 r2 = SIMD::Sub(_mm_ror_ps(Vb, 2), Vb);						// V3^ dot V4' - V3' dot V4^
	__m128 r3 = SIMD::Sub(Va, _mm_ror_ps(Vc, 1));						// V3' dot V4" - V3" dot V4'

	__m128 sum;
	Va = _mm_ror_ps(col1, 1);
	sum = SIMD::Mul(Va, r1);
	Vb = _mm_ror_ps(col1, 2);
	sum = SIMD::MulAdd(Vb, r2, sum);
	Vc = _mm_ror_ps(col1, 3);
	sum = SIMD::MulAdd(Vc, r3, sum);

	// Calculating the determinant
	__m128 det = SIMD::Mul(sum, col0);
	det = SIMD::Add(det, _mm_movehl_ps(det, det));

	// Testing the determinant
	det = _mm_sub_ss(det, _mm_shuffle_ps(det, det, 1));
	return _mm_cvtss_f32(det);
#else
	const float tmp0 = col2.GetZ() * col0.GetW() - col0.GetZ() * col2.GetW();
	const float tmp1 = col3.GetZ() * col1.GetW() - col1.GetZ() * col3.GetW();
	const float tmp2 = col0.GetY() * col2.GetZ() - col2.GetY() * col0.GetZ();
	const float tmp3 = col1.GetY() * col3.GetZ() - col3.GetY() * col1.GetZ();
	const float tmp4 = col2.GetY() * col0.GetW() - col0.GetY() * col2.GetW();
	const float tmp5 = col3.GetY() * col1.GetW() - col1.GetY() * col3.GetW();
	const float dx = col2.GetY() * tmp1 - col2.GetW() * tmp3 - col2.GetZ() * tmp5;
	const float dy = col3.GetY() * tmp0 - col3.GetW() * tmp2 - col3.GetZ() * tmp4;
	const float dz = col0.GetW() * tmp3 + col0.GetZ() * tmp5 - col0.GetY() * tmp1;
	const float dw = col1.GetW() * tmp2 + col1.GetZ() * tmp4 - col1.GetY() * tmp0;
	return col0.GetX() * dx + col1.GetX() * dy + col2.GetX() * dz + col3.GetX() * dw;
#endif
}

inline Matrix4 Matrix4::CreateRotationX(float radians)
{
	Vector4 vecY, vecZ;

#if defined(DESIRE_USE_SSE)
	const __m128 zero = _mm_setzero_ps();
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians), &s, &c);
	vecY = SIMD::Blend_Y(zero, c);
	vecY = SIMD::Blend_Z(vecY, s);
	vecZ = SIMD::Blend_Y(zero, SIMD::Negate(s));
	vecZ = SIMD::Blend_Z(vecZ, c);
#else
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	vecY = Vector4(0.0f, c, s, 0.0f);
	vecZ = Vector4(0.0f, -s, c, 0.0f);
#endif

	return Matrix4(
		Vector4::AxisX(),
		vecY,
		vecZ,
		Vector4::AxisW()
	);
}

inline Matrix4 Matrix4::CreateRotationY(float radians)
{
	Vector4 vecX, vecZ;

#if defined(DESIRE_USE_SSE)
	const __m128 zero = _mm_setzero_ps();
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians), &s, &c);
	vecX = SIMD::Blend_X(zero, c);
	vecX = SIMD::Blend_Z(vecX, SIMD::Negate(s));
	vecZ = SIMD::Blend_X(zero, s);
	vecZ = SIMD::Blend_Z(vecZ, c);
#else
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	vecX = Vector4(c, 0.0f, -s, 0.0f);
	vecZ = Vector4(s, 0.0f, c, 0.0f);
#endif

	return Matrix4(
		vecX,
		Vector4::AxisY(),
		vecZ,
		Vector4::AxisW()
	);
}

inline Matrix4 Matrix4::CreateRotationZ(float radians)
{
	Vector4 vecX, vecY;

#if defined(DESIRE_USE_SSE)
	const __m128 zero = _mm_setzero_ps();
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians), &s, &c);
	vecX = SIMD::Blend_X(zero, c);
	vecX = SIMD::Blend_Y(vecX, s);
	vecY = SIMD::Blend_X(zero, SIMD::Negate(s));
	vecY = SIMD::Blend_Y(vecY, c);
#else
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	vecX = Vector4(c, s, 0.0f, 0.0f);
	vecY = Vector4(-s, c, 0.0f, 0.0f);
#endif

	return Matrix4(
		vecX,
		vecY,
		Vector4::AxisZ(),
		Vector4::AxisW()
	);
}

inline Matrix4 Matrix4::CreateRotationZYX(const Vector3& radiansXYZ)
{
#if defined(DESIRE_USE_SSE)
	__m128 angles = SIMD::SetW(radiansXYZ, 0.0f);
	__m128 s, c;
	sincosf4(angles, &s, &c);
	const __m128 negS = SIMD::Negate(s);
	const __m128 Z0 = _mm_unpackhi_ps(c, s);
	__m128 Z1 = _mm_unpackhi_ps(negS, c);
	alignas(16) const uint32_t select_xyz[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
	Z1 = _mm_and_ps(Z1, _mm_load_ps((float*)select_xyz));
	const __m128 Y0 = _mm_shuffle_ps(c, negS, _MM_SHUFFLE(0, 1, 1, 1));
	const __m128 Y1 = _mm_shuffle_ps(s, c, _MM_SHUFFLE(0, 1, 1, 1));
	const __m128 X0 = SIMD::Swizzle_XXXX(s);
	const __m128 X1 = SIMD::Swizzle_XXXX(c);
	const __m128 tmp = SIMD::Mul(Z0, Y1);
	return Matrix4(
		SIMD::Mul(Z0, Y0),
		SIMD::MulAdd(Z1, X1, SIMD::Mul(tmp, X0)),
		SIMD::MulSub(Z1, X0, SIMD::Mul(tmp, X1)),
		Vector4::AxisW()
	);
#else
	const float sX = std::sin(radiansXYZ.GetX());
	const float cX = std::cos(radiansXYZ.GetX());
	const float sY = std::sin(radiansXYZ.GetY());
	const float cY = std::cos(radiansXYZ.GetY());
	const float sZ = std::sin(radiansXYZ.GetZ());
	const float cZ = std::cos(radiansXYZ.GetZ());
	const float tmp0 = cZ * sY;
	const float tmp1 = sZ * sY;
	return Matrix4(
		Vector4(cZ * cY, sZ * cY, -sY, 0.0f),
		Vector4((tmp0 * sX) - (sZ * cX), (tmp1 * sX) + (cZ * cX), cY * sX, 0.0f),
		Vector4((tmp0 * cX) + (sZ * sX), (tmp1 * cX) - (cZ * sX), cY * cX, 0.0f),
		Vector4::AxisW()
	);
#endif
}

inline Matrix4 Matrix4::CreateRotation(float radians, const Vector3& unitVec)
{
#if defined(DESIRE_USE_SSE)
	__m128 axis = unitVec;
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians), &s, &c);
	const __m128 xxxx = SIMD::Swizzle_XXXX(axis);
	const __m128 yyyy = SIMD::Swizzle_YYYY(axis);
	const __m128 zzzz = SIMD::Swizzle_ZZZZ(axis);
	const __m128 oneMinusC = SIMD::Sub(_mm_set1_ps(1.0f), c);
	const __m128 axisS = SIMD::Mul(axis, s);
	const __m128 negAxisS = SIMD::Negate(axisS);
	__m128 tmp0 = SIMD::Blend_Z(SIMD::Swizzle_XZXX(axisS), SIMD::Swizzle_YYYY(negAxisS));
	__m128 tmp1 = SIMD::Blend_X(SIMD::Swizzle_XXXX(axisS), SIMD::Swizzle_ZZZZ(negAxisS));
	__m128 tmp2 = SIMD::Blend_Y(SIMD::Swizzle_YXXX(axisS), SIMD::Swizzle_XXXX(negAxisS));
	tmp0 = SIMD::Blend_X(tmp0, c);
	tmp1 = SIMD::Blend_Y(tmp1, c);
	tmp2 = SIMD::Blend_Z(tmp2, c);
	alignas(16) const uint32_t select_xyz[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
	const __m128 mask_xyz = _mm_load_ps((float*)select_xyz);
	axis = _mm_and_ps(axis, mask_xyz);
	tmp0 = _mm_and_ps(tmp0, mask_xyz);
	tmp1 = _mm_and_ps(tmp1, mask_xyz);
	tmp2 = _mm_and_ps(tmp2, mask_xyz);
	return Matrix4(
		SIMD::MulAdd(SIMD::Mul(axis, xxxx), oneMinusC, tmp0),
		SIMD::MulAdd(SIMD::Mul(axis, yyyy), oneMinusC, tmp1),
		SIMD::MulAdd(SIMD::Mul(axis, zzzz), oneMinusC, tmp2),
		Vector4::AxisW()
	);
#else
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	const float x = unitVec.GetX();
	const float y = unitVec.GetY();
	const float z = unitVec.GetZ();
	const float xy = x * y;
	const float yz = y * z;
	const float zx = z * x;
	const float oneMinusC = (1.0f - c);
	return Matrix4(
		Vector4(((x * x) * oneMinusC) + c, (xy * oneMinusC) + (z * s), (zx * oneMinusC) - (y * s), 0.0f),
		Vector4((xy * oneMinusC) - (z * s), ((y * y) * oneMinusC) + c, (yz * oneMinusC) + (x * s), 0.0f),
		Vector4((zx * oneMinusC) + (y * s), (yz * oneMinusC) - (x * s), ((z * z) * oneMinusC) + c, 0.0f),
		Vector4::AxisW()
	);
#endif
}
