// --------------------------------------------------------------------------------------------------------------------
//	SSE implementation of quaternion class functions
// --------------------------------------------------------------------------------------------------------------------

DESIRE_FORCE_INLINE Vector3 Quat::RotateVec(const Vector3& vec) const
{
	__m128 tmp0 = _mm_shuffle_ps(mVec128, mVec128, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 tmp1 = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 tmp2 = _mm_shuffle_ps(mVec128, mVec128, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 tmp3 = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 0, 2, 1));
	const __m128 wwww = SIMD::Shuffle_WWWW(mVec128);
	__m128 qv = _mm_mul_ps(wwww, vec);
	qv = vec_madd(tmp0, tmp1, qv);
	qv = vec_nmsub(tmp2, tmp3, qv);
	const __m128 product = _mm_mul_ps(mVec128, vec);
	__m128 qw = vec_madd(_mm_ror_ps(mVec128, 1), _mm_ror_ps(vec, 1), product);
	qw = _mm_add_ps(_mm_ror_ps(product, 2), qw);
	tmp1 = _mm_shuffle_ps(qv, qv, _MM_SHUFFLE(3, 1, 0, 2));
	tmp3 = _mm_shuffle_ps(qv, qv, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 res = _mm_mul_ps(SIMD::Shuffle_XXXX(qw), mVec128);
	res = vec_madd(wwww, qv, res);
	res = vec_madd(tmp0, tmp1, res);
	res = vec_nmsub(tmp2, tmp3, res);
	return res;
}

DESIRE_FORCE_INLINE Quat Quat::Slerp(float t, const Quat& unitQuat0, const Quat& unitQuat1)
{
	__m128 cosAngle = SIMD::Dot4(unitQuat0, unitQuat1);
	__m128 selectMask = _mm_cmpgt_ps(_mm_setzero_ps(), cosAngle);
	cosAngle = _mm_blendv_ps(cosAngle, SIMD::Negate(cosAngle), selectMask);
	const __m128 start = _mm_blendv_ps(unitQuat0, SIMD::Negate(unitQuat0), selectMask);
	selectMask = _mm_cmpgt_ps(_mm_set1_ps(_VECTORMATH_SLERP_TOL), cosAngle);
	const __m128 angle = acosf4(cosAngle);
	const __m128 tttt = _mm_set1_ps(t);
	const __m128 oneMinusT = _mm_sub_ps(_mm_set1_ps(1.0f), tttt);
	__m128 angles = _mm_unpacklo_ps(_mm_set1_ps(1.0f), tttt);
	angles = _mm_unpacklo_ps(angles, oneMinusT);
	angles = vec_madd(angles, angle, _mm_setzero_ps());
	const __m128 sines = sinf4(angles);
	const __m128 scales = _mm_div_ps(sines, SIMD::Shuffle_XXXX(sines));
	const __m128 scale0 = _mm_blendv_ps(oneMinusT, SIMD::Shuffle_YYYY(scales), selectMask);
	const __m128 scale1 = _mm_blendv_ps(tttt, SIMD::Shuffle_ZZZZ(scales), selectMask);
	return vec_madd(start, scale0, _mm_mul_ps(unitQuat1, scale1));
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotation(float radians, const Vector3& unitVec)
{
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
	alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
	const __m128 mask_w = _mm_load_ps((float*)select_w);
	return _mm_blendv_ps(_mm_mul_ps(unitVec, s), c, mask_w);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationX(float radians)
{
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_w = _mm_load_ps((float*)select_w);
	const __m128 res = _mm_blendv_ps(_mm_setzero_ps(), s, mask_x);
	return _mm_blendv_ps(res, c, mask_w);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationY(float radians)
{
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_w = _mm_load_ps((float*)select_w);
	const __m128 res = _mm_blendv_ps(_mm_setzero_ps(), s, mask_y);
	return _mm_blendv_ps(res, c, mask_w);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationZ(float radians)
{
	__m128 s, c;
	sincosf4(_mm_set1_ps(radians * 0.5f), &s, &c);
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	const __m128 mask_w = _mm_load_ps((float*)select_w);
	const __m128 res = _mm_blendv_ps(_mm_setzero_ps(), s, mask_z);
	return _mm_blendv_ps(res, c, mask_w);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationFromEulerAngles(const Vector3& radiansXYZ)
{
	__m128 s, c;
	sincosf4(radiansXYZ * 0.5f, &s, &c);
	const float cZ = SIMD::GetZ(c);
	const float sZ = SIMD::GetZ(s);
	const float cX = SIMD::GetX(c);
	const float sX = SIMD::GetX(s);
	const float cY = SIMD::GetY(c);
	const float sY = SIMD::GetY(s);

	const float cYcZ = cY * cZ;
	const float sYcZ = sY * cZ;
	const float cYsZ = cY * sZ;
	const float sYsZ = sY * sZ;

	return Quat(
		sX * cYcZ - cX * sYsZ,
		cX * sYcZ + sX * cYsZ,
		cX * cYsZ - sX * sYcZ,
		cX * cYcZ + sX * sYsZ
	);
}

DESIRE_FORCE_INLINE Quat Quat::CreateRotationFromTo(const Vector3& unitVecFrom, const Vector3& unitVecTo)
{
	const __m128 cosAngle = SIMD::Dot3(unitVecFrom, unitVecTo);
	const __m128 cosAngleX2Plus2 = vec_madd(cosAngle, _mm_set1_ps(2.0f), _mm_set1_ps(2.0f));
	const __m128 recipCosHalfAngleX2 = _mm_rsqrt_ps(cosAngleX2Plus2);
	const __m128 cosHalfAngleX2 = _mm_mul_ps(recipCosHalfAngleX2, cosAngleX2Plus2);
	const Vector3 crossVec = unitVecFrom.Cross(unitVecTo);
	const __m128 res = _mm_mul_ps(crossVec, recipCosHalfAngleX2);
	alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
	const __m128 mask_w = _mm_load_ps((float*)select_w);
	return _mm_blendv_ps(res, _mm_mul_ps(cosHalfAngleX2, _mm_set1_ps(0.5f)), mask_w);
}

DESIRE_FORCE_INLINE Quat Quat::operator *(const Quat& quat) const
{
	const __m128 tmp0 = _mm_shuffle_ps(mVec128, mVec128, _MM_SHUFFLE(3, 0, 2, 1));
	const __m128 tmp1 = _mm_shuffle_ps(quat.mVec128, quat.mVec128, _MM_SHUFFLE(3, 1, 0, 2));
	const __m128 tmp2 = _mm_shuffle_ps(mVec128, mVec128, _MM_SHUFFLE(3, 1, 0, 2));
	const __m128 tmp3 = _mm_shuffle_ps(quat.mVec128, quat.mVec128, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 qv = _mm_mul_ps(SIMD::Shuffle_WWWW(mVec128), quat.mVec128);
	qv = vec_madd(SIMD::Shuffle_WWWW(quat.mVec128), mVec128, qv);
	qv = vec_madd(tmp0, tmp1, qv);
	qv = vec_nmsub(tmp2, tmp3, qv);
	const __m128 product = _mm_mul_ps(mVec128, quat.mVec128);
	const __m128 l_wxyz = _mm_ror_ps(mVec128, 3);
	const __m128 r_wxyz = _mm_ror_ps(quat.mVec128, 3);
	__m128 qw = vec_nmsub(l_wxyz, r_wxyz, product);
	const __m128 xy = vec_madd(l_wxyz, r_wxyz, product);
	qw = _mm_sub_ps(qw, _mm_ror_ps(xy, 2));
	alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
	const __m128 mask_w = _mm_load_ps((float*)select_w);
	return _mm_blendv_ps(qv, qw, mask_w);
}
