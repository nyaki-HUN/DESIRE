#pragma once

#include "Engine/Core/Math/Vector4.h"
#include "Engine/Core/Math/Matrix3.h"

class Matrix4
{
public:
	inline Matrix4() = default;

	inline Matrix4(const Matrix4& mat)
		: m_col0(mat.m_col0)
		, m_col1(mat.m_col1)
		, m_col2(mat.m_col2)
		, m_col3(mat.m_col3)
	{
	}

	inline Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3)
		: m_col0(col0)
		, m_col1(col1)
		, m_col2(col2)
		, m_col3(col3)
	{
	}

	inline Matrix4(const Matrix3& mat, const Vector3& translateVec)
		: m_col0(mat.m_col0, 0.0f)
		, m_col1(mat.m_col1, 0.0f)
		, m_col2(mat.m_col2, 0.0f)
		, m_col3(translateVec, 1.0f)
	{
	}

	inline Matrix4(const Quat& unitQuat, const Vector3& translateVec)
		: m_col3(translateVec, 1.0f)
	{
		Matrix3 mat = Matrix3(unitQuat);
		m_col0 = Vector4(mat.m_col0, 0.0f);
		m_col1 = Vector4(mat.m_col1, 0.0f);
		m_col2 = Vector4(mat.m_col2, 0.0f);
	}

	inline Matrix4(const float(&fptr)[16])
	{
		m_col0.LoadXYZW(&fptr[0]);
		m_col1.LoadXYZW(&fptr[4]);
		m_col2.LoadXYZW(&fptr[8]);
		m_col3.LoadXYZW(&fptr[12]);
	}

	inline void Store(float(&fptr)[16]) const
	{
		m_col0.StoreXYZW(&fptr[0]);
		m_col1.StoreXYZW(&fptr[4]);
		m_col2.StoreXYZW(&fptr[8]);
		m_col3.StoreXYZW(&fptr[12]);
	}

	inline Matrix4& SetUpper3x3(const Matrix3& mat3)
	{
		m_col0.SetXYZ(mat3.m_col0);
		m_col1.SetXYZ(mat3.m_col1);
		m_col2.SetXYZ(mat3.m_col2);
		return *this;
	}

	inline Matrix3 GetUpper3x3() const
	{
		return Matrix3(
			m_col0.GetXYZ(),
			m_col1.GetXYZ(),
			m_col2.GetXYZ()
		);
	}

	inline Matrix4& SetTranslation(const Vector3& translateVec)	{ m_col3.SetXYZ(translateVec); return *this; }
	inline Vector3 GetTranslation() const						{ return m_col3.GetXYZ(); }

	inline void SetCol(int32_t idx, const Vector4& vec)			{ *(&m_col0 + idx) = vec; }
	inline const Vector4& GetCol(int32_t idx) const				{ return *(&m_col0 + idx); }

	inline void SetRow0(const Vector4& vec)
	{
		m_col0.SetX(vec.GetX());
		m_col1.SetX(vec.GetY());
		m_col2.SetX(vec.GetZ());
		m_col3.SetX(vec.GetW());
	}

	inline Vector4 GetRow0() const								{ return Vector4(m_col0.GetX(), m_col1.GetX(), m_col2.GetX(), m_col3.GetX()); }
	inline Vector4 GetRow1() const								{ return Vector4(m_col0.GetY(), m_col1.GetY(), m_col2.GetY(), m_col3.GetY()); }
	inline Vector4 GetRow2() const								{ return Vector4(m_col0.GetZ(), m_col1.GetZ(), m_col2.GetZ(), m_col3.GetZ()); }
	inline Vector4 GetRow3() const								{ return Vector4(m_col0.GetW(), m_col1.GetW(), m_col2.GetW(), m_col3.GetW()); }

	inline Matrix4& operator =(const Matrix4& mat)
	{
		m_col0 = mat.m_col0;
		m_col1 = mat.m_col1;
		m_col2 = mat.m_col2;
		m_col3 = mat.m_col3;
		return *this;
	}

	inline Matrix4 operator -() const							{ return Matrix4(-m_col0, -m_col1, -m_col2, -m_col3); }
	inline Matrix4 operator +(const Matrix4& mat) const			{ return Matrix4(m_col0 + mat.m_col0, m_col1 + mat.m_col1, m_col2 + mat.m_col2, m_col3 + mat.m_col3); }
	inline Matrix4 operator -(const Matrix4& mat) const			{ return Matrix4(m_col0 - mat.m_col0, m_col1 - mat.m_col1, m_col2 - mat.m_col2, m_col3 - mat.m_col3); }
	inline Matrix4 operator *(const Matrix4& mat) const			{ return Matrix4(*this * mat.m_col0, *this * mat.m_col1, *this * mat.m_col2, *this * mat.m_col3); }
	inline Vector4 operator *(const Vector4& vec) const;
	inline Vector4 operator *(const Vector3& vec) const;
	inline Matrix4 operator *(float scalar) const				{ return Matrix4(m_col0 * scalar, m_col1 * scalar, m_col2 * scalar, m_col3 * scalar); }

	inline Matrix4& operator +=(const Matrix4& mat)				{ *this = *this + mat;		return *this; }
	inline Matrix4& operator -=(const Matrix4& mat)				{ *this = *this - mat;		return *this; }
	inline Matrix4& operator *=(float scalar)					{ *this = *this * scalar;	return *this; }
	inline Matrix4& operator *=(const Matrix4& mat)				{ *this = *this * mat;		return *this; }

	// Append (post-multiply) a scale transformation
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	inline Matrix4& AppendScale(const Vector3& scaleVec)
	{
		m_col0 *= scaleVec.GetX();
		m_col1 *= scaleVec.GetY();
		m_col2 *= scaleVec.GetZ();
		return *this;
	}

	// Prepend (pre-multiply) a scale transformation
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	inline Matrix4& PrependScale(const Vector3& scaleVec)
	{
		const Vector4 scaleVec4(scaleVec, 1.0f);
		m_col0 *= scaleVec4;
		m_col1 *= scaleVec4;
		m_col2 *= scaleVec4;
		m_col3 *= scaleVec4;
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
	static inline Matrix4 CreateRotation(float radians, const Vector3& unitVec)	{ return Matrix4(Matrix3::CreateRotation(radians, unitVec), Vector3::Zero()); }

	static inline Matrix4 CreateScale(const Vector3& scaleVec)
	{
		const Vector4 zero = Vector4::Zero();
		return Matrix4(
			Vector4(SIMD::Blend_X(zero, scaleVec)),
			Vector4(SIMD::Blend_Y(zero, scaleVec)),
			Vector4(SIMD::Blend_Z(zero, scaleVec)),
			Vector4::AxisW()
		);
	}

	Vector4 m_col0;
	Vector4 m_col1;
	Vector4 m_col2;
	Vector4 m_col3;
};

inline Matrix4 operator *(float scalar, const Matrix4& mat)
{
	return mat * scalar;
}

// --------------------------------------------------------------------------------------------------------------------

inline Vector4 Matrix4::operator *(const Vector4& vec) const
{
#if DESIRE_USE_SSE
	__m128 result;
	result = SIMD::Mul(m_col0, SIMD::Swizzle_XXXX(vec));
	result = SIMD::MulAdd(m_col1, SIMD::Swizzle_YYYY(vec), result);
	result = SIMD::MulAdd(m_col2, SIMD::Swizzle_ZZZZ(vec), result);
	result = SIMD::MulAdd(m_col3, SIMD::Swizzle_WWWW(vec), result);
	return result;
#elif DESIRE_USE_NEON
	float32x4_t result;
	const float32x2_t vecLow = vget_low_f32(vec);
	result = vmulq_lane_f32(m_col0, vecLow, 0);
	result = vmlaq_lane_f32(result, m_col1, vecLow, 1);
	const float32x2_t vecHigh = vget_high_f32(vec);
	result = vmlaq_lane_f32(result, m_col2, vecHigh, 0);
	result = vmlaq_lane_f32(result, m_col3, vecHigh, 1);
	return result;
#else
	return Vector4(
		m_col0.GetX() * vec.GetX() + m_col1.GetX() * vec.GetY() + m_col2.GetX() * vec.GetZ() + m_col3.GetX() * vec.GetW(),
		m_col0.GetY() * vec.GetX() + m_col1.GetY() * vec.GetY() + m_col2.GetY() * vec.GetZ() + m_col3.GetY() * vec.GetW(),
		m_col0.GetZ() * vec.GetX() + m_col1.GetZ() * vec.GetY() + m_col2.GetZ() * vec.GetZ() + m_col3.GetZ() * vec.GetW(),
		m_col0.GetW() * vec.GetX() + m_col1.GetW() * vec.GetY() + m_col2.GetW() * vec.GetZ() + m_col3.GetW() * vec.GetW()
	);
#endif
}

inline Vector4 Matrix4::operator *(const Vector3& vec) const
{
#if DESIRE_USE_SSE
	__m128 result;
	result = SIMD::Mul(m_col0, SIMD::Swizzle_XXXX(vec));
	result = SIMD::MulAdd(m_col1, SIMD::Swizzle_YYYY(vec), result);
	result = SIMD::MulAdd(m_col2, SIMD::Swizzle_ZZZZ(vec), result);
	return result;
#elif DESIRE_USE_NEON
	float32x4_t result;
	const float32x2_t vecLow = vget_low_f32(vec);
	result = vmulq_lane_f32(m_col0, vecLow, 0);
	result = vmlaq_lane_f32(result, m_col1, vecLow, 1);
	const float32x2_t vecHigh = vget_high_f32(vec);
	result = vmlaq_lane_f32(result, m_col2, vecHigh, 0);
	return result;
#else
	return Vector4(
		m_col0.GetX() * vec.GetX() + m_col1.GetX() * vec.GetY() + m_col2.GetX() * vec.GetZ(),
		m_col0.GetY() * vec.GetX() + m_col1.GetY() * vec.GetY() + m_col2.GetY() * vec.GetZ(),
		m_col0.GetZ() * vec.GetX() + m_col1.GetZ() * vec.GetY() + m_col2.GetZ() * vec.GetZ(),
		m_col0.GetW() * vec.GetX() + m_col1.GetW() * vec.GetY() + m_col2.GetW() * vec.GetZ()
	);
#endif
}

inline void Matrix4::Transpose()
{
#if DESIRE_USE_SSE
	__m128 tmp0 = _mm_unpacklo_ps(m_col0, m_col2);
	__m128 tmp1 = _mm_unpacklo_ps(m_col1, m_col3);
	__m128 tmp2 = _mm_unpackhi_ps(m_col0, m_col2);
	__m128 tmp3 = _mm_unpackhi_ps(m_col1, m_col3);
	m_col0 = _mm_unpacklo_ps(tmp0, tmp1);
	m_col1 = _mm_unpackhi_ps(tmp0, tmp1);
	m_col2 = _mm_unpacklo_ps(tmp2, tmp3);
	m_col3 = _mm_unpackhi_ps(tmp2, tmp3);
#else
	const Vector4 tmp0(m_col0.GetX(), m_col1.GetX(), m_col2.GetX(), m_col3.GetX());
	const Vector4 tmp1(m_col0.GetY(), m_col1.GetY(), m_col2.GetY(), m_col3.GetY());
	const Vector4 tmp2(m_col0.GetZ(), m_col1.GetZ(), m_col2.GetZ(), m_col3.GetZ());
	const Vector4 tmp3(m_col0.GetW(), m_col1.GetW(), m_col2.GetW(), m_col3.GetW());
	m_col0 = tmp0;
	m_col1 = tmp1;
	m_col2 = tmp2;
	m_col3 = tmp3;
#endif
}

inline void Matrix4::Invert()
{
#if DESIRE_USE_SSE
	// Calculating the minterms for the first line
	__m128 tt2 = _mm_ror_ps(m_col2, 1);
	__m128 Vc = SIMD::Mul(tt2, m_col3);									// V3' dot V4
	__m128 Va = SIMD::Mul(tt2, _mm_ror_ps(m_col3, 2));					// V3' dot V4"
	__m128 Vb = SIMD::Mul(tt2, _mm_ror_ps(m_col3, 3));					// V3' dot V4^

	__m128 r1 = SIMD::Sub(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));		// V3" dot V4^ - V3^ dot V4"
	__m128 r2 = SIMD::Sub(_mm_ror_ps(Vb, 2), Vb);						// V3^ dot V4' - V3' dot V4^
	__m128 r3 = SIMD::Sub(Va, _mm_ror_ps(Vc, 1));						// V3' dot V4" - V3" dot V4'

	__m128 sum;
	Va = _mm_ror_ps(m_col1, 1);
	sum = SIMD::Mul(Va, r1);
	Vb = _mm_ror_ps(m_col1, 2);
	sum = SIMD::MulAdd(Vb, r2, sum);
	Vc = _mm_ror_ps(m_col1, 3);
	sum = SIMD::MulAdd(Vc, r3, sum);

	// Calculating the determinant
	__m128 det = SIMD::Mul(sum, m_col0);
	det = SIMD::Add(det, _mm_movehl_ps(det, det));

	// Testing the determinant
	det = _mm_sub_ss(det, _mm_shuffle_ps(det, det, 1));

	alignas(16) const uint32_t _vmathPNPN[4] = { 0x00000000, 0x80000000, 0x00000000, 0x80000000 };
	alignas(16) const uint32_t _vmathNPNP[4] = { 0x80000000, 0x00000000, 0x80000000, 0x00000000 };
	const __m128 Sign_PNPN = _mm_load_ps(reinterpret_cast<const float*>(_vmathPNPN));
	const __m128 Sign_NPNP = _mm_load_ps(reinterpret_cast<const float*>(_vmathNPNP));
	__m128 mtL1 = _mm_xor_ps(sum, Sign_PNPN);

	// Calculating the minterms of the second line (using previous results)
	__m128 tt = _mm_ror_ps(m_col0, 1);
	sum = SIMD::Mul(tt, r1);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r2, sum);
	tt = _mm_ror_ps(tt, 1);
	sum = SIMD::MulAdd(tt, r3, sum);
	__m128 mtL2 = _mm_xor_ps(sum, Sign_NPNP);

	// Calculating the minterms of the third line
	tt = _mm_ror_ps(m_col0, 1);
	Va = SIMD::Mul(tt, Vb);												// V1' dot V2"
	Vb = SIMD::Mul(tt, Vc);												// V1' dot V2^
	Vc = SIMD::Mul(tt, m_col1);											// V1' dot V2

	r1 = SIMD::Sub(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));				// V1" dot V2^ - V1^ dot V2"
	r2 = SIMD::Sub(_mm_ror_ps(Vb, 2), Vb);								// V1^ dot V2' - V1' dot V2^
	r3 = SIMD::Sub(Va, _mm_ror_ps(Vc, 1));								// V1' dot V2" - V1" dot V2'

	tt = _mm_ror_ps(m_col3, 1);
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
	tt = _mm_ror_ps(m_col2, 1);
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
	m_col0 = _mm_movelh_ps(trns0, trns1);
	m_col1 = _mm_movehl_ps(trns1, trns0);
	m_col2 = _mm_movelh_ps(trns2, trns3);
	m_col3 = _mm_movehl_ps(trns3, trns2);
#else
	float tmp0 = m_col2.GetZ() * m_col0.GetW() - m_col0.GetZ() * m_col2.GetW();
	float tmp1 = m_col3.GetZ() * m_col1.GetW() - m_col1.GetZ() * m_col3.GetW();
	float tmp2 = m_col0.GetY() * m_col2.GetZ() - m_col2.GetY() * m_col0.GetZ();
	float tmp3 = m_col1.GetY() * m_col3.GetZ() - m_col3.GetY() * m_col1.GetZ();
	float tmp4 = m_col2.GetY() * m_col0.GetW() - m_col0.GetY() * m_col2.GetW();
	float tmp5 = m_col3.GetY() * m_col1.GetW() - m_col1.GetY() * m_col3.GetW();
	const Vector4 res0(
		m_col2.GetY() * tmp1 - m_col2.GetW() * tmp3 - m_col2.GetZ() * tmp5,
		m_col3.GetY() * tmp0 - m_col3.GetW() * tmp2 - m_col3.GetZ() * tmp4,
		m_col0.GetW() * tmp3 + m_col0.GetZ() * tmp5 - m_col0.GetY() * tmp1,
		m_col1.GetW() * tmp2 + m_col1.GetZ() * tmp4 - m_col1.GetY() * tmp0
	);
	const float invDet = 1.0f / ((((m_col0.GetX() * res0.GetX()) + (m_col1.GetX() * res0.GetY())) + (m_col2.GetX() * res0.GetZ())) + (m_col3.GetX() * res0.GetW()));
	Vector4 res1(m_col2.GetX() * tmp1, m_col3.GetX() * tmp0, m_col0.GetX() * tmp1, m_col1.GetX() * tmp0);
	Vector4 res3(m_col2.GetX() * tmp3, m_col3.GetX() * tmp2, m_col0.GetX() * tmp3, m_col1.GetX() * tmp2);
	Vector4 res2(m_col2.GetX() * tmp5, m_col3.GetX() * tmp4, m_col0.GetX() * tmp5, m_col1.GetX() * tmp4);
	tmp0 = m_col2.GetX() * m_col0.GetY() - m_col0.GetX() * m_col2.GetY();
	tmp1 = m_col3.GetX() * m_col1.GetY() - m_col1.GetX() * m_col3.GetY();
	tmp2 = m_col2.GetX() * m_col0.GetW() - m_col0.GetX() * m_col2.GetW();
	tmp3 = m_col3.GetX() * m_col1.GetW() - m_col1.GetX() * m_col3.GetW();
	tmp4 = m_col2.GetX() * m_col0.GetZ() - m_col0.GetX() * m_col2.GetZ();
	tmp5 = m_col3.GetX() * m_col1.GetZ() - m_col1.GetX() * m_col3.GetZ();
	res2.SetX(m_col2.GetW() * tmp1 - m_col2.GetY() * tmp3 + res2.GetX());
	res2.SetY(m_col3.GetW() * tmp0 - m_col3.GetY() * tmp2 + res2.GetY());
	res2.SetZ(m_col0.GetY() * tmp3 - m_col0.GetW() * tmp1 - res2.GetZ());
	res2.SetW(m_col1.GetY() * tmp2 - m_col1.GetW() * tmp0 - res2.GetW());
	res3.SetX(m_col2.GetY() * tmp5 - m_col2.GetZ() * tmp1 + res3.GetX());
	res3.SetY(m_col3.GetY() * tmp4 - m_col3.GetZ() * tmp0 + res3.GetY());
	res3.SetZ(m_col0.GetZ() * tmp1 - m_col0.GetY() * tmp5 - res3.GetZ());
	res3.SetW(m_col1.GetZ() * tmp0 - m_col1.GetY() * tmp4 - res3.GetW());
	res1.SetX(m_col2.GetZ() * tmp3 - m_col2.GetW() * tmp5 - res1.GetX());
	res1.SetY(m_col3.GetZ() * tmp2 - m_col3.GetW() * tmp4 - res1.GetY());
	res1.SetZ(m_col0.GetW() * tmp5 - m_col0.GetZ() * tmp3 + res1.GetZ());
	res1.SetW(m_col1.GetW() * tmp4 - m_col1.GetZ() * tmp2 + res1.GetW());
	m_col0 = res0 * invDet;
	m_col1 = res1 * invDet;
	m_col2 = res2 * invDet;
	m_col3 = res3 * invDet;
#endif
}

inline void Matrix4::AffineInvert()
{
#if DESIRE_USE_SSE
	__m128 inv0, inv1, inv2, inv3;
	const __m128 tmp2 = m_col0.GetXYZ().Cross(m_col1.GetXYZ());
	const __m128 tmp0 = m_col1.GetXYZ().Cross(m_col2.GetXYZ());
	const __m128 tmp1 = m_col2.GetXYZ().Cross(m_col0.GetXYZ());
	inv3 = SIMD::Negate(m_col3);
	__m128 dot = SIMD::Dot3(tmp2, m_col2);
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
	m_col0 = Vector4(Vector3(inv0), 0.0f);
	m_col1 = Vector4(Vector3(inv1), 0.0f);
	m_col2 = Vector4(Vector3(inv2), 0.0f);
	m_col3 = Vector4(Vector3(inv3), 1.0f);
#else
	const Vector3 tmp0 = m_col1.GetXYZ().Cross(m_col2.GetXYZ());
	const Vector3 tmp1 = m_col2.GetXYZ().Cross(m_col0.GetXYZ());
	const Vector3 tmp2 = m_col0.GetXYZ().Cross(m_col1.GetXYZ());
	const float invDet = 1.0f / m_col2.GetXYZ().Dot(tmp2);
	const Vector3 inv0(tmp0.GetX() * invDet, tmp1.GetX() * invDet, tmp2.GetX() * invDet);
	const Vector3 inv1(tmp0.GetY() * invDet, tmp1.GetY() * invDet, tmp2.GetY() * invDet);
	const Vector3 inv2(tmp0.GetZ() * invDet, tmp1.GetZ() * invDet, tmp2.GetZ() * invDet);
	m_col0 = Vector4(inv0, 0.0f);
	m_col1 = Vector4(inv1, 0.0f);
	m_col2 = Vector4(inv2, 0.0f);
	m_col3 = Vector4(-((inv0 * m_col3.GetX()) + ((inv1 * m_col3.GetY()) + (inv2 * m_col3.GetZ()))), 1.0f);
#endif
}

inline void Matrix4::OrthoInvert()
{
	Vector3 inv0, inv1, inv2, inv3;

#if DESIRE_USE_SSE
	__m128 tmp0 = _mm_unpacklo_ps(m_col0, m_col2);
	__m128 tmp1 = _mm_unpackhi_ps(m_col0, m_col2);
	inv3 = SIMD::Negate(m_col3);
	inv0 = _mm_unpacklo_ps(tmp0, m_col1);
	const __m128 xxxx = SIMD::Swizzle_XXXX(inv3);
	inv1 = SIMD::Swizzle_ZZWX(tmp0);
	inv1 = SIMD::Blend_Y(inv1, m_col1);
	inv2 = SIMD::Swizzle_XYYX(tmp1);
	inv2 = SIMD::Blend_Y(inv2, SIMD::Swizzle_ZZZZ(m_col1));
	const __m128 yyyy = SIMD::Swizzle_YYYY(inv3);
	const __m128 zzzz = SIMD::Swizzle_ZZZZ(inv3);
	inv3 = SIMD::Mul(inv0, xxxx);
	inv3 = SIMD::MulAdd(inv1, yyyy, inv3);
	inv3 = SIMD::MulAdd(inv2, zzzz, inv3);
#else
	inv0 = Vector3(m_col0.GetX(), m_col1.GetX(), m_col2.GetX());
	inv1 = Vector3(m_col0.GetY(), m_col1.GetY(), m_col2.GetY());
	inv2 = Vector3(m_col0.GetZ(), m_col1.GetZ(), m_col2.GetZ());
	inv3 = Vector3(-(inv0 * m_col3.GetX() + inv1 * m_col3.GetY() + inv2 * m_col3.GetZ()));
#endif

	m_col0 = Vector4(inv0, 0.0f);
	m_col1 = Vector4(inv1, 0.0f);
	m_col2 = Vector4(inv2, 0.0f);
	m_col3 = Vector4(inv3, 1.0f);
}

inline float Matrix4::CalculateDeterminant() const
{
#if DESIRE_USE_SSE
	// Calculating the minterms for the first line
	__m128 tt2 = _mm_ror_ps(m_col2, 1);
	__m128 Vc = SIMD::Mul(tt2, m_col3);									// V3' dot V4
	__m128 Va = SIMD::Mul(tt2, _mm_ror_ps(m_col3, 2));					// V3' dot V4"
	__m128 Vb = SIMD::Mul(tt2, _mm_ror_ps(m_col3, 3));					// V3' dot V4^

	__m128 r1 = SIMD::Sub(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));		// V3" dot V4^ - V3^ dot V4"
	__m128 r2 = SIMD::Sub(_mm_ror_ps(Vb, 2), Vb);						// V3^ dot V4' - V3' dot V4^
	__m128 r3 = SIMD::Sub(Va, _mm_ror_ps(Vc, 1));						// V3' dot V4" - V3" dot V4'

	__m128 sum;
	Va = _mm_ror_ps(m_col1, 1);
	sum = SIMD::Mul(Va, r1);
	Vb = _mm_ror_ps(m_col1, 2);
	sum = SIMD::MulAdd(Vb, r2, sum);
	Vc = _mm_ror_ps(m_col1, 3);
	sum = SIMD::MulAdd(Vc, r3, sum);

	// Calculating the determinant
	__m128 det = SIMD::Mul(sum, m_col0);
	det = SIMD::Add(det, _mm_movehl_ps(det, det));

	// Testing the determinant
	det = _mm_sub_ss(det, _mm_shuffle_ps(det, det, 1));
	return SIMD::GetX(det);
#else
	const float tmp0 = m_col2.GetZ() * m_col0.GetW() - m_col0.GetZ() * m_col2.GetW();
	const float tmp1 = m_col3.GetZ() * m_col1.GetW() - m_col1.GetZ() * m_col3.GetW();
	const float tmp2 = m_col0.GetY() * m_col2.GetZ() - m_col2.GetY() * m_col0.GetZ();
	const float tmp3 = m_col1.GetY() * m_col3.GetZ() - m_col3.GetY() * m_col1.GetZ();
	const float tmp4 = m_col2.GetY() * m_col0.GetW() - m_col0.GetY() * m_col2.GetW();
	const float tmp5 = m_col3.GetY() * m_col1.GetW() - m_col1.GetY() * m_col3.GetW();
	const float dx = m_col2.GetY() * tmp1 - m_col2.GetW() * tmp3 - m_col2.GetZ() * tmp5;
	const float dy = m_col3.GetY() * tmp0 - m_col3.GetW() * tmp2 - m_col3.GetZ() * tmp4;
	const float dz = m_col0.GetW() * tmp3 + m_col0.GetZ() * tmp5 - m_col0.GetY() * tmp1;
	const float dw = m_col1.GetW() * tmp2 + m_col1.GetZ() * tmp4 - m_col1.GetY() * tmp0;
	return m_col0.GetX() * dx + m_col1.GetX() * dy + m_col2.GetX() * dz + m_col3.GetX() * dw;
#endif
}

inline Matrix4 Matrix4::CreateRotationX(float radians)
{
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	return Matrix4(
		Vector4::AxisX(),
		Vector4(0.0f, c, s, 0.0f),
		Vector4(0.0f, -s, c, 0.0f),
		Vector4::AxisW()
	);
}

inline Matrix4 Matrix4::CreateRotationY(float radians)
{
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	return Matrix4(
		Vector4(c, 0.0f, -s, 0.0f),
		Vector4::AxisY(),
		Vector4(s, 0.0f, c, 0.0f),
		Vector4::AxisW()
	);
}

inline Matrix4 Matrix4::CreateRotationZ(float radians)
{
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	return Matrix4(
		Vector4(c, s, 0.0f, 0.0f),
		Vector4(-s, c, 0.0f, 0.0f),
		Vector4::AxisZ(),
		Vector4::AxisW()
	);
}

inline Matrix4 Matrix4::CreateRotationZYX(const Vector3& radiansXYZ)
{
#if DESIRE_USE_SSE
	__m128 angles = SIMD::SetW(radiansXYZ, 0.0f);
	__m128 s, c;
	sincosf4(angles, &s, &c);
	const __m128 negS = SIMD::Negate(s);
	const __m128 Z0 = _mm_unpackhi_ps(c, s);
	__m128 Z1 = _mm_unpackhi_ps(negS, c);
	alignas(16) const uint32_t mask_xyz[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 };
	Z1 = _mm_and_ps(Z1, _mm_load_ps(reinterpret_cast<const float*>(mask_xyz)));
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
