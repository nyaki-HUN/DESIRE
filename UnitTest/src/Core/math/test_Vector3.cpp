#include "stdafx.h"
#include "Engine/Core/math/Vector3.h"

TEST_CASE("Vector3", "[Core][math]")
{
	Vector3 vec0(1.0f, 2.0f, 3.0f);
	Vector3 vec1(10.0f, 5.0f, -2.0f);

	SECTION("LoadXYZ()")
	{
		const float floatVec[3] =
		{
			123.0f, 45.0f, 0.0f
		};

		vec0.LoadXYZ(floatVec);

		CHECK(vec0.GetX() == Approx(floatVec[0]));
		CHECK(vec0.GetY() == Approx(floatVec[1]));
		CHECK(vec0.GetZ() == Approx(floatVec[2]));
	}

	SECTION("StoreXYZ()")
	{
		float result[3] = {};
		vec0.StoreXYZ(result);

		CHECK(vec0.GetX() == Approx(result[0]));
		CHECK(vec0.GetY() == Approx(result[1]));
		CHECK(vec0.GetZ() == Approx(result[2]));
	}

	SECTION("Set element | Get element")
	{
		vec0.SetX(123.0f);
		vec0.SetY(123.0f);
		vec0.SetZ(123.0f);

		CHECK(vec0.GetX() == Approx(123.0f));
		CHECK(vec0.GetY() == Approx(123.0f));
		CHECK(vec0.GetZ() == Approx(123.0f));
	}

	SECTION("operator -()")
	{
		const Vector3 result = -vec0;

		CHECK(result.GetX() == Approx(-vec0.GetX()));
		CHECK(result.GetY() == Approx(-vec0.GetY()));
		CHECK(result.GetZ() == Approx(-vec0.GetZ()));
	}

	SECTION("operator +()")
	{
		const Vector3 result = vec0 + vec1;

		CHECK(result.GetX() == Approx(vec0.GetX() + vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() + vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() + vec1.GetZ()));
	}

	SECTION("operator -()")
	{
		const Vector3 result = vec0 - vec1;

		CHECK(result.GetX() == Approx(vec0.GetX() - vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() - vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() - vec1.GetZ()));
	}

	SECTION("operator *()")
	{
		Vector3 result = vec0 * vec1;
		CHECK(result.GetX() == Approx(vec0.GetX() * vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() * vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() * vec1.GetZ()));

		result = vec0 * 123.0f;
		CHECK(result.GetX() == Approx(vec0.GetX() * 123.0f));
		CHECK(result.GetY() == Approx(vec0.GetY() * 123.0f));
		CHECK(result.GetZ() == Approx(vec0.GetZ() * 123.0f));
	}

	SECTION("operator /()")
	{
		Vector3 result = vec0 / vec1;
		CHECK(result.GetX() == Approx(vec0.GetX() / vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() / vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() / vec1.GetZ()));

		result = vec0 / 123.0f;
		CHECK(result.GetX() == Approx(vec0.GetX() / 123.0f));
		CHECK(result.GetY() == Approx(vec0.GetY() / 123.0f));
		CHECK(result.GetZ() == Approx(vec0.GetZ() / 123.0f));
	}

	SECTION("operator >=()")
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

	SECTION("operator <=()")
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

		CHECK(vec0.GetMaxElem() == Approx(result));
	}

	SECTION("GetMinElem()")
	{
		float result;
		result = (vec0.GetX() < vec0.GetY()) ? vec0.GetX() : vec0.GetY();
		result = (vec0.GetZ() < result) ? vec0.GetZ() : result;

		CHECK(vec0.GetMinElem() == Approx(result));
	}

	SECTION("Dot()")
	{
		CHECK(vec0.Dot(vec1) == Approx(vec0.GetX() * vec1.GetX() + vec0.GetY() * vec1.GetY() + vec0.GetZ() * vec1.GetZ()));
	}

	SECTION("Cross()")
	{
		Vector3 result = vec0.Cross(vec1);

		CHECK(result.GetX() == Approx(vec0.GetY() * vec1.GetZ() - vec0.GetZ() * vec1.GetY()));
		CHECK(result.GetY() == Approx(vec0.GetZ() * vec1.GetX() - vec0.GetX() * vec1.GetZ()));
		CHECK(result.GetZ() == Approx(vec0.GetX() * vec1.GetY() - vec0.GetY() * vec1.GetX()));
	}

	SECTION("LengthSqr()")
	{
		CHECK(vec0.LengthSqr() == Approx(vec0.Dot(vec0)));
	}

	SECTION("Length()")
	{
		CHECK(vec0.Length() == Approx(std::sqrt(vec0.LengthSqr())));
	}

	SECTION("Normalize() | Normalized()")
	{
		Vector3 result = vec0;
		result.Normalize();

		const float length = vec0.Length();
		CHECK(result.GetX() == Approx(vec0.GetX() / length));
		CHECK(result.GetY() == Approx(vec0.GetY() / length));
		CHECK(result.GetZ() == Approx(vec0.GetZ() / length));

		Vector3 result2 = vec0.Normalized();
		CHECK(result2.GetX() == Approx(result.GetX()));
		CHECK(result2.GetY() == Approx(result.GetY()));
		CHECK(result2.GetZ() == Approx(result.GetZ()));
	}

	SECTION("AbsPerElem()")
	{
		const Vector3 result = vec0.AbsPerElem();

		CHECK(result.GetX() == Approx(std::fabsf(vec0.GetX())));
		CHECK(result.GetY() == Approx(std::fabsf(vec0.GetY())));
		CHECK(result.GetZ() == Approx(std::fabsf(vec0.GetZ())));
	}

	SECTION("MaxPerElem()")
	{
		const Vector3 result = Vector3::MaxPerElem(vec0, vec1);

		CHECK(result.GetX() == Approx(std::max(vec0.GetX(), vec1.GetX())));
		CHECK(result.GetY() == Approx(std::max(vec0.GetY(), vec1.GetY())));
		CHECK(result.GetZ() == Approx(std::max(vec0.GetZ(), vec1.GetZ())));
	}

	SECTION("MinPerElem()")
	{
		const Vector3 result = Vector3::MinPerElem(vec0, vec1);

		CHECK(result.GetX() == Approx(std::min(vec0.GetX(), vec1.GetX())));
		CHECK(result.GetY() == Approx(std::min(vec0.GetY(), vec1.GetY())));
		CHECK(result.GetZ() == Approx(std::min(vec0.GetZ(), vec1.GetZ())));
	}
}
