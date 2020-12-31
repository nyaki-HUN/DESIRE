#include "stdafx.h"
#include "Engine/Core/Math/Matrix4.h"

TEST_CASE("Matrix4", "[Core][math]")
{
	Matrix4 mat = Matrix4::CreateRotationZYX(Vector3(1.0f, -4.0f, 20.0f));
	mat.SetTranslation(Vector3(123.0f, -10.0f, 20.0f));

//	SECTION("operator *(Vector4)")
//	SECTION("operator *(Vector3)")
//	SECTION("Transpose()")

	SECTION("Invert()")
	{
		// Scalar version of Invert()
		float tmp0 = mat.col2.GetZ() * mat.col0.GetW() - mat.col0.GetZ() * mat.col2.GetW();
		float tmp1 = mat.col3.GetZ() * mat.col1.GetW() - mat.col1.GetZ() * mat.col3.GetW();
		float tmp2 = mat.col0.GetY() * mat.col2.GetZ() - mat.col2.GetY() * mat.col0.GetZ();
		float tmp3 = mat.col1.GetY() * mat.col3.GetZ() - mat.col3.GetY() * mat.col1.GetZ();
		float tmp4 = mat.col2.GetY() * mat.col0.GetW() - mat.col0.GetY() * mat.col2.GetW();
		float tmp5 = mat.col3.GetY() * mat.col1.GetW() - mat.col1.GetY() * mat.col3.GetW();
		const float res0[4] =
		{
			mat.col2.GetY() * tmp1 - mat.col2.GetW() * tmp3 - mat.col2.GetZ() * tmp5,
			mat.col3.GetY() * tmp0 - mat.col3.GetW() * tmp2 - mat.col3.GetZ() * tmp4,
			mat.col0.GetW() * tmp3 + mat.col0.GetZ() * tmp5 - mat.col0.GetY() * tmp1,
			mat.col1.GetW() * tmp2 + mat.col1.GetZ() * tmp4 - mat.col1.GetY() * tmp0
		};
		const float detInv = 1.0f / ((((mat.col0.GetX() * res0[0]) + (mat.col1.GetX() * res0[1])) + (mat.col2.GetX() * res0[2])) + (mat.col3.GetX() * res0[3]));
		float res1[4] =
		{
			mat.col2.GetX() * tmp1,
			mat.col3.GetX() * tmp0,
			mat.col0.GetX() * tmp1,
			mat.col1.GetX() * tmp0
		};
		float res3[4] =
		{
			mat.col2.GetX() * tmp3,
			mat.col3.GetX() * tmp2,
			mat.col0.GetX() * tmp3,
			mat.col1.GetX() * tmp2
		};
		float res2[4] =
		{
			mat.col2.GetX() * tmp5,
			mat.col3.GetX() * tmp4,
			mat.col0.GetX() * tmp5,
			mat.col1.GetX() * tmp4
		};
		tmp0 = mat.col2.GetX() * mat.col0.GetY() - mat.col0.GetX() * mat.col2.GetY();
		tmp1 = mat.col3.GetX() * mat.col1.GetY() - mat.col1.GetX() * mat.col3.GetY();
		tmp2 = mat.col2.GetX() * mat.col0.GetW() - mat.col0.GetX() * mat.col2.GetW();
		tmp3 = mat.col3.GetX() * mat.col1.GetW() - mat.col1.GetX() * mat.col3.GetW();
		tmp4 = mat.col2.GetX() * mat.col0.GetZ() - mat.col0.GetX() * mat.col2.GetZ();
		tmp5 = mat.col3.GetX() * mat.col1.GetZ() - mat.col1.GetX() * mat.col3.GetZ();
		res2[0] = (mat.col2.GetW() * tmp1 - mat.col2.GetY() * tmp3 + res2[0]);
		res2[1] = (mat.col3.GetW() * tmp0 - mat.col3.GetY() * tmp2 + res2[1]);
		res2[2] = (mat.col0.GetY() * tmp3 - mat.col0.GetW() * tmp1 - res2[2]);
		res2[3] = (mat.col1.GetY() * tmp2 - mat.col1.GetW() * tmp0 - res2[3]);
		res3[0] = (mat.col2.GetY() * tmp5 - mat.col2.GetZ() * tmp1 + res3[0]);
		res3[1] = (mat.col3.GetY() * tmp4 - mat.col3.GetZ() * tmp0 + res3[1]);
		res3[2] = (mat.col0.GetZ() * tmp1 - mat.col0.GetY() * tmp5 - res3[2]);
		res3[3] = (mat.col1.GetZ() * tmp0 - mat.col1.GetY() * tmp4 - res3[3]);
		res1[0] = (mat.col2.GetZ() * tmp3 - mat.col2.GetW() * tmp5 - res1[0]);
		res1[1] = (mat.col3.GetZ() * tmp2 - mat.col3.GetW() * tmp4 - res1[1]);
		res1[2] = (mat.col0.GetW() * tmp5 - mat.col0.GetZ() * tmp3 + res1[2]);
		res1[3] = (mat.col1.GetW() * tmp4 - mat.col1.GetZ() * tmp2 + res1[3]);
		const float matResult[4][4] =
		{
			{
				res0[0] * detInv,
				res0[1] * detInv,
				res0[2] * detInv,
				res0[3] * detInv
			},
			{
				res1[0] * detInv,
				res1[1] * detInv,
				res1[2] * detInv,
				res1[3] * detInv
			},
			{
				res2[0] * detInv,
				res2[1] * detInv,
				res2[2] * detInv,
				res2[3] * detInv
			},
			{
				res3[0] * detInv,
				res3[1] * detInv,
				res3[2] * detInv,
				res3[3] * detInv
			}
		};

		mat.Invert();

		for(uint32_t i = 0; i < 4; ++i)
		{
			float matVec[4];
			mat.GetCol(i).StoreXYZW(matVec);

			CHECK(matVec[0] == Approx(matResult[i][0]));
			CHECK(matVec[1] == Approx(matResult[i][1]));
			CHECK(matVec[2] == Approx(matResult[i][2]));
			CHECK(matVec[3] == Approx(matResult[i][3]));
		}
	}

//	SECTION("AffineInvert()")
//	SECTION("OrthoInvert()")

	SECTION("CalculateDeterminant()")
	{
		// Scalar version of CalculateDeterminant()
		const float tmp0 = mat.col2.GetZ() * mat.col0.GetW() - mat.col0.GetZ() * mat.col2.GetW();
		const float tmp1 = mat.col3.GetZ() * mat.col1.GetW() - mat.col1.GetZ() * mat.col3.GetW();
		const float tmp2 = mat.col0.GetY() * mat.col2.GetZ() - mat.col2.GetY() * mat.col0.GetZ();
		const float tmp3 = mat.col1.GetY() * mat.col3.GetZ() - mat.col3.GetY() * mat.col1.GetZ();
		const float tmp4 = mat.col2.GetY() * mat.col0.GetW() - mat.col0.GetY() * mat.col2.GetW();
		const float tmp5 = mat.col3.GetY() * mat.col1.GetW() - mat.col1.GetY() * mat.col3.GetW();
		const float dx = mat.col2.GetY() * tmp1 - mat.col2.GetW() * tmp3 - mat.col2.GetZ() * tmp5;
		const float dy = mat.col3.GetY() * tmp0 - mat.col3.GetW() * tmp2 - mat.col3.GetZ() * tmp4;
		const float dz = mat.col0.GetW() * tmp3 + mat.col0.GetZ() * tmp5 - mat.col0.GetY() * tmp1;
		const float dw = mat.col1.GetW() * tmp2 + mat.col1.GetZ() * tmp4 - mat.col1.GetY() * tmp0;
		const float det = mat.col0.GetX() * dx + mat.col1.GetX() * dy + mat.col2.GetX() * dz + mat.col3.GetX() * dw;

		CHECK(mat.CalculateDeterminant() == Approx(det));
	}

//	SECTION("CreateRotationX()")
//	SECTION("CreateRotationY()")
//	SECTION("CreateRotationZ()")
//	SECTION("CreateRotationZYX()")
//	SECTION("CreateRotation()")
//	SECTION("CreateScale()")
}
