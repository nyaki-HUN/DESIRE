// --------------------------------------------------------------------------------------------------------------------
//	SSE implementation of matrix class functions
// --------------------------------------------------------------------------------------------------------------------

DESIRE_FORCE_INLINE Matrix3::Matrix3(const Quat& unitQuat)
{
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_z = _mm_load_ps((float*)select_z);

	__m128 xyzw_2 = _mm_add_ps(unitQuat, unitQuat);
	__m128 wwww = SIMD::Shuffle_WWWW(unitQuat);
	__m128 yzxw = SIMD::Shuffle_YZXW(unitQuat);
	__m128 zxyw = _mm_shuffle_ps(unitQuat, unitQuat, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 yzxw_2 = SIMD::Shuffle_YZXW(xyzw_2);
	__m128 zxyw_2 = _mm_shuffle_ps(xyzw_2, xyzw_2, _MM_SHUFFLE(3, 1, 0, 2));

	__m128 tmp0 = _mm_mul_ps(yzxw_2, wwww);									// tmp0 = 2yw, 2zw, 2xw, 2w2
	__m128 tmp1 = _mm_sub_ps(_mm_set1_ps(1.0f), _mm_mul_ps(yzxw, yzxw_2));	// tmp1 = 1 - 2y2, 1 - 2z2, 1 - 2x2, 1 - 2w2
	__m128 tmp2 = _mm_mul_ps(yzxw, xyzw_2);									// tmp2 = 2xy, 2yz, 2xz, 2w2
	tmp0 = _mm_add_ps(_mm_mul_ps(zxyw, xyzw_2), tmp0);						// tmp0 = 2yw + 2zx, 2zw + 2xy, 2xw + 2yz, 2w2 + 2w2
	tmp1 = _mm_sub_ps(tmp1, _mm_mul_ps(zxyw, zxyw_2));						// tmp1 = 1 - 2y2 - 2z2, 1 - 2z2 - 2x2, 1 - 2x2 - 2y2, 1 - 2w2 - 2w2
	tmp2 = _mm_sub_ps(tmp2, _mm_mul_ps(zxyw_2, wwww));						// tmp2 = 2xy - 2zw, 2yz - 2xw, 2xz - 2yw, 2w2 -2w2

	__m128 tmp3 = SIMD::Blend(tmp0, tmp1, mask_x);
	__m128 tmp4 = SIMD::Blend(tmp1, tmp2, mask_x);
	__m128 tmp5 = SIMD::Blend(tmp2, tmp0, mask_x);
	col0 = SIMD::Blend(tmp3, tmp2, mask_z);
	col1 = SIMD::Blend(tmp4, tmp0, mask_z);
	col2 = SIMD::Blend(tmp5, tmp1, mask_z);
}

DESIRE_FORCE_INLINE void Matrix3::Transpose()
{
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	const __m128 mask_y = _mm_load_ps((float*)select_y);

	__m128 tmp0 = _mm_unpacklo_ps(col0, col2);
	__m128 tmp1 = _mm_unpackhi_ps(col0, col2);
	col0 = _mm_unpacklo_ps(tmp0, col1);

	tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(0, 1, 1, 0));
	col2 = SIMD::Blend(tmp1, SIMD::Shuffle_ZZZZ(col1), mask_y);

	tmp0 = _mm_shuffle_ps(tmp0, tmp0, _MM_SHUFFLE(0, 3, 2, 2));
	col1 = SIMD::Blend(tmp0, col1, mask_y);
}

DESIRE_FORCE_INLINE void Matrix3::Invert()
{
	const __m128 tmp2 = col0.Cross(col1);
	const __m128 tmp0 = col1.Cross(col2);
	const __m128 tmp1 = col2.Cross(col0);
	__m128 dot = SIMD::Dot3(tmp2, col2);
	dot = SIMD::Shuffle_XXXX(dot);
	const __m128 invdet = _mm_rcp_ps(dot);
	const __m128 tmp3 = _mm_unpacklo_ps(tmp0, tmp2);
	const __m128 tmp4 = _mm_unpackhi_ps(tmp0, tmp2);
	const __m128 inv0 = _mm_unpacklo_ps(tmp3, tmp1);
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	__m128 inv1 = _mm_shuffle_ps(tmp3, tmp3, _MM_SHUFFLE(0, 3, 2, 2));
	__m128 inv2 = _mm_shuffle_ps(tmp4, tmp4, _MM_SHUFFLE(0, 1, 1, 0));
	inv1 = SIMD::Blend(inv1, tmp1, mask_y);
	inv2 = SIMD::Blend(inv2, SIMD::Shuffle_ZZZZ(tmp1), mask_y);
	col0 = _mm_mul_ps(inv0, invdet);
	col1 = _mm_mul_ps(inv1, invdet);
	col2 = _mm_mul_ps(inv2, invdet);
}

DESIRE_FORCE_INLINE Vector3 Matrix3::operator *(const Vector3& vec) const
{
	return Vector3(
		_mm_add_ps(
			_mm_add_ps(_mm_mul_ps(col0, SIMD::Shuffle_XXXX(vec)), _mm_mul_ps(col1, SIMD::Shuffle_YYYY(vec))),
			_mm_mul_ps(col2, SIMD::Shuffle_ZZZZ(vec)))
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationX(float radians)
{
	__m128 s, c, res1, res2;
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	const __m128 zero = _mm_setzero_ps();
	sincosf4(_mm_set1_ps(radians), &s, &c);
	res1 = SIMD::Blend(zero, c, mask_y);
	res1 = SIMD::Blend(res1, s, mask_z);
	res2 = SIMD::Blend(zero, SIMD::Negate(s), mask_y);
	res2 = SIMD::Blend(res2, c, mask_z);
	return Matrix3(
		Vector3::AxisX(),
		res1,
		res2
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationY(float radians)
{
	__m128 s, c, res0, res2;
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	const __m128 zero = _mm_setzero_ps();
	sincosf4(_mm_set1_ps(radians), &s, &c);
	res0 = SIMD::Blend(zero, c, mask_x);
	res0 = SIMD::Blend(res0, SIMD::Negate(s), mask_z);
	res2 = SIMD::Blend(zero, s, mask_x);
	res2 = SIMD::Blend(res2, c, mask_z);
	return Matrix3(
		res0,
		Vector3::AxisY(),
		res2
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationZ(float radians)
{
	__m128 s, c, res0, res1;
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 zero = _mm_setzero_ps();
	sincosf4(_mm_set1_ps(radians), &s, &c);
	res0 = SIMD::Blend(zero, c, mask_x);
	res0 = SIMD::Blend(res0, s, mask_y);
	res1 = SIMD::Blend(zero, SIMD::Negate(s), mask_x);
	res1 = SIMD::Blend(res1, c, mask_y);
	return Matrix3(
		res0,
		res1,
		Vector3::AxisZ()
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotationZYX(const Vector3& radiansXYZ)
{
	__m128 s, c;
	__m128 angles = Vector4(radiansXYZ, 0.0f);
	sincosf4(angles, &s, &c);
	__m128 negS = SIMD::Negate(s);
	__m128 Z0 = _mm_unpackhi_ps(c, s);
	__m128 Z1 = _mm_unpackhi_ps(negS, c);
	alignas(16) const uint32_t select_xyz[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
	Z1 = _mm_and_ps(Z1, _mm_load_ps((float *)select_xyz));
	__m128 Y0 = _mm_shuffle_ps(c, negS, _MM_SHUFFLE(0, 1, 1, 1));
	__m128 Y1 = _mm_shuffle_ps(s, c, _MM_SHUFFLE(0, 1, 1, 1));
	__m128 X0 = SIMD::Shuffle_XXXX(s);
	__m128 X1 = SIMD::Shuffle_XXXX(c);
	__m128 tmp = _mm_mul_ps(Z0, Y1);
	return Matrix3(
		_mm_mul_ps(Z0, Y0),
		vec_madd(Z1, X1, _mm_mul_ps(tmp, X0)),
		vec_nmsub(Z1, X0, _mm_mul_ps(tmp, X1))
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateRotation(float radians, const Vector3& unitVec)
{
	__m128 s, c, tmp0, tmp1, tmp2;
	__m128 axis = unitVec;
	sincosf4(_mm_set1_ps(radians), &s, &c);
	__m128 xxxx = SIMD::Shuffle_XXXX(axis);
	__m128 yyyy = SIMD::Shuffle_YYYY(axis);
	__m128 zzzz = SIMD::Shuffle_ZZZZ(axis);
	__m128 oneMinusC = _mm_sub_ps(_mm_set1_ps(1.0f), c);
	__m128 axisS = _mm_mul_ps(axis, s);
	__m128 negAxisS = SIMD::Negate(axisS);
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	tmp0 = _mm_shuffle_ps(axisS, axisS, _MM_SHUFFLE(0, 0, 2, 0));
	tmp0 = SIMD::Blend(tmp0, SIMD::Shuffle_YYYY(negAxisS), mask_z);
	tmp1 = SIMD::Blend(SIMD::Shuffle_XXXX(axisS), SIMD::Shuffle_ZZZZ(negAxisS), mask_x);
	tmp2 = _mm_shuffle_ps(axisS, axisS, _MM_SHUFFLE(0, 0, 0, 1));
	tmp2 = SIMD::Blend(tmp2, SIMD::Shuffle_XXXX(negAxisS), mask_y);
	tmp0 = SIMD::Blend(tmp0, c, mask_x);
	tmp1 = SIMD::Blend(tmp1, c, mask_y);
	tmp2 = SIMD::Blend(tmp2, c, mask_z);
	return Matrix3(
		vec_madd(_mm_mul_ps(axis, xxxx), oneMinusC, tmp0),
		vec_madd(_mm_mul_ps(axis, yyyy), oneMinusC, tmp1),
		vec_madd(_mm_mul_ps(axis, zzzz), oneMinusC, tmp2)
	);
}

DESIRE_FORCE_INLINE Matrix3 Matrix3::CreateScale(const Vector3& scaleVec)
{
	const __m128 zero = _mm_setzero_ps();
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	return Matrix3(
		SIMD::Blend(zero, scaleVec, mask_x),
		SIMD::Blend(zero, scaleVec, mask_y),
		SIMD::Blend(zero, scaleVec, mask_z)
	);
}

DESIRE_FORCE_INLINE void Matrix4::Transpose()
{
	__m128 tmp0 = _mm_unpacklo_ps(col0, col2);
	__m128 tmp1 = _mm_unpacklo_ps(col1, col3);
	__m128 tmp2 = _mm_unpackhi_ps(col0, col2);
	__m128 tmp3 = _mm_unpackhi_ps(col1, col3);
	col0 = _mm_unpacklo_ps(tmp0, tmp1);
	col1 = _mm_unpackhi_ps(tmp0, tmp1);
	col2 = _mm_unpacklo_ps(tmp2, tmp3);
	col3 = _mm_unpackhi_ps(tmp2, tmp3);
}

DESIRE_FORCE_INLINE void Matrix4::Invert()
{
	// Calculating the minterms for the first line
	__m128 tt2 = _mm_ror_ps(col2, 1);
	__m128 Vc = _mm_mul_ps(tt2, col3);									// V3' dot V4
	__m128 Va = _mm_mul_ps(tt2, _mm_ror_ps(col3, 2));					// V3' dot V4"
	__m128 Vb = _mm_mul_ps(tt2, _mm_ror_ps(col3, 3));					// V3' dot V4^

	__m128 r1 = _mm_sub_ps(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));		// V3" dot V4^ - V3^ dot V4"
	__m128 r2 = _mm_sub_ps(_mm_ror_ps(Vb, 2), Vb);						// V3^ dot V4' - V3' dot V4^
	__m128 r3 = _mm_sub_ps(Va, _mm_ror_ps(Vc, 1));						// V3' dot V4" - V3" dot V4'

	__m128 sum;
	Va = _mm_ror_ps(col1, 1);
	sum = _mm_mul_ps(Va, r1);
	Vb = _mm_ror_ps(col1, 2);
	sum = _mm_add_ps(sum, _mm_mul_ps(Vb, r2));
	Vc = _mm_ror_ps(col1, 3);
	sum = _mm_add_ps(sum, _mm_mul_ps(Vc, r3));

	// Calculating the determinant
	__m128 det = _mm_mul_ps(sum, col0);
	det = _mm_add_ps(det, _mm_movehl_ps(det, det));

	// Testing the determinant
	det = _mm_sub_ss(det, _mm_shuffle_ps(det, det, 1));

	alignas(16) static const uint32_t _vmathPNPN[4] = { 0x00000000, 0x80000000, 0x00000000, 0x80000000 };
	alignas(16) static const uint32_t _vmathNPNP[4] = { 0x80000000, 0x00000000, 0x80000000, 0x00000000 };
	const __m128 Sign_PNPN = _mm_load_ps((float*)_vmathPNPN);
	const __m128 Sign_NPNP = _mm_load_ps((float*)_vmathNPNP);
	__m128 mtL1 = _mm_xor_ps(sum, Sign_PNPN);

	// Calculating the minterms of the second line (using previous results)
	__m128 tt = _mm_ror_ps(col0, 1);
	sum = _mm_mul_ps(tt, r1);
	tt = _mm_ror_ps(tt, 1);
	sum = _mm_add_ps(sum, _mm_mul_ps(tt, r2));
	tt = _mm_ror_ps(tt, 1);
	sum = _mm_add_ps(sum, _mm_mul_ps(tt, r3));
	__m128 mtL2 = _mm_xor_ps(sum, Sign_NPNP);

	// Calculating the minterms of the third line
	tt = _mm_ror_ps(col0, 1);
	Va = _mm_mul_ps(tt, Vb);											// V1' dot V2"
	Vb = _mm_mul_ps(tt, Vc);											// V1' dot V2^
	Vc = _mm_mul_ps(tt, col1);											// V1' dot V2

	r1 = _mm_sub_ps(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));				// V1" dot V2^ - V1^ dot V2"
	r2 = _mm_sub_ps(_mm_ror_ps(Vb, 2), Vb);								// V1^ dot V2' - V1' dot V2^
	r3 = _mm_sub_ps(Va, _mm_ror_ps(Vc, 1));								// V1' dot V2" - V1" dot V2'

	tt = _mm_ror_ps(col3, 1);
	sum = _mm_mul_ps(tt, r1);
	tt = _mm_ror_ps(tt, 1);
	sum = _mm_add_ps(sum, _mm_mul_ps(tt, r2));
	tt = _mm_ror_ps(tt, 1);
	sum = _mm_add_ps(sum, _mm_mul_ps(tt, r3));
	__m128 mtL3 = _mm_xor_ps(sum, Sign_PNPN);

	// Dividing is FASTER than rcp_nr! (Because rcp_nr causes many register-memory RWs)
	const float one = 1.0f;
	__m128 rDet = _mm_div_ss(_mm_load_ss(&one), det);
	rDet = SIMD::Shuffle_XXXX(rDet);

	// Devide the first 12 minterms with the determinant
	mtL1 = _mm_mul_ps(mtL1, rDet);
	mtL2 = _mm_mul_ps(mtL2, rDet);
	mtL3 = _mm_mul_ps(mtL3, rDet);

	// Calculate the minterms of the forth line and devide by the determinant
	tt = _mm_ror_ps(col2, 1);
	sum = _mm_mul_ps(tt, r1);
	tt = _mm_ror_ps(tt, 1);
	sum = _mm_add_ps(sum, _mm_mul_ps(tt, r2));
	tt = _mm_ror_ps(tt, 1);
	sum = _mm_add_ps(sum, _mm_mul_ps(tt, r3));
	__m128 mtL4 = _mm_xor_ps(sum, Sign_NPNP);
	mtL4 = _mm_mul_ps(mtL4, rDet);

	// Now we just have to transpose the minterms matrix
	__m128 trns0 = _mm_unpacklo_ps(mtL1, mtL2);
	__m128 trns1 = _mm_unpacklo_ps(mtL3, mtL4);
	__m128 trns2 = _mm_unpackhi_ps(mtL1, mtL2);
	__m128 trns3 = _mm_unpackhi_ps(mtL3, mtL4);
	col0 = _mm_movelh_ps(trns0, trns1);
	col1 = _mm_movehl_ps(trns1, trns0);
	col2 = _mm_movelh_ps(trns2, trns3);
	col3 = _mm_movehl_ps(trns3, trns2);
}

DESIRE_FORCE_INLINE void Matrix4::AffineInvert()
{
	__m128 inv0, inv1, inv2, inv3;
	const __m128 tmp2 = col0.GetXYZ().Cross(col1.GetXYZ());
	const __m128 tmp0 = col1.GetXYZ().Cross(col2.GetXYZ());
	const __m128 tmp1 = col2.GetXYZ().Cross(col0.GetXYZ());
	inv3 = SIMD::Negate(col3);
	__m128 dot = SIMD::Dot3(tmp2, col2);
	dot = SIMD::Shuffle_XXXX(dot);
	const __m128 invdet = _mm_rcp_ps(dot);
	const __m128 tmp3 = _mm_unpacklo_ps(tmp0, tmp2);
	const __m128 tmp4 = _mm_unpackhi_ps(tmp0, tmp2);
	inv0 = _mm_unpacklo_ps(tmp3, tmp1);
	const __m128 xxxx = SIMD::Shuffle_XXXX(inv3);
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	inv1 = _mm_shuffle_ps(tmp3, tmp3, _MM_SHUFFLE(0, 3, 2, 2));
	inv1 = SIMD::Blend(inv1, tmp1, mask_y);
	inv2 = _mm_shuffle_ps(tmp4, tmp4, _MM_SHUFFLE(0, 1, 1, 0));
	inv2 = SIMD::Blend(inv2, SIMD::Shuffle_ZZZZ(tmp1), mask_y);
	const __m128 yyyy = SIMD::Shuffle_YYYY(inv3);
	const __m128 zzzz = SIMD::Shuffle_ZZZZ(inv3);
	inv3 = _mm_mul_ps(inv0, xxxx);
	inv3 = vec_madd(inv1, yyyy, inv3);
	inv3 = vec_madd(inv2, zzzz, inv3);
	inv0 = _mm_mul_ps(inv0, invdet);
	inv1 = _mm_mul_ps(inv1, invdet);
	inv2 = _mm_mul_ps(inv2, invdet);
	inv3 = _mm_mul_ps(inv3, invdet);
	col0 = Vector4(Vector3(inv0), 0.0f);
	col1 = Vector4(Vector3(inv1), 0.0f);
	col2 = Vector4(Vector3(inv2), 0.0f);
	col3 = Vector4(Vector3(inv3), 1.0f);
}

DESIRE_FORCE_INLINE void Matrix4::OrthoInvert()
{
	__m128 inv0, inv1, inv2, inv3;
	__m128 tmp0, tmp1;
	__m128 xxxx, yyyy, zzzz;
	tmp0 = _mm_unpacklo_ps(col0, col2);
	tmp1 = _mm_unpackhi_ps(col0, col2);
	inv3 = SIMD::Negate(col3);
	inv0 = _mm_unpacklo_ps(tmp0, col1);
	xxxx = SIMD::Shuffle_XXXX(inv3);
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	inv1 = _mm_shuffle_ps(tmp0, tmp0, _MM_SHUFFLE(0, 3, 2, 2));
	inv1 = SIMD::Blend(inv1, col1, mask_y);
	inv2 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(0, 1, 1, 0));
	inv2 = SIMD::Blend(inv2, SIMD::Shuffle_ZZZZ(col1), mask_y);
	yyyy = SIMD::Shuffle_YYYY(inv3);
	zzzz = SIMD::Shuffle_ZZZZ(inv3);
	inv3 = _mm_mul_ps(inv0, xxxx);
	inv3 = vec_madd(inv1, yyyy, inv3);
	inv3 = vec_madd(inv2, zzzz, inv3);
	col0 = Vector4(Vector3(inv0), 0.0f);
	col1 = Vector4(Vector3(inv1), 0.0f);
	col2 = Vector4(Vector3(inv2), 0.0f);
	col3 = Vector4(Vector3(inv3), 1.0f);
}

DESIRE_FORCE_INLINE float Matrix4::CalculateDeterminant() const
{
	// Calculating the minterms for the first line
	__m128 tt2 = _mm_ror_ps(col2, 1);
	__m128 Vc = _mm_mul_ps(tt2, col3);									// V3' dot V4
	__m128 Va = _mm_mul_ps(tt2, _mm_ror_ps(col3, 2));					// V3' dot V4"
	__m128 Vb = _mm_mul_ps(tt2, _mm_ror_ps(col3, 3));					// V3' dot V4^

	__m128 r1 = _mm_sub_ps(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));		// V3" dot V4^ - V3^ dot V4"
	__m128 r2 = _mm_sub_ps(_mm_ror_ps(Vb, 2), Vb);						// V3^ dot V4' - V3' dot V4^
	__m128 r3 = _mm_sub_ps(Va, _mm_ror_ps(Vc, 1));						// V3' dot V4" - V3" dot V4'

	__m128 sum;
	Va = _mm_ror_ps(col1, 1);
	sum = _mm_mul_ps(Va, r1);
	Vb = _mm_ror_ps(col1, 2);
	sum = _mm_add_ps(sum, _mm_mul_ps(Vb, r2));
	Vc = _mm_ror_ps(col1, 3);
	sum = _mm_add_ps(sum, _mm_mul_ps(Vc, r3));

	// Calculating the determinant
	__m128 det = _mm_mul_ps(sum, col0);
	det = _mm_add_ps(det, _mm_movehl_ps(det, det));

	// Testing the determinant
	det = _mm_sub_ss(det, _mm_shuffle_ps(det, det, 1));
	return _mm_cvtss_f32(det);
}

DESIRE_FORCE_INLINE Vector4 Matrix4::operator *(const Vector4& vec) const
{
	return Vector4(
		_mm_add_ps(
			_mm_add_ps(_mm_mul_ps(col0, SIMD::Shuffle_XXXX(vec)), _mm_mul_ps(col1, SIMD::Shuffle_YYYY(vec))),
			_mm_add_ps(_mm_mul_ps(col2, SIMD::Shuffle_ZZZZ(vec)), _mm_mul_ps(col3, SIMD::Shuffle_WWWW(vec))))
	);
}

DESIRE_FORCE_INLINE Vector4 Matrix4::operator *(const Vector3& vec) const
{
	return Vector4(
		_mm_add_ps(
			_mm_add_ps(_mm_mul_ps(col0, SIMD::Shuffle_XXXX(vec)), _mm_mul_ps(col1, SIMD::Shuffle_YYYY(vec))),
			_mm_mul_ps(col2, SIMD::Shuffle_ZZZZ(vec)))
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationX(float radians)
{
	__m128 s, c, res1, res2;
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	const __m128 zero = _mm_setzero_ps();
	sincosf4(_mm_set1_ps(radians), &s, &c);
	res1 = SIMD::Blend(zero, c, mask_y);
	res1 = SIMD::Blend(res1, s, mask_z);
	res2 = SIMD::Blend(zero, SIMD::Negate(s), mask_y);
	res2 = SIMD::Blend(res2, c, mask_z);
	return Matrix4(
		Vector4::AxisX(),
		res1,
		res2,
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationY(float radians)
{
	__m128 s, c, res0, res2;
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	const __m128 zero = _mm_setzero_ps();
	sincosf4(_mm_set1_ps(radians), &s, &c);
	res0 = SIMD::Blend(zero, c, mask_x);
	res0 = SIMD::Blend(res0, SIMD::Negate(s), mask_z);
	res2 = SIMD::Blend(zero, s, mask_x);
	res2 = SIMD::Blend(res2, c, mask_z);
	return Matrix4(
		res0,
		Vector4::AxisY(),
		res2,
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationZ(float radians)
{
	__m128 s, c, res0, res1;
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 zero = _mm_setzero_ps();
	sincosf4(_mm_set1_ps(radians), &s, &c);
	res0 = SIMD::Blend(zero, c, mask_x);
	res0 = SIMD::Blend(res0, s, mask_y);
	res1 = SIMD::Blend(zero, SIMD::Negate(s), mask_x);
	res1 = SIMD::Blend(res1, c, mask_y);
	return Matrix4(
		res0,
		res1,
		Vector4::AxisZ(),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotationZYX(const Vector3& radiansXYZ)
{
	__m128 s, c, X0, X1, Y0, Y1;
	__m128 angles = Vector4(radiansXYZ, 0.0f);
	sincosf4(angles, &s, &c);
	__m128 negS = SIMD::Negate(s);
	__m128 Z0 = _mm_unpackhi_ps(c, s);
	__m128 Z1 = _mm_unpackhi_ps(negS, c);
	alignas(16) const uint32_t select_xyz[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
	Z1 = _mm_and_ps(Z1, _mm_load_ps((float*)select_xyz));
	Y0 = _mm_shuffle_ps(c, negS, _MM_SHUFFLE(0, 1, 1, 1));
	Y1 = _mm_shuffle_ps(s, c, _MM_SHUFFLE(0, 1, 1, 1));
	X0 = SIMD::Shuffle_XXXX(s);
	X1 = SIMD::Shuffle_XXXX(c);
	__m128 tmp = _mm_mul_ps(Z0, Y1);
	return Matrix4(
		_mm_mul_ps(Z0, Y0),
		vec_madd(Z1, X1, _mm_mul_ps(tmp, X0)),
		vec_nmsub(Z1, X0, _mm_mul_ps(tmp, X1)),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateRotation(float radians, const Vector3& unitVec)
{
	__m128 s, c, tmp0, tmp1, tmp2;
	__m128 axis = unitVec;
	sincosf4(_mm_set1_ps(radians), &s, &c);
	__m128 xxxx = SIMD::Shuffle_XXXX(axis);
	__m128 yyyy = SIMD::Shuffle_YYYY(axis);
	__m128 zzzz = SIMD::Shuffle_ZZZZ(axis);
	__m128 oneMinusC = _mm_sub_ps(_mm_set1_ps(1.0f), c);
	__m128 axisS = _mm_mul_ps(axis, s);
	__m128 negAxisS = SIMD::Negate(axisS);
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	tmp0 = _mm_shuffle_ps(axisS, axisS, _MM_SHUFFLE(0, 0, 2, 0));
	tmp0 = SIMD::Blend(tmp0, SIMD::Shuffle_YYYY(negAxisS), mask_z);
	tmp1 = SIMD::Blend(SIMD::Shuffle_XXXX(axisS), SIMD::Shuffle_ZZZZ(negAxisS), mask_x);
	tmp2 = _mm_shuffle_ps(axisS, axisS, _MM_SHUFFLE(0, 0, 0, 1));
	tmp2 = SIMD::Blend(tmp2, SIMD::Shuffle_XXXX(negAxisS), mask_y);
	tmp0 = SIMD::Blend(tmp0, c, mask_x);
	tmp1 = SIMD::Blend(tmp1, c, mask_y);
	tmp2 = SIMD::Blend(tmp2, c, mask_z);
	alignas(16) const uint32_t select_xyz[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
	const __m128 mask_xyz = _mm_load_ps((float *)select_xyz);
	axis = _mm_and_ps(axis, mask_xyz);
	tmp0 = _mm_and_ps(tmp0, mask_xyz);
	tmp1 = _mm_and_ps(tmp1, mask_xyz);
	tmp2 = _mm_and_ps(tmp2, mask_xyz);
	return Matrix4(
		vec_madd(_mm_mul_ps(axis, xxxx), oneMinusC, tmp0),
		vec_madd(_mm_mul_ps(axis, yyyy), oneMinusC, tmp1),
		vec_madd(_mm_mul_ps(axis, zzzz), oneMinusC, tmp2),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Matrix4 Matrix4::CreateScale(const Vector3& scaleVec)
{
	__m128 zero = _mm_setzero_ps();
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	return Matrix4(
		SIMD::Blend(zero, scaleVec, mask_x),
		SIMD::Blend(zero, scaleVec, mask_y),
		SIMD::Blend(zero, scaleVec, mask_z),
		Vector4::AxisW()
	);
}

DESIRE_FORCE_INLINE Quat::Quat(const Matrix3& tfrm)
{
	__m128 col0, col1, col2;
	__m128 xx_yy, xx_yy_zz_xx, yy_zz_xx_yy, zz_xx_yy_zz, diagSum, diagDiff;
	__m128 zy_xz_yx, yz_zx_xy, sum, diff;
	__m128 radicand, invSqrt, scale;
	__m128 res0, res1, res2, res3;
	__m128 xx, yy, zz;
	alignas(16) const uint32_t select_x[4] = { 0xffffffff, 0, 0, 0 };
	alignas(16) const uint32_t select_y[4] = { 0, 0xffffffff, 0, 0 };
	alignas(16) const uint32_t select_z[4] = { 0, 0, 0xffffffff, 0 };
	alignas(16) const uint32_t select_w[4] = { 0, 0, 0, 0xffffffff };
	const __m128 mask_x = _mm_load_ps((float*)select_x);
	const __m128 mask_y = _mm_load_ps((float*)select_y);
	const __m128 mask_z = _mm_load_ps((float*)select_z);
	const __m128 mask_w = _mm_load_ps((float*)select_w);

	col0 = tfrm.col0;
	col1 = tfrm.col1;
	col2 = tfrm.col2;

	/* four cases: */
	/* trace > 0 */
	/* else */
	/*    xx largest diagonal element */
	/*    yy largest diagonal element */
	/*    zz largest diagonal element */

	/* compute quaternion for each case */

	xx_yy = SIMD::Blend(col0, col1, mask_y);
	xx_yy_zz_xx = _mm_shuffle_ps(xx_yy, xx_yy, _MM_SHUFFLE(0, 0, 1, 0));
	xx_yy_zz_xx = SIMD::Blend(xx_yy_zz_xx, col2, mask_z); // TODO: Ck
	yy_zz_xx_yy = _mm_shuffle_ps(xx_yy_zz_xx, xx_yy_zz_xx, _MM_SHUFFLE(1, 0, 2, 1));
	zz_xx_yy_zz = _mm_shuffle_ps(xx_yy_zz_xx, xx_yy_zz_xx, _MM_SHUFFLE(2, 1, 0, 2));

	diagSum = _mm_add_ps(_mm_add_ps(xx_yy_zz_xx, yy_zz_xx_yy), zz_xx_yy_zz);
	diagDiff = _mm_sub_ps(_mm_sub_ps(xx_yy_zz_xx, yy_zz_xx_yy), zz_xx_yy_zz);
	radicand = _mm_add_ps(SIMD::Blend(diagDiff, diagSum, mask_w), _mm_set1_ps(1.0f));
	invSqrt = newtonrapson_rsqrt4(radicand);

	zy_xz_yx = SIMD::Blend(col0, col1, mask_z);								// zy_xz_yx = 00 01 12 03
	zy_xz_yx = _mm_shuffle_ps(zy_xz_yx, zy_xz_yx, _MM_SHUFFLE(0, 1, 2, 2));	// zy_xz_yx = 12 12 01 00
	zy_xz_yx = SIMD::Blend(zy_xz_yx, SIMD::Shuffle_XXXX(col2), mask_y);		// zy_xz_yx = 12 20 01 00
	yz_zx_xy = SIMD::Blend(col0, col1, mask_x);								// yz_zx_xy = 10 01 02 03
	yz_zx_xy = _mm_shuffle_ps(yz_zx_xy, yz_zx_xy, _MM_SHUFFLE(0, 0, 2, 0));	// yz_zx_xy = 10 02 10 10
	yz_zx_xy = SIMD::Blend(yz_zx_xy, SIMD::Shuffle_YYYY(col2), mask_x);		// yz_zx_xy = 21 02 10 10

	sum = _mm_add_ps(zy_xz_yx, yz_zx_xy);
	diff = _mm_sub_ps(zy_xz_yx, yz_zx_xy);

	scale = _mm_mul_ps(invSqrt, _mm_set1_ps(0.5f));

	res0 = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 1, 2, 0));
	res0 = SIMD::Blend(res0, SIMD::Shuffle_XXXX(diff), mask_w);  // TODO: Ck
	res1 = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 0, 0, 2));
	res1 = SIMD::Blend(res1, SIMD::Shuffle_YYYY(diff), mask_w);  // TODO: Ck
	res2 = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 0, 0, 1));
	res2 = SIMD::Blend(res2, SIMD::Shuffle_ZZZZ(diff), mask_w);  // TODO: Ck
	res3 = diff;
	res0 = SIMD::Blend(res0, radicand, mask_x);
	res1 = SIMD::Blend(res1, radicand, mask_y);
	res2 = SIMD::Blend(res2, radicand, mask_z);
	res3 = SIMD::Blend(res3, radicand, mask_w);
	res0 = _mm_mul_ps(res0, SIMD::Shuffle_XXXX(scale));
	res1 = _mm_mul_ps(res1, SIMD::Shuffle_YYYY(scale));
	res2 = _mm_mul_ps(res2, SIMD::Shuffle_ZZZZ(scale));
	res3 = _mm_mul_ps(res3, SIMD::Shuffle_WWWW(scale));

	/* determine case and select answer */

	xx = SIMD::Shuffle_XXXX(col0);
	yy = SIMD::Shuffle_YYYY(col1);
	zz = SIMD::Shuffle_ZZZZ(col2);
	__m128 res = SIMD::Blend(res0, res1, _mm_cmpgt_ps(yy, xx));
	res = SIMD::Blend(res, res2, _mm_and_ps(_mm_cmpgt_ps(zz, xx), _mm_cmpgt_ps(zz, yy)));
	res = SIMD::Blend(res, res3, _mm_cmpgt_ps(SIMD::Shuffle_XXXX(diagSum), _mm_setzero_ps()));
	mVec128 = res;
}
