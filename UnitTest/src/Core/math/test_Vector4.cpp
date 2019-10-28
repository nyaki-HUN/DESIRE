#include "stdafx.h"
#include "Engine/Core/math/Vector4.h"

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

		CHECK(vec0.GetX() == Approx(floatVec[0]));
		CHECK(vec0.GetY() == Approx(floatVec[1]));
		CHECK(vec0.GetZ() == Approx(floatVec[2]));
		CHECK(vec0.GetW() == Approx(floatVec[3]));
	}

	SECTION("StoreXYZW()")
	{
		float result[4] = {};
		vec0.StoreXYZW(result);

		CHECK(vec0.GetX() == Approx(result[0]));
		CHECK(vec0.GetY() == Approx(result[1]));
		CHECK(vec0.GetZ() == Approx(result[2]));
		CHECK(vec0.GetW() == Approx(result[3]));
	}

	SECTION("SetXYZ() | GetXYZ()")
	{
		vec0.SetXYZ(vec1.GetXYZ());

		CHECK(vec0.GetX() == Approx(vec1.GetX()));
		CHECK(vec0.GetY() == Approx(vec1.GetY()));
		CHECK(vec0.GetZ() == Approx(vec1.GetZ()));
		CHECK(vec0.GetW() == Approx(1.0f));
	}

	SECTION("Set element | Get element")
	{
		vec0.SetX(123.0f);
		vec0.SetY(123.0f);
		vec0.SetZ(123.0f);
		vec0.SetW(123.0f);

		CHECK(vec0.GetX() == Approx(123.0f));
		CHECK(vec0.GetY() == Approx(123.0f));
		CHECK(vec0.GetZ() == Approx(123.0f));
		CHECK(vec0.GetW() == Approx(123.0f));
	}

	SECTION("operator -()")
	{
		const Vector4 result = -vec0;

		CHECK(result.GetX() == Approx(-vec0.GetX()));
		CHECK(result.GetY() == Approx(-vec0.GetY()));
		CHECK(result.GetZ() == Approx(-vec0.GetZ()));
		CHECK(result.GetW() == Approx(-vec0.GetW()));
	}

	SECTION("operator +()")
	{
		const Vector4 result = vec0 + vec1;

		CHECK(result.GetX() == Approx(vec0.GetX() + vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() + vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() + vec1.GetZ()));
		CHECK(result.GetW() == Approx(vec0.GetW() + vec1.GetW()));
	}

	SECTION("operator -()")
	{
		const Vector4 result = vec0 - vec1;

		CHECK(result.GetX() == Approx(vec0.GetX() - vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() - vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() - vec1.GetZ()));
		CHECK(result.GetW() == Approx(vec0.GetW() - vec1.GetW()));
	}

	SECTION("operator *()")
	{
		Vector4 result = vec0 * vec1;
		CHECK(result.GetX() == Approx(vec0.GetX() * vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() * vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() * vec1.GetZ()));
		CHECK(result.GetW() == Approx(vec0.GetW() * vec1.GetW()));

		result = vec0 * 123.0f;
		CHECK(result.GetX() == Approx(vec0.GetX() * 123.0f));
		CHECK(result.GetY() == Approx(vec0.GetY() * 123.0f));
		CHECK(result.GetZ() == Approx(vec0.GetZ() * 123.0f));
		CHECK(result.GetW() == Approx(vec0.GetW() * 123.0f));
	}

	SECTION("operator /()")
	{
		Vector4 result = vec0 / vec1;
		CHECK(result.GetX() == Approx(vec0.GetX() / vec1.GetX()));
		CHECK(result.GetY() == Approx(vec0.GetY() / vec1.GetY()));
		CHECK(result.GetZ() == Approx(vec0.GetZ() / vec1.GetZ()));
		CHECK(result.GetW() == Approx(vec0.GetW() / vec1.GetW()));

		result = vec0 / 123.0f;
		CHECK(result.GetX() == Approx(vec0.GetX() / 123.0f));
		CHECK(result.GetY() == Approx(vec0.GetY() / 123.0f));
		CHECK(result.GetZ() == Approx(vec0.GetZ() / 123.0f));
		CHECK(result.GetW() == Approx(vec0.GetW() / 123.0f));
	}

	SECTION("Dot()")
	{
		CHECK(vec0.Dot(vec1) == Approx(vec0.GetX() * vec1.GetX() + vec0.GetY() * vec1.GetY() + vec0.GetZ() * vec1.GetZ() + vec0.GetW() * vec1.GetW()));
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
		Vector4 result = vec0;
		result.Normalize();

		const float length = vec0.Length();
		CHECK(result.GetX() == Approx(vec0.GetX() / length));
		CHECK(result.GetY() == Approx(vec0.GetY() / length));
		CHECK(result.GetZ() == Approx(vec0.GetZ() / length));
		CHECK(result.GetW() == Approx(vec0.GetW() / length));

		Vector4 result2 = vec0.Normalized();
		CHECK(result2.GetX() == Approx(result.GetX()));
		CHECK(result2.GetY() == Approx(result.GetY()));
		CHECK(result2.GetZ() == Approx(result.GetZ()));
		CHECK(result2.GetW() == Approx(result.GetW()));
	}

	SECTION("AbsPerElem()")
	{
		const Vector4 result = vec0.AbsPerElem();

		CHECK(result.GetX() == Approx(std::fabsf(vec0.GetX())));
		CHECK(result.GetY() == Approx(std::fabsf(vec0.GetY())));
		CHECK(result.GetZ() == Approx(std::fabsf(vec0.GetZ())));
		CHECK(result.GetW() == Approx(std::fabsf(vec0.GetW())));
	}

	SECTION("GetMaxElem()")
	{
		float result;

		result = vec0.GetX();
		result = (result > vec0.GetY()) ? result : vec0.GetY();
		result = (result > vec0.GetZ()) ? result : vec0.GetZ();
		result = (result > vec0.GetW()) ? result : vec0.GetW();
		CHECK(vec0.GetMaxElem() == Approx(result));

		result = vec1.GetX();
		result = (result > vec1.GetY()) ? result : vec1.GetY();
		result = (result > vec1.GetZ()) ? result : vec1.GetZ();
		result = (result > vec1.GetW()) ? result : vec1.GetW();
		CHECK(vec1.GetMaxElem() == Approx(result));
	}

	SECTION("GetMinElem()")
	{
		float result;

		result = vec0.GetX();
		result = (result < vec0.GetY()) ? result : vec0.GetY();
		result = (result < vec0.GetZ()) ? result : vec0.GetZ();
		result = (result < vec0.GetW()) ? result : vec0.GetW();
		CHECK(vec0.GetMinElem() == Approx(result));

		result = vec0.GetX();
		result = (result < vec1.GetY()) ? result : vec1.GetY();
		result = (result < vec1.GetZ()) ? result : vec1.GetZ();
		result = (result < vec1.GetW()) ? result : vec1.GetW();
		CHECK(vec1.GetMinElem() == Approx(result));
	}

	SECTION("MaxPerElem()")
	{
		const Vector4 result = Vector4::MaxPerElem(vec0, vec1);

		CHECK(result.GetX() == Approx(std::max(vec0.GetX(), vec1.GetX())));
		CHECK(result.GetY() == Approx(std::max(vec0.GetY(), vec1.GetY())));
		CHECK(result.GetZ() == Approx(std::max(vec0.GetZ(), vec1.GetZ())));
		CHECK(result.GetW() == Approx(std::max(vec0.GetW(), vec1.GetW())));
	}

	SECTION("MinPerElem()")
	{
		const Vector4 result = Vector4::MinPerElem(vec0, vec1);

		CHECK(result.GetX() == Approx(std::min(vec0.GetX(), vec1.GetX())));
		CHECK(result.GetY() == Approx(std::min(vec0.GetY(), vec1.GetY())));
		CHECK(result.GetZ() == Approx(std::min(vec0.GetZ(), vec1.GetZ())));
		CHECK(result.GetW() == Approx(std::min(vec0.GetW(), vec1.GetW())));
	}
}
