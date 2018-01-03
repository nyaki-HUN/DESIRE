// --------------------------------------------------------------------------------------------------------------------
//	NEON implementation of matrix class functions
// --------------------------------------------------------------------------------------------------------------------

DESIRE_FORCE_INLINE Matrix3::Matrix3(const Quat& unitQuat)
{
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
	col0 = Vector3(1.0f - qyqy2 - qzqz2, qxqy2 + qzqw2, qxqz2 - qyqw2);
	col1 = Vector3(qxqy2 - qzqw2, (1.0f - qxqx2) - qzqz2, qyqz2 + qxqw2);
	col2 = Vector3(qxqz2 + qyqw2, qyqz2 - qxqw2, (1.0f - qxqx2) - qyqy2);
}

DESIRE_FORCE_INLINE void Matrix3::Transpose()
{
	const Vector3 tmp0(col0.GetX(), col1.GetX(), col2.GetX());
	const Vector3 tmp1(col0.GetY(), col1.GetY(), col2.GetY());
	const Vector3 tmp2(col0.GetZ(), col1.GetZ(), col2.GetZ());
	col0 = tmp0;
	col1 = tmp1;
	col2 = tmp2;
}

DESIRE_FORCE_INLINE void Matrix3::Invert()
{
	const Vector3 tmp0 = col1.Cross(col2);
	const Vector3 tmp1 = col2.Cross(col0);
	const Vector3 tmp2 = col0.Cross(col1);
	const float detinv = (1.0f / col2.Dot(tmp2));
	col0 = Vector3(tmp0.GetX() * detinv, tmp1.GetX() * detinv, tmp2.GetX() * detinv);
	col1 = Vector3(tmp0.GetY() * detinv, tmp1.GetY() * detinv, tmp2.GetY() * detinv);
	col2 = Vector3(tmp0.GetZ() * detinv, tmp1.GetZ() * detinv, tmp2.GetZ() * detinv);
}

DESIRE_FORCE_INLINE Vector3 Matrix3::operator *(const Vector3& vec) const
{
	float32x4_t result;
	result = vmulq_lane_f32(col0, vget_low_f32(vec), 0);
	result = vmlaq_lane_f32(result, col1, vget_low_f32(vec), 1);
	result = vmlaq_lane_f32(result, col2, vget_high_f32(vec), 0);
	return result;
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationX(float radians)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
	return Matrix3(
		Vector3::AxisX(),
		Vector3(0.0f, c, s),
		Vector3(0.0f, -s, c)
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationY(float radians)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
	return Matrix3(
		Vector3(c, 0.0f, -s),
		Vector3::AxisY(),
		Vector3(s, 0.0f, c)
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationZ(float radians)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
	return Matrix3(
		Vector3(c, s, 0.0f),
		Vector3(-s, c, 0.0f),
		Vector3::AxisZ()
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationZYX(const Vector3& radiansXYZ)
{
	const float sX = sinf(radiansXYZ.GetX());
	const float cX = cosf(radiansXYZ.GetX());
	const float sY = sinf(radiansXYZ.GetY());
	const float cY = cosf(radiansXYZ.GetY());
	const float sZ = sinf(radiansXYZ.GetZ());
	const float cZ = cosf(radiansXYZ.GetZ());
	const float tmp0 = cZ * sY;
	const float tmp1 = sZ * sY;
	return Matrix3(
		Vector3(cZ * cY, sZ * cY, -sY),
		Vector3((tmp0 * sX) - (sZ * cX), (tmp1 * sX) + (cZ * cX), cY * sX),
		Vector3((tmp0 * cX) + (sZ * sX), (tmp1 * cX) - (cZ * sX), cY * cX)
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotation(float radians, const Vector3& unitVec)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
	const float x = unitVec.GetX();
	const float y = unitVec.GetY();
	const float z = unitVec.GetZ();
	const float xy = (x * y);
	const float yz = (y * z);
	const float zx = (z * x);
	const float oneMinusC = (1.0f - c);
	return Matrix3(
		Vector3((((x * x) * oneMinusC) + c), ((xy * oneMinusC) + (z * s)), ((zx * oneMinusC) - (y * s))),
		Vector3(((xy * oneMinusC) - (z * s)), (((y * y) * oneMinusC) + c), ((yz * oneMinusC) + (x * s))),
		Vector3(((zx * oneMinusC) + (y * s)), ((yz * oneMinusC) - (x * s)), (((z * z) * oneMinusC) + c))
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateScale(const Vector3& scaleVec)
{
	const float32x4_t zero = vdupq_n_f32(0.0f);
	const uint32x4_t mask_x = (uint32x4_t){ 0xffffffff, 0, 0, 0 };
	const uint32x4_t mask_y = (uint32x4_t){ 0, 0xffffffff, 0, 0 };
	const uint32x4_t mask_z = (uint32x4_t){ 0, 0, 0xffffffff, 0 };
	return Matrix3(
		SIMD::Blend(zero, scaleVec, mask_x),
		SIMD::Blend(zero, scaleVec, mask_y),
		SIMD::Blend(zero, scaleVec, mask_z)
	);
}

DESIRE_FORCE_INLINE void Matrix4::Transpose()
{
	const Vector4 tmp0(col0.GetX(), col1.GetX(), col2.GetX(), col3.GetX());
	const Vector4 tmp1(col0.GetY(), col1.GetY(), col2.GetY(), col3.GetY());
	const Vector4 tmp2(col0.GetZ(), col1.GetZ(), col2.GetZ(), col3.GetZ());
	const Vector4 tmp3(col0.GetW(), col1.GetW(), col2.GetW(), col3.GetW());
	col0 = tmp0;
	col1 = tmp1;
	col2 = tmp2;
	col3 = tmp3;
}

DESIRE_FORCE_INLINE void Matrix4::Invert()
{
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
	const float detInv = 1.0f / ((((col0.GetX() * res0.GetX()) + (col1.GetX() * res0.GetY())) + (col2.GetX() * res0.GetZ())) + (col3.GetX() * res0.GetW()));
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
	col0 = res0 * detInv;
	col1 = res1 * detInv;
	col2 = res2 * detInv;
	col3 = res3 * detInv;
}

DESIRE_FORCE_INLINE void Matrix4::AffineInvert()
{
	const Vector3 tmp0 = col1.GetXYZ().Cross(col2.GetXYZ());
	const Vector3 tmp1 = col2.GetXYZ().Cross(col0.GetXYZ());
	const Vector3 tmp2 = col0.GetXYZ().Cross(col1.GetXYZ());
	const float detinv = 1.0f / col2.GetXYZ().Dot(tmp2);
	const Vector3 inv0(tmp0.GetX() * detinv, tmp1.GetX() * detinv, tmp2.GetX() * detinv);
	const Vector3 inv1(tmp0.GetY() * detinv, tmp1.GetY() * detinv, tmp2.GetY() * detinv);
	const Vector3 inv2(tmp0.GetZ() * detinv, tmp1.GetZ() * detinv, tmp2.GetZ() * detinv);
	col0 = Vector4(inv0, 0.0f);
	col1 = Vector4(inv1, 0.0f);
	col2 = Vector4(inv2, 0.0f);
	col3 = Vector4(-((inv0 * col3.GetX()) + ((inv1 * col3.GetY()) + (inv2 * col3.GetZ()))), 1.0f);
}

DESIRE_FORCE_INLINE void Matrix4::OrthoInvert()
{
	const Vector3 inv0(col0.GetX(), col1.GetX(), col2.GetX());
	const Vector3 inv1(col0.GetY(), col1.GetY(), col2.GetY());
	const Vector3 inv2(col0.GetZ(), col1.GetZ(), col2.GetZ());
	col0 = Vector4(inv0, 0.0f);
	col1 = Vector4(inv1, 0.0f);
	col2 = Vector4(inv2, 0.0f);
	col3 = Vector4(-((inv0 * col3.GetX()) + ((inv1 * col3.GetY()) + (inv2 * col3.GetZ()))), 1.0f);
}

DESIRE_FORCE_INLINE float Matrix4::CalculateDeterminant() const
{
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
}

DESIRE_FORCE_INLINE Vector4 Matrix4::operator *(const Vector4& vec) const
{
	float32x4_t result;
	result = vmulq_lane_f32(col0, vget_low_f32(vec), 0);
	result = vmlaq_lane_f32(result, col1, vget_low_f32(vec), 1);
	result = vmlaq_lane_f32(result, col2, vget_high_f32(vec), 0);
	result = vmlaq_lane_f32(result, col3, vget_high_f32(vec), 1);
	return result;
}

DESIRE_FORCE_INLINE Vector4 Matrix4::operator *(const Vector3& vec) const
{
	float32x4_t result;
	result = vmulq_lane_f32(col0, vget_low_f32(vec), 0);
	result = vmlaq_lane_f32(result, col1, vget_low_f32(vec), 1);
	result = vmlaq_lane_f32(result, col2, vget_high_f32(vec), 0);
	return result;
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationX(float radians)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
	return Matrix4(
		Vector4::AxisX(),
		Vector4(0.0f, c, s, 0.0f),
		Vector4(0.0f, -s, c, 0.0f),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationY(float radians)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
	return Matrix4(
		Vector4(c, 0.0f, -s, 0.0f),
		Vector4::AxisY(),
		Vector4(s, 0.0f, c, 0.0f),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationZ(float radians)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
	return Matrix4(
		Vector4(c, s, 0.0f, 0.0f),
		Vector4(-s, c, 0.0f, 0.0f),
		Vector4::AxisZ(),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationZYX(const Vector3& radiansXYZ)
{
	const float sX = sinf(radiansXYZ.GetX());
	const float cX = cosf(radiansXYZ.GetX());
	const float sY = sinf(radiansXYZ.GetY());
	const float cY = cosf(radiansXYZ.GetY());
	const float sZ = sinf(radiansXYZ.GetZ());
	const float cZ = cosf(radiansXYZ.GetZ());
	const float tmp0 = cZ * sY;
	const float tmp1 = sZ * sY;
	return Matrix4(
		Vector4(cZ * cY, sZ * cY, -sY, 0.0f),
		Vector4(((tmp0 * sX) - (sZ * cX)), ((tmp1 * sX) + (cZ * cX)), cY * sX, 0.0f),
		Vector4(((tmp0 * cX) + (sZ * sX)), ((tmp1 * cX) - (cZ * sX)), cY * cX, 0.0f),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotation(float radians, const Vector3& unitVec)
{
	const float s = sinf(radians);
	const float c = cosf(radians);
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
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateScale(const Vector3& scaleVec)
{
	const float32x4_t zero = vdupq_n_f32(0.0f);
	const uint32x4_t mask_x = (uint32x4_t) { 0xffffffff, 0, 0, 0 };
	const uint32x4_t mask_y = (uint32x4_t) { 0, 0xffffffff, 0, 0 };
	const uint32x4_t mask_z = (uint32x4_t) { 0, 0, 0xffffffff, 0 };
	return Matrix4(
		SIMD::Blend(zero, scaleVec, mask_x),
		SIMD::Blend(zero, scaleVec, mask_y),
		SIMD::Blend(zero, scaleVec, mask_z),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Quat::Quat(const Matrix3& tfrm)
{
	float q[4];

	float xx = tfrm.col0.GetX();
	float yx = tfrm.col0.GetY();
	float zx = tfrm.col0.GetZ();
	float xy = tfrm.col1.GetX();
	float yy = tfrm.col1.GetY();
	float zy = tfrm.col1.GetZ();
	float xz = tfrm.col2.GetX();
	float yz = tfrm.col2.GetY();
	float zz = tfrm.col2.GetZ();

	const float negTrace = (xx + yy + zz < 0.0f);
	const int ZgtX = zz > xx;
	const int ZgtY = zz > yy;
	const int YgtX = yy > xx;
	const int largestXorY = (!ZgtX || !ZgtY) && negTrace;
	const int largestYorZ = (YgtX || ZgtX) && negTrace;
	const int largestZorX = (ZgtY || !YgtX) && negTrace;

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
	const float scale = 0.5f * (1.0f / sqrtf(radicand));

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
}
