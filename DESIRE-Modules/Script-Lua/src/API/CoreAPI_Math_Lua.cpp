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
	vector3.set("WithX", &Vector3::WithX);
	vector3.set("WithY", &Vector3::WithY);
	vector3.set("WithZ", &Vector3::WithZ);
	vector3.set(sol::meta_function::unary_minus, sol::resolve<Vector3() const>(&Vector3::operator -));
	vector3.set(sol::meta_function::addition, &Vector3::operator +);
	vector3.set(sol::meta_function::subtraction, sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator -));
	vector3.set(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator *),
		sol::resolve<Vector3(float) const>(&Vector3::operator *)));
	vector3.set(sol::meta_function::division, sol::overload(
		sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator /),
		sol::resolve<Vector3(float) const>(&Vector3::operator /)));
	vector3.set("Dot", &Vector3::Dot);
	vector3.set("Cross", &Vector3::Cross);
	vector3.set("LengthSqr", &Vector3::LengthSqr);
	vector3.set("Length", &Vector3::Length);
	vector3.set("Normalize", &Vector3::Normalize);
	vector3.set("Normalized", &Vector3::Normalized);
	vector3.set("AbsPerElem", &Vector3::AbsPerElem);
	vector3.set("GetMaxElem", &Vector3::GetMaxElem);
	vector3.set("GetMinElem", &Vector3::GetMinElem);
	vector3.set("MaxPerElem", &Vector3::MaxPerElem);
	vector3.set("MinPerElem", &Vector3::MinPerElem);
	vector3.set("Slerp", &Vector3::Slerp);
	vector3.set("Zero", &Vector3::Zero);
	vector3.set("One", &Vector3::One);
	vector3.set("AxisX", &Vector3::AxisX);
	vector3.set("AxisY", &Vector3::AxisY);
	vector3.set("AxisZ", &Vector3::AxisZ);

	// Vector4
	auto vector4 = lua.new_usertype<Vector4>("Vector4", sol::constructors<
		Vector4(),
		Vector4(float, float, float, float),
		Vector4(float),
		Vector4(const Vector3&, float),
		Vector4(const Vector3&) >());
	vector4.set("SetXYZ", &Vector4::SetXYZ);
	vector4.set("GetXYZ", &Vector4::GetXYZ);
	vector4.set("x", sol::property(&Vector4::GetX, &Vector4::SetX));
	vector4.set("y", sol::property(&Vector4::GetY, &Vector4::SetY));
	vector4.set("z", sol::property(&Vector4::GetZ, &Vector4::SetZ));
	vector4.set("w", sol::property(&Vector4::GetW, &Vector4::SetW));
	vector4.set("WithX", &Vector4::WithX);
	vector4.set("WithY", &Vector4::WithY);
	vector4.set("WithZ", &Vector4::WithZ);
	vector4.set("WithW", &Vector4::WithW);
	vector4.set(sol::meta_function::unary_minus, sol::resolve<Vector4() const>(&Vector4::operator -));
	vector4.set(sol::meta_function::addition, &Vector4::operator +);
	vector4.set(sol::meta_function::subtraction, sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator -));
	vector4.set(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator *),
		sol::resolve<Vector4(float) const>(&Vector4::operator *)));
	vector4.set(sol::meta_function::division, sol::overload(
		sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator /),
		sol::resolve<Vector4(float) const>(&Vector4::operator /)));
	vector4.set("Dot", &Vector4::Dot);
	vector4.set("LengthSqr", &Vector4::LengthSqr);
	vector4.set("Length", &Vector4::Length);
	vector4.set("Normalize", &Vector4::Normalize);
	vector4.set("Normalized", &Vector4::Normalized);
	vector4.set("AbsPerElem", &Vector4::AbsPerElem);
	vector4.set("GetMaxElem", &Vector4::GetMaxElem);
	vector4.set("GetMinElem", &Vector4::GetMinElem);
	vector4.set("MaxPerElem", &Vector4::MaxPerElem);
	vector4.set("MinPerElem", &Vector4::MinPerElem);
	vector4.set("Slerp", &Vector4::Slerp);
	vector4.set("AxisX", &Vector4::AxisX);
	vector4.set("AxisY", &Vector4::AxisY);
	vector4.set("AxisZ", &Vector4::AxisZ);
	vector4.set("AxisZ", &Vector4::AxisW);

	// Quat
	auto quat = lua.new_usertype<Quat>("Quat", sol::constructors<
		Quat(),
		Quat(float, float, float, float),
		Quat(const Matrix3&) >());
	quat.set(sol::meta_function::unary_minus, sol::resolve<Quat() const>(&Quat::operator -));
	quat.set(sol::meta_function::addition, &Quat::operator +);
	quat.set(sol::meta_function::subtraction, sol::resolve<Quat(const Quat&) const>(&Quat::operator -));
	quat.set(sol::meta_function::multiplication, &Quat::operator *);
	quat.set("Dot", &Quat::Dot);
	quat.set("Norm", &Quat::Norm);
	quat.set("Length", &Quat::Length);
	quat.set("Conjugate", &Quat::Conjugate);
	quat.set("EulerAngles", &Quat::EulerAngles);
	quat.set("RotateVec", &Quat::RotateVec);
	quat.set("Normalize", &Quat::Normalize);
	quat.set("Normalized", &Quat::Normalized);
	quat.set("Slerp", &Quat::Slerp);
	quat.set("Squad", &Quat::Squad);
	quat.set("Identity", &Quat::Identity);
	quat.set("CreateRotation", &Quat::CreateRotation);
	quat.set("CreateRotationX", &Quat::CreateRotationX);
	quat.set("CreateRotationY", &Quat::CreateRotationY);
	quat.set("CreateRotationZ", &Quat::CreateRotationZ);
	quat.set("CreateRotationFromEulerAngles", &Quat::CreateRotationFromEulerAngles);
	quat.set("CreateRotationFromTo", &Quat::CreateRotationFromTo);

	// Matrix3
	auto matrix3 = lua.new_usertype<Matrix3>("Matrix3", sol::constructors<
		Matrix3(),
		Matrix3(const Vector3&, const Vector3&, const Vector3&),
		Matrix3(const Quat&) >());
	matrix3.set("col0", &Matrix3::col0);
	matrix3.set("col1", &Matrix3::col1);
	matrix3.set("col2", &Matrix3::col2);
	matrix3.set("SetCol", &Matrix3::SetCol);
	matrix3.set("GetCol", &Matrix3::GetCol);
	matrix3.set("SetRow0", &Matrix3::SetRow0);
	matrix3.set("GetRow0", &Matrix3::GetRow0);
	matrix3.set(sol::meta_function::unary_minus, sol::resolve<Matrix3() const>(&Matrix3::operator -));
	matrix3.set(sol::meta_function::addition, &Matrix3::operator +);
	matrix3.set(sol::meta_function::subtraction, sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator -));
	matrix3.set(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator *),
		sol::resolve<Vector3(const Vector3&) const>(&Matrix3::operator *),
		sol::resolve<Matrix3(float) const>(&Matrix3::operator *)));
	matrix3.set("AppendScale", &Matrix3::AppendScale);
	matrix3.set("PrependScale", &Matrix3::PrependScale);
	matrix3.set("Transpose", &Matrix3::Transpose);
	matrix3.set("Invert", &Matrix3::Invert);
	matrix3.set("CalculateDeterminant", &Matrix3::CalculateDeterminant);
	matrix3.set("Identity", &Matrix3::Identity);
	matrix3.set("CreateRotationX", &Matrix3::CreateRotationX);
	matrix3.set("CreateRotationY", &Matrix3::CreateRotationY);
	matrix3.set("CreateRotationZ", &Matrix3::CreateRotationZ);
	matrix3.set("CreateRotationZYX", &Matrix3::CreateRotationZYX);
	matrix3.set("CreateRotation", &Matrix3::CreateRotation);
	matrix3.set("CreateScale", &Matrix3::CreateScale);

	// Matrix4
	auto matrix4 = lua.new_usertype<Matrix4>("Matrix4", sol::constructors<
		Matrix4(),
		Matrix4(const Vector4&, const Vector4&, const Vector4&, const Vector4&),
		Matrix4(const Matrix3&, const Vector3&),
		Matrix4(const Quat&, const Vector3&) >());
	matrix4.set("SetUpper3x3", &Matrix4::SetUpper3x3);
	matrix4.set("GetUpper3x3", &Matrix4::GetUpper3x3);
	matrix4.set("SetTranslation", &Matrix4::SetTranslation);
	matrix4.set("GetTranslation", &Matrix4::GetTranslation);
	matrix4.set("col0", &Matrix4::col0);
	matrix4.set("col1", &Matrix4::col1);
	matrix4.set("col2", &Matrix4::col2);
	matrix4.set("col3", &Matrix4::col3);
	matrix4.set("SetCol", &Matrix4::SetCol);
	matrix4.set("GetCol", &Matrix4::GetCol);
	matrix4.set("SetRow0", &Matrix4::SetRow0);
	matrix4.set("GetRow0", &Matrix4::GetRow0);
	matrix4.set(sol::meta_function::unary_minus, sol::resolve<Matrix4() const>(&Matrix4::operator -));
	matrix4.set(sol::meta_function::addition, &Matrix4::operator +);
	matrix4.set(sol::meta_function::subtraction, sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator -));
	matrix4.set(sol::meta_function::multiplication, sol::overload(
		sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator *),
		sol::resolve<Vector4(const Vector4&) const>(&Matrix4::operator *),
		sol::resolve<Vector4(const Vector3&) const>(&Matrix4::operator *),
		sol::resolve<Matrix4(float) const>(&Matrix4::operator *)));
	matrix4.set("AppendScale", &Matrix4::AppendScale);
	matrix4.set("PrependScale", &Matrix4::PrependScale);
	matrix4.set("Transpose", &Matrix4::Transpose);
	matrix4.set("Invert", &Matrix4::Invert);
	matrix4.set("AffineInvert", &Matrix4::AffineInvert);
	matrix4.set("OrthoInvert", &Matrix4::OrthoInvert);
	matrix4.set("CalculateDeterminant", &Matrix4::CalculateDeterminant);
	matrix4.set("CreateTranslation", &Matrix4::CreateTranslation);
	matrix4.set("CreateRotationX", &Matrix4::CreateRotationX);
	matrix4.set("CreateRotationY", &Matrix4::CreateRotationY);
	matrix4.set("CreateRotationZ", &Matrix4::CreateRotationZ);
	matrix4.set("CreateRotationZYX", &Matrix4::CreateRotationZYX);
	matrix4.set("CreateRotation", &Matrix4::CreateRotation);
	matrix4.set("CreateScale", &Matrix4::CreateScale);

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
	rand.set("GetInt", &Rand::GetInt);
	rand.set("GetUint", &Rand::GetUint);
	rand.set("GetFloat", &Rand::GetFloat);
	rand.set("GetDouble", &Rand::GetDouble);
	rand.set("GetBool", &Rand::GetBool);
	lua.set("s_globalRand", &Rand::s_globalRand);

	// Math
	auto math = lua.create_named_table("Math");
	math.set("Round32", &Math::Round32);
	math.set("RoundUp", &Math::RoundUp);
	math.set("Clamp", &Math::Clamp);
	math.set("Clamp01", &Math::Clamp01);

	// Trigonometric functions
	lua.set("cos", &std::cosf);
	lua.set("sin", &std::sinf);
	lua.set("tan", &std::tanf);
	lua.set("acos", &std::acosf);
	lua.set("asin", &std::asinf);
	lua.set("atan", &std::atanf);
	lua.set("atan2", &std::atan2f);
	// Hyberbolic functions
	lua.set("cosh", &std::coshf);
	lua.set("sinh", &std::sinhf);
	lua.set("tanh", &std::tanhf);
	// Exponential and logarithmic functions
	lua.set("log", &std::logf);
	lua.set("log10", &std::log10f);
	// Power functions
	lua.set("pow", &std::powf);
	lua.set("sqrt", &std::sqrtf);
	// Absolute value functions
	lua.set("fabsf", &std::fabsf);
	lua.set("abs", sol::resolve<int(int)>(&std::abs));
}
