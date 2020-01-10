#include "API/LuaScriptAPI.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/Math/math.h"
#include "Engine/Core/Math/Rand.h"
#include "Engine/Core/Math/Transform.h"

void RegisterCoreAPI_Math_Lua(sol::state_view& lua)
{
	// Vector3
	lua.new_usertype<Vector3>("Vector3",
		sol::constructors<Vector3(), Vector3(float, float, float), Vector3(float)>(),
		"x", sol::property(&Vector3::GetX, &Vector3::SetX),
		"y", sol::property(&Vector3::GetY, &Vector3::SetY),
		"z", sol::property(&Vector3::GetZ, &Vector3::SetZ),
		"WithX", &Vector3::WithX,
		"WithY", &Vector3::WithY,
		"WithZ", &Vector3::WithZ,
		sol::meta_function::unary_minus, sol::resolve<Vector3() const>(&Vector3::operator -),
		sol::meta_function::addition, &Vector3::operator +,
		sol::meta_function::subtraction, sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator -),
		sol::meta_function::multiplication, sol::overload(
			sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator *),
			sol::resolve<Vector3(float) const>(&Vector3::operator *)),
		sol::meta_function::division, sol::overload(
			sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator /),
			sol::resolve<Vector3(float) const>(&Vector3::operator /)),
		sol::meta_function::less_than, &Vector3::operator <,
		sol::meta_function::less_than_or_equal_to, &Vector3::operator <=,
		"GetMaxElem", &Vector3::GetMaxElem,
		"GetMinElem", &Vector3::GetMinElem,
		"Dot", &Vector3::Dot,
		"Cross", &Vector3::Cross,
		"LengthSqr", &Vector3::LengthSqr,
		"Length", &Vector3::Length,
		"Normalize", &Vector3::Normalize,
		"Normalized", &Vector3::Normalized,
		"AbsPerElem", &Vector3::AbsPerElem,
		"MaxPerElem", &Vector3::MaxPerElem,
		"MinPerElem", &Vector3::MinPerElem,
		"Slerp", &Vector3::Slerp,
		"Zero", &Vector3::Zero,
		"One", &Vector3::One,
		"AxisX", &Vector3::AxisX,
		"AxisY", &Vector3::AxisY,
		"AxisZ", &Vector3::AxisZ
	);

	// Vector4
	lua.new_usertype<Vector4>("Vector4",
		sol::constructors<Vector4(), Vector4(float, float, float, float), Vector4(float), Vector4(const Vector3&, float), Vector4(const Vector3&)>(),
		"SetXYZ", &Vector4::SetXYZ,
		"GetXYZ", &Vector4::GetXYZ,
		"x", &Vector4::GetX, &Vector4::SetX,
		"y", &Vector4::GetY, &Vector4::SetY,
		"z", &Vector4::GetZ, &Vector4::SetZ,
		"w", &Vector4::GetW, &Vector4::SetW,
		sol::meta_function::unary_minus, sol::resolve<Vector4() const>(&Vector4::operator -),
		sol::meta_function::addition, &Vector4::operator +,
		sol::meta_function::subtraction, sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator -),
		sol::meta_function::multiplication, sol::overload(
			sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator *),
			sol::resolve<Vector4(float) const>(&Vector4::operator *)),
		sol::meta_function::division, sol::overload(
			sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator /),
			sol::resolve<Vector4(float) const>(&Vector4::operator /)),
		"Dot", &Vector4::Dot,
		"LengthSqr", &Vector4::LengthSqr,
		"Length", &Vector4::Length,
		"Normalize", &Vector4::Normalize,
		"Normalized", &Vector4::Normalized,
		"AbsPerElem", &Vector4::AbsPerElem,
		"GetMaxElem", &Vector4::GetMaxElem,
		"GetMinElem", &Vector4::GetMinElem,
		"MaxPerElem", &Vector4::MaxPerElem,
		"MinPerElem", &Vector4::MinPerElem,
		"Slerp", &Vector4::Slerp,
		"AxisX", &Vector4::AxisX,
		"AxisY", &Vector4::AxisY,
		"AxisZ", &Vector4::AxisZ,
		"AxisZ", &Vector4::AxisW
	);

	// Quat
	lua.new_usertype<Quat>("Quat",
		sol::constructors<Quat(), Quat(float, float, float, float), Quat(const Matrix3&)>(),
		sol::meta_function::unary_minus, sol::resolve<Quat() const>(&Quat::operator -),
		sol::meta_function::addition, &Quat::operator +,
		sol::meta_function::subtraction, sol::resolve<Quat(const Quat&) const>(&Quat::operator -),
		sol::meta_function::multiplication, &Quat::operator *,
		"Dot", &Quat::Dot,
		"Norm", &Quat::Norm,
		"Length", &Quat::Length,
		"Conjugate", &Quat::Conjugate,
		"EulerAngles", &Quat::EulerAngles,
		"RotateVec", &Quat::RotateVec,
		"Normalize", &Quat::Normalize,
		"Normalized", &Quat::Normalized,
		"Slerp", &Quat::Slerp,
		"Squad", &Quat::Squad,
		"Identity", &Quat::Identity,
		"CreateRotation", &Quat::CreateRotation,
		"CreateRotationX", &Quat::CreateRotationX,
		"CreateRotationY", &Quat::CreateRotationY,
		"CreateRotationZ", &Quat::CreateRotationZ,
		"CreateRotationFromEulerAngles", &Quat::CreateRotationFromEulerAngles,
		"CreateRotationFromTo", &Quat::CreateRotationFromTo
	);

	// Matrix3
	lua.new_usertype<Matrix3>("Matrix3",
		sol::constructors<Matrix3(), Matrix3(const Vector3&, const Vector3&, const Vector3&), Matrix3(const Quat&)>(),
		"col0", &Matrix3::col0,
		"col1", &Matrix3::col1,
		"col2", &Matrix3::col2,
		"SetCol", &Matrix3::SetCol,
		"GetCol", &Matrix3::GetCol,
		"SetRow0", &Matrix3::SetRow0,
		"GetRow0", &Matrix3::GetRow0,
		sol::meta_function::unary_minus, sol::resolve<Matrix3() const>(&Matrix3::operator -),
		sol::meta_function::addition, &Matrix3::operator +,
		sol::meta_function::subtraction, sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator -),
		sol::meta_function::multiplication, sol::overload(
			sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator *),
			sol::resolve<Vector3(const Vector3&) const>(&Matrix3::operator *),
			sol::resolve<Matrix3(float) const>(&Matrix3::operator *)),
		"AppendScale", &Matrix3::AppendScale,
		"PrependScale", &Matrix3::PrependScale,
		"Transpose", &Matrix3::Transpose,
		"Invert", &Matrix3::Invert,
		"CalculateDeterminant", &Matrix3::CalculateDeterminant,
		"Identity", &Matrix3::Identity,
		"CreateRotationX", &Matrix3::CreateRotationX,
		"CreateRotationY", &Matrix3::CreateRotationY,
		"CreateRotationZ", &Matrix3::CreateRotationZ,
		"CreateRotationZYX", &Matrix3::CreateRotationZYX,
		"CreateRotation", &Matrix3::CreateRotation,
		"CreateScale", &Matrix3::CreateScale
	);

	// Matrix4
	lua.new_usertype<Matrix4>("Matrix4",
		sol::constructors<Matrix4(), Matrix4(const Vector4&, const Vector4&, const Vector4&, const Vector4&), Matrix4(const Matrix3&, const Vector3&), Matrix4(const Quat&, const Vector3&)>(),
		"SetUpper3x3", &Matrix4::SetUpper3x3,
		"GetUpper3x3", &Matrix4::GetUpper3x3,
		"SetTranslation", &Matrix4::SetTranslation,
		"GetTranslation", &Matrix4::GetTranslation,
		"col0", &Matrix4::col0,
		"col1", &Matrix4::col1,
		"col2", &Matrix4::col2,
		"col3", &Matrix4::col3,
		"SetCol", &Matrix4::SetCol,
		"GetCol", &Matrix4::GetCol,
		"SetRow0", &Matrix4::SetRow0,
		"GetRow0", &Matrix4::GetRow0,
		sol::meta_function::unary_minus, sol::resolve<Matrix4() const>(&Matrix4::operator -),
		sol::meta_function::addition, &Matrix4::operator +,
		sol::meta_function::subtraction, sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator -),
		sol::meta_function::multiplication, sol::overload(
			sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator *),
			sol::resolve<Vector4(const Vector4&) const>(&Matrix4::operator *),
			sol::resolve<Vector4(const Vector3&) const>(&Matrix4::operator *),
			sol::resolve<Matrix4(float) const>(&Matrix4::operator *)),
		"AppendScale", &Matrix4::AppendScale,
		"PrependScale", &Matrix4::PrependScale,
		"Transpose", &Matrix4::Transpose,
		"Invert", &Matrix4::Invert,
		"AffineInvert", &Matrix4::AffineInvert,
		"OrthoInvert", &Matrix4::OrthoInvert,
		"CalculateDeterminant", &Matrix4::CalculateDeterminant,
		"CreateTranslation", &Matrix4::CreateTranslation,
		"CreateRotationX", &Matrix4::CreateRotationX,
		"CreateRotationY", &Matrix4::CreateRotationY,
		"CreateRotationZ", &Matrix4::CreateRotationZ,
		"CreateRotationZYX", &Matrix4::CreateRotationZYX,
		"CreateRotation", &Matrix4::CreateRotation,
		"CreateScale", &Matrix4::CreateScale
	);

	// Transform
	lua.new_usertype<Transform>("Transform",
		"localPosition", sol::property(&Transform::GetLocalPosition, &Transform::SetLocalPosition),
		"localRotation", sol::property(&Transform::GetLocalRotation, &Transform::SetLocalRotation),
		"localScale", sol::property(&Transform::GetLocalScale, &Transform::SetLocalScale),
		"position", sol::property(&Transform::GetPosition, &Transform::SetPosition),
		"rotation", sol::property(&Transform::GetRotation, &Transform::SetRotation),
		"scale", sol::property(&Transform::GetScale, &Transform::SetScale)
	);

	// Rand
	lua.new_usertype<Rand>("Rand",
		"GetInt", &Rand::GetInt,
		"GetUint", &Rand::GetUint,
		"GetFloat", &Rand::GetFloat,
		"GetDouble", &Rand::GetDouble,
		"GetBool", &Rand::GetBool
	);
	lua.set("s_globalRand", &Rand::s_globalRand);

	// Math
	lua.create_named_table("Math",
		"Round32", &Math::Round32,
		"RoundUp", &Math::RoundUp,
		"Clamp", &Math::Clamp,
		"Clamp01", &Math::Clamp01
	);

	// Trigonometric functions
	lua["cos"] = &std::cosf;
	lua["sin"] = &std::sinf;
	lua["tan"] = &std::tanf;
	lua["acos"] = &std::acosf;
	lua["asin"] = &std::asinf;
	lua["atan"] = &std::atanf;
	lua["atan2"] = &std::atan2f;
	// Hyberbolic functions
	lua["cosh"] = &std::coshf;
	lua["sinh"] = &std::sinhf;
	lua["tanh"] = &std::tanhf;
	// Exponential and logarithmic functions
	lua["log"] = &std::logf;
	lua["log10"] = &std::log10f;
	// Power functions
	lua["pow"] = &std::powf;
	lua["sqrt"] = &std::sqrtf;
	// Absolute value functions
	lua["fabsf"] = &std::fabsf;
	lua["abs"] = sol::resolve<int(int)>(&std::abs);
}
