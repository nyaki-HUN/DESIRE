#pragma once

#include "Engine/Core/Math/Quat.h"

class Matrix3
{
public:
	inline Matrix3() = default;

	inline Matrix3(const Matrix3& mat)
		: m_col0(mat.m_col0)
		, m_col1(mat.m_col1)
		, m_col2(mat.m_col2)
	{
	}

	inline Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2)
		: m_col0(col0)
		, m_col1(col1)
		, m_col2(col2)
	{
	}

	explicit inline Matrix3(const Quat& unitQuat);

	inline void SetCol(int32_t idx, const Vector3& vec)			{ *(&m_col0 + idx) = vec; }
	inline const Vector3& GetCol(int32_t idx) const				{ return *(&m_col0 + idx); }

	inline void SetRow0(const Vector3& vec)
	{
		m_col0.SetX(vec.GetX());
		m_col1.SetX(vec.GetY());
		m_col2.SetX(vec.GetZ());
	}

	inline Vector3 GetRow0() const								{ return Vector3(m_col0.GetX(), m_col1.GetX(), m_col2.GetX()); }
	inline Vector3 GetRow1() const								{ return Vector3(m_col0.GetY(), m_col1.GetY(), m_col2.GetY()); }
	inline Vector3 GetRow2() const								{ return Vector3(m_col0.GetZ(), m_col1.GetZ(), m_col2.GetZ()); }

	inline Matrix3& operator =(const Matrix3& mat)
	{
		m_col0 = mat.m_col0;
		m_col1 = mat.m_col1;
		m_col2 = mat.m_col2;
		return *this;
	}

	inline Matrix3 operator -() const							{ return Matrix3(-m_col0, -m_col1, -m_col2); }
	inline Matrix3 operator +(const Matrix3& mat) const			{ return Matrix3(m_col0 + mat.m_col0, m_col1 + mat.m_col1, m_col2 + mat.m_col2); }
	inline Matrix3 operator -(const Matrix3& mat) const			{ return Matrix3(m_col0 - mat.m_col0, m_col1 - mat.m_col1, m_col2 - mat.m_col2); }
	inline Matrix3 operator *(const Matrix3& mat) const			{ return Matrix3(*this * mat.m_col0, *this * mat.m_col1, *this * mat.m_col2); }
	inline Vector3 operator *(const Vector3& vec) const;
	inline Matrix3 operator *(float scalar) const				{ return Matrix3(m_col0 * scalar, m_col1 * scalar, m_col2 * scalar); }

	inline Matrix3& operator +=(const Matrix3& mat)				{ *this = *this + mat;		return *this; }
	inline Matrix3& operator -=(const Matrix3& mat)				{ *this = *this - mat;		return *this; }
	inline Matrix3& operator *=(float scalar)					{ *this = *this * scalar;	return *this; }
	inline Matrix3& operator *=(const Matrix3& mat)				{ *this = *this * mat;		return *this; }

	// Append (post-multiply) a scale transformation
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	inline Matrix3& AppendScale(const Vector3& scaleVec)
	{
		m_col0 *= scaleVec.GetX();
		m_col1 *= scaleVec.GetY();
		m_col2 *= scaleVec.GetZ();
		return *this;
	}

	// Prepend (pre-multiply) a scale transformation
	// NOTE: Faster than creating and multiplying a scale transformation matrix
	inline Matrix3& PrependScale(const Vector3& scaleVec)
	{
		m_col0 *= scaleVec;
		m_col1 *= scaleVec;
		m_col2 *= scaleVec;
		return *this;
	}

	inline void Transpose();

	// Compute the inverse of a 3x3 matrix
	// NOTE: Result is unpredictable when the determinant of mat is equal to or near 0
	inline void Invert();

	inline float CalculateDeterminant() const
	{
		return m_col2.Dot(m_col0.Cross(m_col1));
	}

	static inline Matrix3 Identity()						{ return Matrix3(Vector3::AxisX(), Vector3::AxisY(), Vector3::AxisZ()); }

	static inline Matrix3 CreateRotationX(float radians);
	static inline Matrix3 CreateRotationY(float radians);
	static inline Matrix3 CreateRotationZ(float radians);
	static inline Matrix3 CreateRotationZYX(const Vector3& radiansXYZ);

	// Construct a 3x3 matrix to rotate around a unit-length 3-D vector
	static inline Matrix3 CreateRotation(float radians, const Vector3& unitVec);

	static inline Matrix3 CreateScale(const Vector3& scaleVec)
	{
		const Vector3 zero = Vector3::Zero();
		return Matrix3(
			Vector3(SIMD::Blend_X(zero, scaleVec)),
			Vector3(SIMD::Blend_Y(zero, scaleVec)),
			Vector3(SIMD::Blend_Z(zero, scaleVec))
		);
	}

	Vector3 m_col0;
	Vector3 m_col1;
	Vector3 m_col2;
};

inline Matrix3 operator *(float scalar, const Matrix3& mat)
{
	return mat * scalar;
}

// --------------------------------------------------------------------------------------------------------------------

inline Matrix3::Matrix3(const Quat& unitQuat)
{
#if DESIRE_USE_SSE
	const __m128 xyzw_2 = SIMD::Add(unitQuat, unitQuat);
	const __m128 wwww = SIMD::Swizzle_WWWW(unitQuat);
	const __m128 yzxw = SIMD::Swizzle_YZXW(unitQuat);
	const __m128 zxyw = SIMD::Swizzle_ZXYW(unitQuat);
	const __m128 yzxw_2 = SIMD::Swizzle_YZXW(xyzw_2);
	const __m128 zxyw_2 = SIMD::Swizzle_ZXYW(xyzw_2);

	__m128 tmp0 = SIMD::Mul(yzxw_2, wwww);									// tmp0 = 2yw, 2zw, 2xw, 2w2
	__m128 tmp1 = SIMD::Sub(SIMD::Construct(1.0f), SIMD::Mul(yzxw, yzxw_2));	// tmp1 = 1 - 2y2, 1 - 2z2, 1 - 2x2, 1 - 2w2
	__m128 tmp2 = SIMD::Mul(yzxw, xyzw_2);									// tmp2 = 2xy, 2yz, 2xz, 2w2
	tmp0 = SIMD::Add(SIMD::Mul(zxyw, xyzw_2), tmp0);						// tmp0 = 2yw + 2zx, 2zw + 2xy, 2xw + 2yz, 2w2 + 2w2
	tmp1 = SIMD::Sub(tmp1, SIMD::Mul(zxyw, zxyw_2));						// tmp1 = 1 - 2y2 - 2z2, 1 - 2z2 - 2x2, 1 - 2x2 - 2y2, 1 - 2w2 - 2w2
	tmp2 = SIMD::Sub(tmp2, SIMD::Mul(zxyw_2, wwww));						// tmp2 = 2xy - 2zw, 2yz - 2xw, 2xz - 2yw, 2w2 - 2w2

	const __m128 tmp3 = SIMD::Blend_X(tmp0, tmp1);
	const __m128 tmp4 = SIMD::Blend_X(tmp1, tmp2);
	const __m128 tmp5 = SIMD::Blend_X(tmp2, tmp0);
	m_col0 = SIMD::Blend_Z(tmp3, tmp2);
	m_col1 = SIMD::Blend_Z(tmp4, tmp0);
	m_col2 = SIMD::Blend_Z(tmp5, tmp1);
#else
	const float qx = unitQuat.GetX();
	const float qy = unitQuat.GetY();
	const float qz = unitQuat.GetZ();
	const float qw = unitQuat.GetW();
	const float qx2 = qx + qx;
	const float qy2 = qy + qy;
	const float qz2 = qz + qz;
	const float qxqx2 = qx * qx2;
	const float qxqy2 = qx * qy2;
	const float qxqz2 = qx * qz2;
	const float qxqw2 = qw * qx2;
	const float qyqy2 = qy * qy2;
	const float qyqz2 = qy * qz2;
	const float qyqw2 = qw * qy2;
	const float qzqz2 = qz * qz2;
	const float qzqw2 = qw * qz2;
	m_col0 = Vector3(1.0f - qyqy2 - qzqz2, qxqy2 + qzqw2, qxqz2 - qyqw2);
	m_col1 = Vector3(qxqy2 - qzqw2, (1.0f - qxqx2) - qzqz2, qyqz2 + qxqw2);
	m_col2 = Vector3(qxqz2 + qyqw2, qyqz2 - qxqw2, (1.0f - qxqx2) - qyqy2);
#endif
}

inline Vector3 Matrix3::operator *(const Vector3& vec) const
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
	return Vector3(
		m_col0.GetX() * vec.GetX() + m_col1.GetX() * vec.GetY() + m_col2.GetX() * vec.GetZ(),
		m_col0.GetY() * vec.GetX() + m_col1.GetY() * vec.GetY() + m_col2.GetY() * vec.GetZ(),
		m_col0.GetZ() * vec.GetX() + m_col1.GetZ() * vec.GetY() + m_col2.GetZ() * vec.GetZ()
	);
#endif
}

inline void Matrix3::Transpose()
{
#if DESIRE_USE_SSE
	__m128 tmp0 = _mm_unpacklo_ps(m_col0, m_col2);
	__m128 tmp1 = _mm_unpackhi_ps(m_col0, m_col2);
	m_col0 = _mm_unpacklo_ps(tmp0, m_col1);

	tmp1 = SIMD::Swizzle_XYYX(tmp1);
	m_col2 = SIMD::Blend_Y(tmp1, SIMD::Swizzle_ZZZZ(m_col1));

	tmp0 = SIMD::Swizzle_ZZWX(tmp0);
	m_col1 = SIMD::Blend_Y(tmp0, m_col1);
#else
	const Vector3 tmp0(m_col0.GetX(), m_col1.GetX(), m_col2.GetX());
	const Vector3 tmp1(m_col0.GetY(), m_col1.GetY(), m_col2.GetY());
	const Vector3 tmp2(m_col0.GetZ(), m_col1.GetZ(), m_col2.GetZ());
	m_col0 = tmp0;
	m_col1 = tmp1;
	m_col2 = tmp2;
#endif
}

inline void Matrix3::Invert()
{
	const Vector3 tmp2 = m_col0.Cross(m_col1);
	const Vector3 tmp0 = m_col1.Cross(m_col2);
	const Vector3 tmp1 = m_col2.Cross(m_col0);

#if DESIRE_USE_SSE
	__m128 dot = SIMD::Dot3(tmp2, m_col2);
	const __m128 invDet = _mm_rcp_ps(dot);
	const __m128 tmp3 = _mm_unpacklo_ps(tmp0, tmp2);
	const __m128 tmp4 = _mm_unpackhi_ps(tmp0, tmp2);
	const __m128 inv0 = _mm_unpacklo_ps(tmp3, tmp1);
	__m128 inv1 = SIMD::Swizzle_ZZWX(tmp3);
	__m128 inv2 = SIMD::Swizzle_XYYX(tmp4);
	inv1 = SIMD::Blend_Y(inv1, tmp1);
	inv2 = SIMD::Blend_Y(inv2, SIMD::Swizzle_ZZZZ(tmp1));
	m_col0 = SIMD::Mul(inv0, invDet);
	m_col1 = SIMD::Mul(inv1, invDet);
	m_col2 = SIMD::Mul(inv2, invDet);
#else
	const float invDet = 1.0f / m_col2.Dot(tmp2);
	m_col0 = Vector3(tmp0.GetX() * invDet, tmp1.GetX() * invDet, tmp2.GetX() * invDet);
	m_col1 = Vector3(tmp0.GetY() * invDet, tmp1.GetY() * invDet, tmp2.GetY() * invDet);
	m_col2 = Vector3(tmp0.GetZ() * invDet, tmp1.GetZ() * invDet, tmp2.GetZ() * invDet);
#endif
}

inline Matrix3 Matrix3::Matrix3::CreateRotationX(float radians)
{
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	return Matrix3(
		Vector3::AxisX(),
		Vector3(0.0f, c, s),
		Vector3(0.0f, -s, c)
	);
}

inline Matrix3 Matrix3::CreateRotationY(float radians)
{
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	return Matrix3(
		Vector3(c, 0.0f, -s),
		Vector3::AxisY(),
		Vector3(s, 0.0f, c)
	);
}

inline Matrix3 Matrix3::CreateRotationZ(float radians)
{
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	return Matrix3(
		Vector3(c, s, 0.0f),
		Vector3(-s, c, 0.0f),
		Vector3::AxisZ()
	);
}

inline Matrix3 Matrix3::CreateRotationZYX(const Vector3& radiansXYZ)
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
	return Matrix3(
		SIMD::Mul(Z0, Y0),
		SIMD::MulAdd(Z1, X1, SIMD::Mul(tmp, X0)),
		SIMD::MulSub(Z1, X0, SIMD::Mul(tmp, X1))
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
	return Matrix3(
		Vector3(cZ * cY,				sZ * cY,				-sY),
		Vector3(tmp0 * sX - sZ * cX,	tmp1 * sX + cZ * cX,	cY * sX),
		Vector3(tmp0 * cX + sZ * sX,	tmp1 * cX - cZ * sX,	cY * cX)
	);
#endif
}

inline Matrix3 Matrix3::CreateRotation(float radians, const Vector3& unitVec)
{
#if DESIRE_USE_SSE
	__m128 s, c;
	sincosf4(SIMD::Construct(radians), &s, &c);
	const __m128 xxxx = SIMD::Swizzle_XXXX(unitVec);
	const __m128 yyyy = SIMD::Swizzle_YYYY(unitVec);
	const __m128 zzzz = SIMD::Swizzle_ZZZZ(unitVec);
	const __m128 oneMinusC = SIMD::Sub(Vector4::One(), c);
	const __m128 axisS = SIMD::Mul(unitVec, s);
	const __m128 negAxisS = SIMD::Negate(axisS);
	__m128 tmp0 = SIMD::Blend_Z(SIMD::Swizzle_XZXX(axisS), SIMD::Swizzle_YYYY(negAxisS));
	__m128 tmp1 = SIMD::Blend_X(SIMD::Swizzle_XXXX(axisS), SIMD::Swizzle_ZZZZ(negAxisS));
	__m128 tmp2 = SIMD::Blend_Y(SIMD::Swizzle_YXXX(axisS), SIMD::Swizzle_XXXX(negAxisS));
	tmp0 = SIMD::Blend_X(tmp0, c);
	tmp1 = SIMD::Blend_Y(tmp1, c);
	tmp2 = SIMD::Blend_Z(tmp2, c);
	return Matrix3(
		SIMD::MulAdd(SIMD::Mul(unitVec, xxxx), oneMinusC, tmp0),
		SIMD::MulAdd(SIMD::Mul(unitVec, yyyy), oneMinusC, tmp1),
		SIMD::MulAdd(SIMD::Mul(unitVec, zzzz), oneMinusC, tmp2)
	);
#else
	const float s = std::sin(radians);
	const float c = std::cos(radians);
	const float x = unitVec.GetX();
	const float y = unitVec.GetY();
	const float z = unitVec.GetZ();
	const float oneMinusC = 1.0f - c;
	const float xyC = x * y * oneMinusC;
	const float yzC = y * z * oneMinusC;
	const float zxC = z * x * oneMinusC;
	return Matrix3(
		Vector3((x * x * oneMinusC) + c, xyC + (z * s), zxC - (y * s)),
		Vector3(xyC - z * s, (y * y * oneMinusC) + c, yzC + (x * s)),
		Vector3(zxC + (y * s), yzC - (x * s), (z * z * oneMinusC) + c)
	);
#endif
}

// --------------------------------------------------------------------------------------------------------------------

inline Quat::Quat(const Matrix3& rotMat)
{
#if DESIRE_USE_SSE
	/* four cases: */
	/* trace > 0 */
	/* else */
	/*    xx largest diagonal element */
	/*    yy largest diagonal element */
	/*    zz largest diagonal element */

	/* compute quaternion for each case */

	const __m128 xx_yy = SIMD::Blend_Y(rotMat.m_col0, rotMat.m_col1);
	const __m128 xx_yy_zz_xx = SIMD::Blend_Z(SIMD::Swizzle_XYXX(xx_yy), rotMat.m_col2);
	const __m128 yy_zz_xx_yy = SIMD::Swizzle_YZXY(xx_yy_zz_xx);
	const __m128 zz_xx_yy_zz = SIMD::Swizzle_ZXYZ(xx_yy_zz_xx);

	const __m128 diagSum = SIMD::Add(SIMD::Add(xx_yy_zz_xx, yy_zz_xx_yy), zz_xx_yy_zz);
	const __m128 diagDiff = SIMD::Sub(SIMD::Sub(xx_yy_zz_xx, yy_zz_xx_yy), zz_xx_yy_zz);
	const __m128 radicand = SIMD::Add(SIMD::Blend_W(diagDiff, diagSum), SIMD::Construct(1.0f));
	const __m128 scale = SIMD::Mul(SIMD::InvSqrt(radicand), 0.5f);

	__m128 zy_xz_yx = SIMD::Blend_Z(rotMat.m_col0, rotMat.m_col1);					// zy_xz_yx = 00 01 12 03
	zy_xz_yx = SIMD::Swizzle_ZZYX(zy_xz_yx);									// zy_xz_yx = 12 12 01 00
	zy_xz_yx = SIMD::Blend_Y(zy_xz_yx, SIMD::Swizzle_XXXX(rotMat.m_col2));		// zy_xz_yx = 12 20 01 00
	__m128 yz_zx_xy = SIMD::Blend_X(rotMat.m_col0, rotMat.m_col1);					// yz_zx_xy = 10 01 02 03
	yz_zx_xy = SIMD::Swizzle_XZXX(yz_zx_xy);									// yz_zx_xy = 10 02 10 10
	yz_zx_xy = SIMD::Blend_X(yz_zx_xy, SIMD::Swizzle_YYYY(rotMat.m_col2));		// yz_zx_xy = 21 02 10 10

	const __m128 sum = SIMD::Add(zy_xz_yx, yz_zx_xy);
	const __m128 diff = SIMD::Sub(zy_xz_yx, yz_zx_xy);

	__m128 res0 = SIMD::Blend_W(SIMD::Swizzle_XZYX(sum), SIMD::Swizzle_XXXX(diff));
	__m128 res1 = SIMD::Blend_W(SIMD::Swizzle_ZXXX(sum), SIMD::Swizzle_YYYY(diff));
	__m128 res2 = SIMD::Blend_W(SIMD::Swizzle_YXXX(sum), SIMD::Swizzle_ZZZZ(diff));
	__m128 res3 = diff;
	res0 = SIMD::Blend_X(res0, radicand);
	res1 = SIMD::Blend_Y(res1, radicand);
	res2 = SIMD::Blend_Z(res2, radicand);
	res3 = SIMD::Blend_W(res3, radicand);
	res0 = SIMD::Mul(res0, SIMD::Swizzle_XXXX(scale));
	res1 = SIMD::Mul(res1, SIMD::Swizzle_YYYY(scale));
	res2 = SIMD::Mul(res2, SIMD::Swizzle_ZZZZ(scale));
	res3 = SIMD::Mul(res3, SIMD::Swizzle_WWWW(scale));

	/* determine case and select answer */

	const __m128 xx = SIMD::Swizzle_XXXX(rotMat.m_col0);
	const __m128 yy = SIMD::Swizzle_YYYY(rotMat.m_col1);
	const __m128 zz = SIMD::Swizzle_ZZZZ(rotMat.m_col2);
	__m128 res = SIMD::Blend(res0, res1, _mm_cmpgt_ps(yy, xx));
	res = SIMD::Blend(res, res2, _mm_and_ps(_mm_cmpgt_ps(zz, xx), _mm_cmpgt_ps(zz, yy)));
	res = SIMD::Blend(res, res3, _mm_cmpgt_ps(SIMD::Swizzle_XXXX(diagSum), SIMD::ConstructZero()));
	*this = res;
#else
	float q[4];

	float xx = rotMat.m_col0.GetX();
	float yx = rotMat.m_col0.GetY();
	float zx = rotMat.m_col0.GetZ();
	float xy = rotMat.m_col1.GetX();
	float yy = rotMat.m_col1.GetY();
	float zy = rotMat.m_col1.GetZ();
	float xz = rotMat.m_col2.GetX();
	float yz = rotMat.m_col2.GetY();
	float zz = rotMat.m_col2.GetZ();

	const float negTrace = (xx + yy + zz < 0.0f);
	const int32_t  ZgtX = zz > xx;
	const int32_t  ZgtY = zz > yy;
	const int32_t  YgtX = yy > xx;
	const int32_t  largestXorY = (!ZgtX || !ZgtY) && negTrace;
	const int32_t  largestYorZ = (YgtX || ZgtX) && negTrace;
	const int32_t  largestZorX = (ZgtY || !YgtX) && negTrace;

	if(largestXorY)
	{
		zz = -zz;
		xy = -xy;
	}
	if(largestYorZ)
	{
		xx = -xx;
		yz = -yz;
	}
	if(largestZorX)
	{
		yy = -yy;
		zx = -zx;
	}

	const float radicand = xx + yy + zz + 1.0f;
	const float scale = 0.5f * (1.0f / std::sqrt(radicand));

	float tmp[4] =
	{
		(zy - yz) * scale,
		(xz - zx) * scale,
		(yx - xy) * scale,
		radicand * scale
	};
	q[0] = tmp[0];
	q[1] = tmp[1];
	q[2] = tmp[2];
	q[3] = tmp[3];

	if(largestXorY)
	{
		q[0] = tmp[3];
		q[1] = tmp[2];
		q[2] = tmp[1];
		q[3] = tmp[0];
	}
	if(largestYorZ)
	{
		tmp[0] = q[0];
		tmp[2] = q[2];
		q[0] = q[1];
		q[1] = tmp[0];
		q[2] = q[3];
		q[3] = tmp[2];
	}

	LoadXYZW(q);
#endif
}
