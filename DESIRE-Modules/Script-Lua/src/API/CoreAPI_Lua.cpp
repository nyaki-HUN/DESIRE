#include "API/LuaScriptAPI.h"

#include "Engine/Application/Application.h"

#include "Engine/Core/Component.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Timer.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/Math/math.h"
#include "Engine/Core/Math/Rand.h"
#include "Engine/Core/Math/Transform.h"

void RegisterVectormathFunctions_Lua(sol::state_view& lua)
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
/*
	// Vector4
	luabridge::getGlobalNamespace(L).beginClass<Vector4>("Vector4")
		.addConstructor<void(*)(float, float, float, float)>()
//		.addConstructor<void(*)(const Vector3&, float)>()
		.addFunction("SetXYZ", &Vector4::SetXYZ)
		.addFunction("GetXYZ", &Vector4::GetXYZ)
		.addProperty("x", &Vector4::GetX, &Vector4::SetX)
		.addProperty("y", &Vector4::GetY, &Vector4::SetY)
		.addProperty("z", &Vector4::GetZ, &Vector4::SetZ)
		.addProperty("w", &Vector4::GetW, &Vector4::SetW)
		.addFunction<Vector4>("__unm", &Vector4::operator -)
		.addFunction("__add", &Vector4::operator +)
		.addFunction<Vector4, const Vector4&>("__sub", &Vector4::operator -)
//		.addFunction<Vector4, const Vector4&>("__mul", &Vector4::operator *)
		.addFunction<Vector4, float>("__mul", &Vector4::operator *)
//		.addFunction<Vector4, const Vector4&>("__div", &Vector4::operator /)
		.addFunction<Vector4, float>("__div", &Vector4::operator /)
		.addFunction("GetMaxElem", &Vector4::GetMaxElem)
		.addFunction("GetMinElem", &Vector4::GetMinElem)
		.addFunction("Dot", &Vector4::Dot)
		.addFunction("LengthSqr", &Vector4::LengthSqr)
		.addFunction("Length", &Vector4::Length)
		.addFunction("Normalize", &Vector4::Normalize)
		.addFunction("Normalized", &Vector4::Normalized)
		.addFunction("AbsPerElem", &Vector4::AbsPerElem)
		.addStaticFunction("MaxPerElem", &Vector4::MaxPerElem)
		.addStaticFunction("MinPerElem", &Vector4::MinPerElem)
		.addStaticFunction("Slerp", &Vector4::Slerp)
		.addStaticFunction("AxisX", &Vector4::AxisX)
		.addStaticFunction("AxisY", &Vector4::AxisY)
		.addStaticFunction("AxisZ", &Vector4::AxisZ)
		.addStaticFunction("AxisZ", &Vector4::AxisW)
		.endClass();

	// Quat
	luabridge::getGlobalNamespace(L).beginClass<Quat>("Quat")
		.addConstructor<void(*)(float, float, float, float)>()
//		.addConstructor<void(*)(const Matrix3&)>()
		.addFunction<Quat>("__unm", &Quat::operator -)
		.addFunction("__add", &Quat::operator +)
		.addFunction<Quat, const Quat&>("__sub", &Quat::operator -)
		.addFunction("__mul", &Quat::operator *)
		.addFunction("Dot", &Quat::Dot)
		.addFunction("Norm", &Quat::Norm)
		.addFunction("Length", &Quat::Length)
		.addFunction("Conjugate", &Quat::Conjugate)
		.addFunction("EulerAngles", &Quat::EulerAngles)
		.addFunction("RotateVec", &Quat::RotateVec)
		.addFunction("Normalize", &Quat::Normalize)
		.addFunction("Normalized", &Quat::Normalized)
		.addStaticFunction("Slerp", &Quat::Slerp)
		.addStaticFunction("Squad", &Quat::Squad)
		.addStaticFunction("Identity", &Quat::Identity)
		.addStaticFunction("CreateRotation", &Quat::CreateRotation)
		.addStaticFunction("CreateRotationX", &Quat::CreateRotationX)
		.addStaticFunction("CreateRotationY", &Quat::CreateRotationY)
		.addStaticFunction("CreateRotationZ", &Quat::CreateRotationZ)
		.addStaticFunction("CreateRotationFromEulerAngles", &Quat::CreateRotationFromEulerAngles)
		.addStaticFunction("CreateRotationFromTo", &Quat::CreateRotationFromTo)
		.endClass();

	// Matrix3
	luabridge::getGlobalNamespace(L).beginClass<Matrix3>("Matrix3")
		.addConstructor<void(*)(const Vector3&, const Vector3&, const Vector3&)>()
//		.addConstructor<void(*)(const Quat&)>()
		.addProperty("col0", &Matrix3::col0)
		.addProperty("col1", &Matrix3::col1)
		.addProperty("col2", &Matrix3::col2)
		.addFunction("SetCol", &Matrix3::SetCol)
		.addFunction("GetCol", &Matrix3::GetCol)
		.addFunction("SetRow0", &Matrix3::SetRow0)
		.addFunction("GetRow0", &Matrix3::GetRow0)
		.addFunction<Matrix3>("__unm", &Matrix3::operator -)
		.addFunction("__add", &Matrix3::operator +)
		.addFunction<Matrix3, const Matrix3&>("__sub", &Matrix3::operator -)
//		.addFunction("__mul", &SquirrelScriptAPI<Matrix3>::OpMulOverrides<float, const Vector3&, const Matrix3&>)
		.addFunction("AppendScale", &Matrix3::AppendScale)
		.addFunction("PrependScale", &Matrix3::PrependScale)
		.addFunction("Transpose", &Matrix3::Transpose)
		.addFunction("Invert", &Matrix3::Invert)
		.addFunction("CalculateDeterminant", &Matrix3::CalculateDeterminant)
		.addStaticFunction("Identity", &Matrix3::Identity)
		.addStaticFunction("CreateRotationX", &Matrix3::CreateRotationX)
		.addStaticFunction("CreateRotationY", &Matrix3::CreateRotationY)
		.addStaticFunction("CreateRotationZ", &Matrix3::CreateRotationZ)
		.addStaticFunction("CreateRotationZYX", &Matrix3::CreateRotationZYX)
		.addStaticFunction("CreateRotation", &Matrix3::CreateRotation)
		.addStaticFunction("CreateScale", &Matrix3::CreateScale)
		.endClass();
*/
	// Matrix4
	lua.new_usertype<Matrix4>("Matrix4",
		sol::constructors<Matrix4(), Matrix4(const Vector4&, const Vector4&, const Vector4&, const Vector4&)>(),
//		.addConstructor<void(*)(const Matrix3&, const Vector3&)>()
//		.addConstructor<void(*)(const Quat&, const Vector3&)>()
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
//		sol::meta_function::multiplication, sol::overload(
//			sol::resolve<Matrix4(float) const>(&Matrix4::operator *),
//			sol::resolve<Vector4(const Vector4&) const>(&Matrix4::operator *),
//			sol::resolve<Vector4(const Vector3&) const>(&Matrix4::operator *),
//			sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator *)),
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
}

void RegisterMathFunctions_Lua(sol::state_view& lua)
{
/*
	luabridge::getGlobalNamespace(L)
		// Trigonometric functions
		.addFunction("cos", &std::cosf)
		.addFunction("sin", &std::sinf)
		.addFunction("tan", &std::tanf)
		.addFunction("acos", &std::acosf)
		.addFunction("asin", &std::asinf)
		.addFunction("atan", &std::atanf)
		.addFunction("atan2", &std::atan2f)
		// Hyberbolic functions
		.addFunction("cosh", &std::coshf)
		.addFunction("sinh", &std::sinhf)
		.addFunction("tanh", &std::tanhf)
		// Exponential and logarithmic functions
		.addFunction("log", &std::logf)
		.addFunction("log10", &std::log10f)
		// Power functions
		.addFunction("pow", &std::powf)
		.addFunction("sqrt", &std::sqrtf)
		// Absolute value functions
		.addFunction("fabsf", &std::fabsf)
		.addFunction<int(*)(int)>("abs", &std::abs);

	// Math
	luabridge::getGlobalNamespace(L).beginNamespace("Math")
		.addFunction("Round32", &Math::Round32)
		.addFunction("RoundUp", &Math::RoundUp)
		.addFunction("Clamp", &Math::Clamp)
		.addFunction("Clamp01", &Math::Clamp01)
		.endNamespace();

	// Rand
	luabridge::getGlobalNamespace(L).beginClass<Rand>("Rand")
		.addFunction("GetInt", &Rand::GetInt)
		.addFunction("GetUint", &Rand::GetUint)
		.addFunction("GetFloat", &Rand::GetFloat)
		.addFunction("GetDouble", &Rand::GetDouble)
		.addFunction("GetBool", &Rand::GetBool)
		.endClass();

	luabridge::setGlobal(L, &Rand::s_globalRand, "globalRand");
*/
}

void RegisterCoreAPI_Lua(sol::state_view& lua)
{
	RegisterVectormathFunctions_Lua(lua);
	RegisterMathFunctions_Lua(lua);

	// Transform
	lua.new_usertype<Transform>("Transform",
		"localPosition", sol::property(&Transform::GetLocalPosition, &Transform::SetLocalPosition),
		"localRotation", sol::property(&Transform::GetLocalRotation, &Transform::SetLocalRotation),
		"localScale", sol::property(&Transform::GetLocalScale, &Transform::SetLocalScale),
		"position", sol::property(&Transform::GetPosition, &Transform::SetPosition),
		"rotation", sol::property(&Transform::GetRotation, &Transform::SetRotation),
		"scale", sol::property(&Transform::GetScale, &Transform::SetScale)
	);

	// Component
	lua.new_usertype<Component>("Component",
		"enabled", sol::property(&Component::IsEnabled, &Component::SetEnabled),
		"object", sol::property(&Component::GetObject)
	);

	// Object
	lua.new_usertype<Object>("Object",
		"GetObjectName", &Object::GetObjectName,
		"SetActive", &Object::SetActive,
		"IsActiveSelf", &Object::IsActiveSelf,
		"IsActiveInHierarchy", &Object::IsActiveInHierarchy,
		"RemoveComponent", &Object::RemoveComponent,
		"GetComponent", &Object::GetComponentByTypeId,
		"transform", sol::property(&Object::GetTransform),
		"GetParent", &Object::GetParent
	);

	// Timer
	lua.new_usertype<Timer>("ITimer",
		"GetTimeMicroSec", &Timer::GetTimeMicroSec,
		"GetTimeMilliSec", &Timer::GetTimeMilliSec,
		"GetTimeSec", &Timer::GetTimeSec,
		"GetMicroDelta", &Timer::GetMicroDelta,
		"GetMilliDelta", &Timer::GetMilliDelta,
		"GetSecDelta", &Timer::GetSecDelta);

	lua.set("Timer", Modules::Application->GetTimer());
}
