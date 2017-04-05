#include "stdafx.h"
#include "Core/math/vectormath.h"

TEST_CASE("Vector3", "[Core][math]")
{
	Vector3 vec0(1.0f, 2.0f, 3.0f);
	Vector3 vec1(10.0f, 5.0f, 2.0f);

	SECTION("LoadXYZ()")
	{
		const float floatVec[3] =
		{
			123.0f, 45.0f, 0.0f
		};

		vec0.LoadXYZ(floatVec);

		CHECK_FLOATS(vec0.GetX(), floatVec[0]);
		CHECK_FLOATS(vec0.GetY(), floatVec[1]);
		CHECK_FLOATS(vec0.GetZ(), floatVec[2]);
	}

	SECTION("StoreXYZ()")
	{
		float result[3] = {};
		vec0.StoreXYZ(result);

		CHECK_FLOATS(vec0.GetX(), result[0]);
		CHECK_FLOATS(vec0.GetY(), result[1]);
		CHECK_FLOATS(vec0.GetZ(), result[2]);
	}

	SECTION("operator >=")
	{
		auto func = [](const Vector3& vec0, const Vector3& vec1) -> bool
		{
			return (
				vec0.GetX() >= vec1.GetX() &&
				vec0.GetY() >= vec1.GetY() &&
				vec0.GetZ() >= vec1.GetZ());
		};

		CHECK((vec0 >= vec0) == func(vec0, vec0));
		CHECK((vec0 >= vec1) == func(vec0, vec1));
		CHECK((vec0 >= Vector3(2.0f)) == func(vec0, Vector3(2.0f)));
		CHECK((vec0 >= Vector3(-10.0f)) == func(vec0, Vector3(-10.0f)));
		CHECK((vec0 >= Vector3(100.0f)) == func(vec0, Vector3(100.0f)));
	}

	SECTION("operator <=")
	{
		auto func = [](const Vector3& vec0, const Vector3& vec1) -> bool
		{
			return (
				vec0.GetX() <= vec1.GetX() &&
				vec0.GetY() <= vec1.GetY() &&
				vec0.GetZ() <= vec1.GetZ());
		};

		CHECK((vec0 <= vec0) == func(vec0, vec0));
		CHECK((vec0 <= vec1) == func(vec0, vec1));
		CHECK((vec0 <= Vector3(2.0f)) == func(vec0, Vector3(2.0f)));
		CHECK((vec0 <= Vector3(-10.0f)) == func(vec0, Vector3(-10.0f)));
		CHECK((vec0 <= Vector3(100.0f)) == func(vec0, Vector3(100.0f)));
	}

	SECTION("GetMaxElem()")
	{
		float result;
		result = (vec0.GetX() > vec0.GetY()) ? vec0.GetX() : vec0.GetY();
		result = (vec0.GetZ() > result) ? vec0.GetZ() : result;

		CHECK_FLOATS(vec0.GetMaxElem(), result);
	}

	SECTION("GetMinElem()")
	{
		float result;
		result = (vec0.GetX() < vec0.GetY()) ? vec0.GetX() : vec0.GetY();
		result = (vec0.GetZ() < result) ? vec0.GetZ() : result;

		CHECK_FLOATS(vec0.GetMinElem(), result);
	}

	SECTION("Dot()")
	{
		CHECK_FLOATS(vec0.Dot(vec1), (vec0.GetX() * vec1.GetX() + vec0.GetY() * vec1.GetY() + vec0.GetZ() * vec1.GetZ()));
	}

	SECTION("Cross()")
	{
		Vector3 result = vec0.Cross(vec1);

		CHECK_FLOATS(result.GetX(), (vec0.GetY() * vec1.GetZ() - vec0.GetZ() * vec1.GetY()));
		CHECK_FLOATS(result.GetY(), (vec0.GetZ() * vec1.GetX() - vec0.GetX() * vec1.GetZ()));
		CHECK_FLOATS(result.GetZ(), (vec0.GetX() * vec1.GetY() - vec0.GetY() * vec1.GetX()));
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
		Vector3 result = vec0;
		result.Normalize();

		const float length = vec0.Length();
		CHECK_FLOATS(result.GetX(), vec0.GetX() / length);
		CHECK_FLOATS(result.GetY(), vec0.GetY() / length);
		CHECK_FLOATS(result.GetZ(), vec0.GetZ() / length);
	}
}
