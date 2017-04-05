#include "stdafx.h"
#include "Core/math/vectormath.h"

TEST_CASE("Vector4", "[Core][math]")
{
	Vector4 vec0(4.0f, 3.0f, 2.0f, 1.0f);
	Vector4 vec1(10.0f, 5.0f, -2.0f, 0.1f);

	SECTION("LoadXYZW()")
	{
		const float floatVec[4] =
		{
			123.0f, 45.0f, 0.0f, 1.0f
		};

		vec0.LoadXYZW(floatVec);

		CHECK_FLOATS(vec0.GetX(), floatVec[0]);
		CHECK_FLOATS(vec0.GetY(), floatVec[1]);
		CHECK_FLOATS(vec0.GetZ(), floatVec[2]);
		CHECK_FLOATS(vec0.GetW(), floatVec[3]);
	}

	SECTION("StoreXYZW()")
	{
		float result[4] = {};
		vec0.StoreXYZW(result);

		CHECK_FLOATS(vec0.GetX(), result[0]);
		CHECK_FLOATS(vec0.GetY(), result[1]);
		CHECK_FLOATS(vec0.GetZ(), result[2]);
		CHECK_FLOATS(vec0.GetW(), result[3]);
	}

	SECTION("SetXYZ()")
	{
		vec0.SetXYZ(vec1.GetXYZ());

		CHECK_FLOATS(vec0.GetX(), vec1.GetX());
		CHECK_FLOATS(vec0.GetY(), vec1.GetY());
		CHECK_FLOATS(vec0.GetZ(), vec1.GetZ());
		CHECK_FLOATS(vec0.GetW(), 1.0f);
	}

	SECTION("Set element & Get element")
	{
		vec0.SetX(123.0f);
		vec0.SetY(123.0f);
		vec0.SetZ(123.0f);
		vec0.SetW(123.0f);

		CHECK_FLOATS(vec0.GetX(), 123.0f);
		CHECK_FLOATS(vec0.GetY(), 123.0f);
		CHECK_FLOATS(vec0.GetZ(), 123.0f);
		CHECK_FLOATS(vec0.GetW(), 123.0f);
	}

	SECTION("operator -()")
	{
		const Vector4 result = -vec0;

		CHECK_FLOATS(result.GetX(), -vec0.GetX());
		CHECK_FLOATS(result.GetY(), -vec0.GetY());
		CHECK_FLOATS(result.GetZ(), -vec0.GetZ());
		CHECK_FLOATS(result.GetW(), -vec0.GetW());
	}

	SECTION("operator +")
	{
		const Vector4 result = vec0 + vec1;

		CHECK_FLOATS(result.GetX(), vec0.GetX() + vec1.GetX());
		CHECK_FLOATS(result.GetY(), vec0.GetY() + vec1.GetY());
		CHECK_FLOATS(result.GetZ(), vec0.GetZ() + vec1.GetZ());
		CHECK_FLOATS(result.GetW(), vec0.GetW() + vec1.GetW());
	}

	SECTION("operator -")
	{
		const Vector4 result = vec0 - vec1;

		CHECK_FLOATS(result.GetX(), vec0.GetX() - vec1.GetX());
		CHECK_FLOATS(result.GetY(), vec0.GetY() - vec1.GetY());
		CHECK_FLOATS(result.GetZ(), vec0.GetZ() - vec1.GetZ());
		CHECK_FLOATS(result.GetW(), vec0.GetW() - vec1.GetW());
	}

	SECTION("operator *(float)")
	{
		const Vector4 result = vec0 * 123.0f;

		CHECK_FLOATS(result.GetX(), vec0.GetX() * 123.0f);
		CHECK_FLOATS(result.GetY(), vec0.GetY() * 123.0f);
		CHECK_FLOATS(result.GetZ(), vec0.GetZ() * 123.0f);
		CHECK_FLOATS(result.GetW(), vec0.GetW() * 123.0f);
	}

	SECTION("GetMaxElem()")
	{
		float result;
		result = (vec0.GetX() > vec0.GetY()) ? vec0.GetX() : vec0.GetY();
		result = (vec0.GetZ() > result) ? vec0.GetZ() : result;
		result = (vec0.GetW() > result) ? vec0.GetW() : result;

		CHECK_FLOATS(vec0.GetMaxElem(), result);
	}

	SECTION("GetMinElem()")
	{
		float result;
		result = (vec0.GetX() < vec0.GetY()) ? vec0.GetX() : vec0.GetY();
		result = (vec0.GetZ() < result) ? vec0.GetZ() : result;
		result = (vec0.GetW() < result) ? vec0.GetW() : result;

		CHECK_FLOATS(vec0.GetMinElem(), result);
	}

	SECTION("Dot()")
	{
		CHECK_FLOATS(vec0.Dot(vec1), (vec0.GetX() * vec1.GetX() + vec0.GetY() * vec1.GetY() + vec0.GetZ() * vec1.GetZ() + vec0.GetW() * vec1.GetW()));
	}

	SECTION("LengthSqr()")
	{
		CHECK_FLOATS(vec0.LengthSqr(), vec0.Dot(vec0));
	}

	SECTION("Length()")
	{
		CHECK_FLOATS(vec0.Length(), std::sqrtf(vec0.LengthSqr()));
	}

	SECTION("Normalize()")
	{
		Vector4 result = vec0;
		result.Normalize();

		const float length = vec0.Length();
		CHECK_FLOATS(result.GetX(), vec0.GetX() / length);
		CHECK_FLOATS(result.GetY(), vec0.GetY() / length);
		CHECK_FLOATS(result.GetZ(), vec0.GetZ() / length);
		CHECK_FLOATS(result.GetW(), vec0.GetW() / length);
	}

	SECTION("MulPerElem()")
	{
		const Vector4 result = vec0.MulPerElem(vec1);

		CHECK_FLOATS(result.GetX(), vec0.GetX() * vec1.GetX());
		CHECK_FLOATS(result.GetY(), vec0.GetY() * vec1.GetY());
		CHECK_FLOATS(result.GetZ(), vec0.GetZ() * vec1.GetZ());
		CHECK_FLOATS(result.GetW(), vec0.GetW() * vec1.GetW());
	}

	SECTION("DivPerElem()")
	{
		const Vector4 result = vec0.DivPerElem(vec1);

		CHECK_FLOATS(result.GetX(), vec0.GetX() / vec1.GetX());
		CHECK_FLOATS(result.GetY(), vec0.GetY() / vec1.GetY());
		CHECK_FLOATS(result.GetZ(), vec0.GetZ() / vec1.GetZ());
		CHECK_FLOATS(result.GetW(), vec0.GetW() / vec1.GetW());
	}

	SECTION("AbsPerElem()")
	{
		const Vector4 result = vec0.AbsPerElem();

		CHECK_FLOATS(result.GetX(), std::fabsf(vec0.GetX()));
		CHECK_FLOATS(result.GetY(), std::fabsf(vec0.GetY()));
		CHECK_FLOATS(result.GetZ(), std::fabsf(vec0.GetZ()));
		CHECK_FLOATS(result.GetW(), std::fabsf(vec0.GetW()));
	}

	SECTION("MaxPerElem()")
	{
		const Vector4 result = Vector4::MaxPerElem(vec0, vec1);

		CHECK_FLOATS(result.GetX(), std::max(vec0.GetX(), vec1.GetX()));
		CHECK_FLOATS(result.GetY(), std::max(vec0.GetY(), vec1.GetY()));
		CHECK_FLOATS(result.GetZ(), std::max(vec0.GetZ(), vec1.GetZ()));
		CHECK_FLOATS(result.GetW(), std::max(vec0.GetW(), vec1.GetW()));
	}

	SECTION("MinPerElem()")
	{
		const Vector4 result = Vector4::MinPerElem(vec0, vec1);

		CHECK_FLOATS(result.GetX(), std::min(vec0.GetX(), vec1.GetX()));
		CHECK_FLOATS(result.GetY(), std::min(vec0.GetY(), vec1.GetY()));
		CHECK_FLOATS(result.GetZ(), std::min(vec0.GetZ(), vec1.GetZ()));
		CHECK_FLOATS(result.GetW(), std::min(vec0.GetW(), vec1.GetW()));
	}
}
