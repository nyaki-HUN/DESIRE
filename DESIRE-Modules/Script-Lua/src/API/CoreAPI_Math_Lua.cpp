#include "stdafx_Lua.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/Math/math.h"
#include "Engine/Core/Math/Rand.h"
#include "Engine/Core/Math/Transform.h"

void RegisterCoreAPI_Math_Lua(sol::state_view& lua)
{
	// Vector3
	auto vector3 = lua.new_usertype<Vector3>("Vector3", sol::constructors<
		Vector3(),
		Vector3(float, float, float),
		Vector3(float) >());
	vector3.set("x", sol::property(&Vector3::GetX, &Vector3::SetX));
	vector3.set("y", sol::property(&Vector3::GetY, &Vector3::SetY));
	vector3.set("z", sol::property(&Vector3::GetZ, &Vector3::SetZ));
	vector3.set_function("WithX", &Vector3::WithX);
	vector3.set_function("WithY", &Vector3::WithY);
	vector3.set_function("WithZ", &Vector3::WithZ);
	vector3.set_function(sol::meta_function::unary_minus, sol::resolve<Vector3() const>(&Vector3::operator -));
	vector3.set_function(sol::meta_function::addition, &Vector3::operator +);
	vector3.set_function(sol::meta_function::subtraction, sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator -));
	vector3.set_function(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator *),
		sol::resolve<Vector3(float) const>(&Vector3::operator *)));
	vector3.set_function(sol::meta_function::division, sol::overload(
		sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator /),
		sol::resolve<Vector3(float) const>(&Vector3::operator /)));
	vector3.set_function("Dot", &Vector3::Dot);
	vector3.set_function("Cross", &Vector3::Cross);
	vector3.set_function("LengthSqr", &Vector3::LengthSqr);
	vector3.set_function("Length", &Vector3::Length);
	vector3.set_function("Normalize", &Vector3::Normalize);
	vector3.set_function("Normalized", &Vector3::Normalized);
	vector3.set_function("AbsPerElem", &Vector3::AbsPerElem);
	vector3.set_function("GetMaxElem", &Vector3::GetMaxElem);
	vector3.set_function("GetMinElem", &Vector3::GetMinElem);
	vector3.set_function("MaxPerElem", &Vector3::MaxPerElem);
	vector3.set_function("MinPerElem", &Vector3::MinPerElem);
	vector3.set_function("Slerp", &Vector3::Slerp);
	vector3.set_function("Zero", &Vector3::Zero);
	vector3.set_function("One", &Vector3::One);
	vector3.set_function("AxisX", &Vector3::AxisX);
	vector3.set_function("AxisY", &Vector3::AxisY);
	vector3.set_function("AxisZ", &Vector3::AxisZ);

	// Vector4
	auto vector4 = lua.new_usertype<Vector4>("Vector4", sol::constructors<
		Vector4(),
		Vector4(float, float, float, float),
		Vector4(float),
		Vector4(const Vector3&, float),
		Vector4(const Vector3&) >());
	vector4.set_function("SetXYZ", &Vector4::SetXYZ);
	vector4.set_function("GetXYZ", &Vector4::GetXYZ);
	vector4.set("x", sol::property(&Vector4::GetX, &Vector4::SetX));
	vector4.set("y", sol::property(&Vector4::GetY, &Vector4::SetY));
	vector4.set("z", sol::property(&Vector4::GetZ, &Vector4::SetZ));
	vector4.set("w", sol::property(&Vector4::GetW, &Vector4::SetW));
	vector4.set_function("WithX", &Vector4::WithX);
	vector4.set_function("WithY", &Vector4::WithY);
	vector4.set_function("WithZ", &Vector4::WithZ);
	vector4.set_function("WithW", &Vector4::WithW);
	vector4.set_function(sol::meta_function::unary_minus, sol::resolve<Vector4() const>(&Vector4::operator -));
	vector4.set_function(sol::meta_function::addition, &Vector4::operator +);
	vector4.set_function(sol::meta_function::subtraction, sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator -));
	vector4.set_function(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator *),
		sol::resolve<Vector4(float) const>(&Vector4::operator *)));
	vector4.set_function(sol::meta_function::division, sol::overload(
		sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator /),
		sol::resolve<Vector4(float) const>(&Vector4::operator /)));
	vector4.set_function("Dot", &Vector4::Dot);
	vector4.set_function("LengthSqr", &Vector4::LengthSqr);
	vector4.set_function("Length", &Vector4::Length);
	vector4.set_function("Normalize", &Vector4::Normalize);
	vector4.set_function("Normalized", &Vector4::Normalized);
	vector4.set_function("AbsPerElem", &Vector4::AbsPerElem);
	vector4.set_function("GetMaxElem", &Vector4::GetMaxElem);
	vector4.set_function("GetMinElem", &Vector4::GetMinElem);
	vector4.set_function("MaxPerElem", &Vector4::MaxPerElem);
	vector4.set_function("MinPerElem", &Vector4::MinPerElem);
	vector4.set_function("Slerp", &Vector4::Slerp);
	vector4.set_function("AxisX", &Vector4::AxisX);
	vector4.set_function("AxisY", &Vector4::AxisY);
	vector4.set_function("AxisZ", &Vector4::AxisZ);
	vector4.set_function("AxisZ", &Vector4::AxisW);

	// Quat
	auto quat = lua.new_usertype<Quat>("Quat", sol::constructors<
		Quat(),
		Quat(float, float, float, float),
		Quat(const Matrix3&) >());
	quat.set_function(sol::meta_function::unary_minus, sol::resolve<Quat() const>(&Quat::operator -));
	quat.set_function(sol::meta_function::addition, &Quat::operator +);
	quat.set_function(sol::meta_function::subtraction, sol::resolve<Quat(const Quat&) const>(&Quat::operator -));
	quat.set_function(sol::meta_function::multiplication, &Quat::operator *);
	quat.set_function("Dot", &Quat::Dot);
	quat.set_function("Norm", &Quat::Norm);
	quat.set_function("Length", &Quat::Length);
	quat.set_function("Conjugate", &Quat::Conjugate);
	quat.set_function("EulerAngles", &Quat::EulerAngles);
	quat.set_function("RotateVec", &Quat::RotateVec);
	quat.set_function("Normalize", &Quat::Normalize);
	quat.set_function("Normalized", &Quat::Normalized);
	quat.set_function("Slerp", &Quat::Slerp);
	quat.set_function("Squad", &Quat::Squad);
	quat.set_function("Identity", &Quat::Identity);
	quat.set_function("CreateRotation", &Quat::CreateRotation);
	quat.set_function("CreateRotationX", &Quat::CreateRotationX);
	quat.set_function("CreateRotationY", &Quat::CreateRotationY);
	quat.set_function("CreateRotationZ", &Quat::CreateRotationZ);
	quat.set_function("CreateRotationFromEulerAngles", &Quat::CreateRotationFromEulerAngles);

	// Matrix3
	auto matrix3 = lua.new_usertype<Matrix3>("Matrix3", sol::constructors<
		Matrix3(),
		Matrix3(const Vector3&, const Vector3&, const Vector3&),
		Matrix3(const Quat&) >());
	matrix3.set("col0", &Matrix3::col0);
	matrix3.set("col1", &Matrix3::col1);
	matrix3.set("col2", &Matrix3::col2);
	matrix3.set_function("SetCol", &Matrix3::SetCol);
	matrix3.set_function("GetCol", &Matrix3::GetCol);
	matrix3.set_function("SetRow0", &Matrix3::SetRow0);
	matrix3.set_function("GetRow0", &Matrix3::GetRow0);
	matrix3.set_function(sol::meta_function::unary_minus, sol::resolve<Matrix3() const>(&Matrix3::operator -));
	matrix3.set_function(sol::meta_function::addition, &Matrix3::operator +);
	matrix3.set_function(sol::meta_function::subtraction, sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator -));
	matrix3.set_function(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator *),
		sol::resolve<Vector3(const Vector3&) const>(&Matrix3::operator *),
		sol::resolve<Matrix3(float) const>(&Matrix3::operator *)));
	matrix3.set_function("AppendScale", &Matrix3::AppendScale);
	matrix3.set_function("PrependScale", &Matrix3::PrependScale);
	matrix3.set_function("Transpose", &Matrix3::Transpose);
	matrix3.set_function("Invert", &Matrix3::Invert);
	matrix3.set_function("CalculateDeterminant", &Matrix3::CalculateDeterminant);
	matrix3.set_function("Identity", &Matrix3::Identity);
	matrix3.set_function("CreateRotationX", &Matrix3::CreateRotationX);
	matrix3.set_function("CreateRotationY", &Matrix3::CreateRotationY);
	matrix3.set_function("CreateRotationZ", &Matrix3::CreateRotationZ);
	matrix3.set_function("CreateRotationZYX", &Matrix3::CreateRotationZYX);
	matrix3.set_function("CreateRotation", &Matrix3::CreateRotation);
	matrix3.set_function("CreateScale", &Matrix3::CreateScale);

	// Matrix4
	auto matrix4 = lua.new_usertype<Matrix4>("Matrix4", sol::constructors<
		Matrix4(),
		Matrix4(const Vector4&, const Vector4&, const Vector4&, const Vector4&),
		Matrix4(const Matrix3&, const Vector3&),
		Matrix4(const Quat&, const Vector3&) >());
	matrix4.set_function("SetUpper3x3", &Matrix4::SetUpper3x3);
	matrix4.set_function("GetUpper3x3", &Matrix4::GetUpper3x3);
	matrix4.set_function("SetTranslation", &Matrix4::SetTranslation);
	matrix4.set_function("GetTranslation", &Matrix4::GetTranslation);
	matrix4.set("col0", &Matrix4::col0);
	matrix4.set("col1", &Matrix4::col1);
	matrix4.set("col2", &Matrix4::col2);
	matrix4.set("col3", &Matrix4::col3);
	matrix4.set_function("SetCol", &Matrix4::SetCol);
	matrix4.set_function("GetCol", &Matrix4::GetCol);
	matrix4.set_function("SetRow0", &Matrix4::SetRow0);
	matrix4.set_function("GetRow0", &Matrix4::GetRow0);
	matrix4.set_function(sol::meta_function::unary_minus, sol::resolve<Matrix4() const>(&Matrix4::operator -));
	matrix4.set_function(sol::meta_function::addition, &Matrix4::operator +);
	matrix4.set_function(sol::meta_function::subtraction, sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator -));
	matrix4.set_function(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator *),
		sol::resolve<Vector4(const Vector4&) const>(&Matrix4::operator *),
		sol::resolve<Vector4(const Vector3&) const>(&Matrix4::operator *),
		sol::resolve<Matrix4(float) const>(&Matrix4::operator *)));
	matrix4.set_function("AppendScale", &Matrix4::AppendScale);
	matrix4.set_function("PrependScale", &Matrix4::PrependScale);
	matrix4.set_function("Transpose", &Matrix4::Transpose);
	matrix4.set_function("Invert", &Matrix4::Invert);
	matrix4.set_function("AffineInvert", &Matrix4::AffineInvert);
	matrix4.set_function("OrthoInvert", &Matrix4::OrthoInvert);
	matrix4.set_function("CalculateDeterminant", &Matrix4::CalculateDeterminant);
	matrix4.set_function("CreateTranslation", &Matrix4::CreateTranslation);
	matrix4.set_function("CreateRotationX", &Matrix4::CreateRotationX);
	matrix4.set_function("CreateRotationY", &Matrix4::CreateRotationY);
	matrix4.set_function("CreateRotationZ", &Matrix4::CreateRotationZ);
	matrix4.set_function("CreateRotationZYX", &Matrix4::CreateRotationZYX);
	matrix4.set_function("CreateRotation", &Matrix4::CreateRotation);
	matrix4.set_function("CreateScale", &Matrix4::CreateScale);

	// Transform
	auto transform = lua.new_usertype<Transform>("Transform");
	transform.set("localPosition", sol::property(&Transform::GetLocalPosition, &Transform::SetLocalPosition));
	transform.set("localRotation", sol::property(&Transform::GetLocalRotation, &Transform::SetLocalRotation));
	transform.set("localScale", sol::property(&Transform::GetLocalScale, &Transform::SetLocalScale));
	transform.set("position", sol::property(&Transform::GetPosition, &Transform::SetPosition));
	transform.set("rotation", sol::property(&Transform::GetRotation, &Transform::SetRotation));
	transform.set("scale", sol::property(&Transform::GetScale, &Transform::SetScale));

	// Rand
	auto rand = lua.new_usertype<Rand>("Rand");
	rand.set_function("GetInt", &Rand::GetInt);
	rand.set_function("GetUint", &Rand::GetUint);
	rand.set_function("GetFloat", &Rand::GetFloat);
	rand.set_function("GetDouble", &Rand::GetDouble);
	rand.set_function("GetBool", &Rand::GetBool);
	lua["s_globalRand"] = &Rand::s_globalRand;

	// Math
	auto math = lua.create_named_table("Math");
	math.set_function("Round32", &Math::Round32);
	math.set_function("RoundUp", &Math::RoundUp);

	// Trigonometric functions
	lua.set_function("cos", &std::cosf);
	lua.set_function("sin", &std::sinf);
	lua.set_function("tan", &std::tanf);
	lua.set_function("acos", &std::acosf);
	lua.set_function("asin", &std::asinf);
	lua.set_function("atan", &std::atanf);
	lua.set_function("atan2", &std::atan2f);
	// Hyberbolic functions
	lua.set_function("cosh", &std::coshf);
	lua.set_function("sinh", &std::sinhf);
	lua.set_function("tanh", &std::tanhf);
	// Exponential and logarithmic functions
	lua.set_function("log", &std::logf);
	lua.set_function("log10", &std::log10f);
	// Power functions
	lua.set_function("pow", &std::powf);
	lua.set_function("sqrt", &std::sqrtf);
	// Absolute value functions
	lua.set_function("fabsf", &std::fabsf);
	lua.set_function("abs", sol::resolve<int(int)>(&std::abs));
}
