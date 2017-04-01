#include "stdafx.h"
#include "Script/Lua/API/LuaScriptAPI.h"
#include "Core/math/vectormath.h"
#include "Core/math/math.h"
#include "Core/Timer.h"

void RegisterVectormathFunctions_Lua(lua_State *L)
{
	// Vector3
	luabridge::getGlobalNamespace(L).beginClass<Vector3>("Vector3")
		.addConstructor<void(*)(float, float, float)>()
		.addProperty("x", &Vector3::GetX, &Vector3::SetX)
		.addProperty("y", &Vector3::GetY, &Vector3::SetY)
		.addProperty("z", &Vector3::GetZ, &Vector3::SetZ)
		.addFunction<Vector3(Vector3::*)() const>("__unm", &Vector3::operator -)
		.addFunction("__add", &Vector3::operator +)
		.addFunction<Vector3(Vector3::*)(const Vector3&) const>("__sub", &Vector3::operator -)
		.addFunction("__mul", &Vector3::operator *)
		.addFunction("__div", &Vector3::operator /)
		.addFunction("GetMaxElem", &Vector3::GetMaxElem)
		.addFunction("GetMinElem", &Vector3::GetMinElem)
		.addFunction("Dot", &Vector3::Dot)
		.addFunction("Cross", &Vector3::Cross)
		.addFunction("LengthSqr", &Vector3::LengthSqr)
		.addFunction("Length", &Vector3::Length)
		.addFunction("Normalize", &Vector3::Normalize)
		.addFunction("MulPerElem", &Vector3::MulPerElem)
		.addFunction("DivPerElem", &Vector3::DivPerElem)
		.addFunction("AbsPerElem", &Vector3::AbsPerElem)
		.addStaticFunction("MaxPerElem", &Vector3::MaxPerElem)
		.addStaticFunction("MinPerElem", &Vector3::MinPerElem)
		.addStaticFunction("Slerp", &Vector3::Slerp)
		.addStaticFunction("AxisX", &Vector3::AxisX)
		.addStaticFunction("AxisY", &Vector3::AxisY)
		.addStaticFunction("AxisZ", &Vector3::AxisZ)
		.endClass();

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
		.addFunction<Vector4(Vector4::*)() const>("__unm", &Vector4::operator -)
		.addFunction("__add", &Vector4::operator +)
		.addFunction<Vector4(Vector4::*)(const Vector4&) const>("__sub", &Vector4::operator -)
		.addFunction("__mul", &Vector4::operator *)
		.addFunction("__div", &Vector4::operator /)
		.addFunction("GetMaxElem", &Vector4::GetMaxElem)
		.addFunction("GetMinElem", &Vector4::GetMinElem)
		.addFunction("Dot", &Vector4::Dot)
		.addFunction("LengthSqr", &Vector4::LengthSqr)
		.addFunction("Length", &Vector4::Length)
		.addFunction("Normalize", &Vector4::Normalize)
		.addFunction("MulPerElem", &Vector4::MulPerElem)
		.addFunction("DivPerElem", &Vector4::DivPerElem)
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
//		.addConstructor<void(*)(const Vector3&, float)>()
//		.addConstructor<void(*)(const Matrix3&)>()
		.addFunction("SetXYZ", &Quat::SetXYZ)
		.addFunction("GetXYZ", &Quat::GetXYZ)
		.addProperty("x", &Quat::GetX, &Quat::SetX)
		.addProperty("y", &Quat::GetY, &Quat::SetY)
		.addProperty("z", &Quat::GetZ, &Quat::SetZ)
		.addProperty("w", &Quat::GetW, &Quat::SetW)
		.addFunction<Quat(Quat::*)() const>("__unm", &Quat::operator -)
		.addFunction("__add", &Quat::operator +)
		.addFunction<Quat(Quat::*)(const Quat&) const>("__sub", &Quat::operator -)
//		.SquirrelFunc("__mul", &SquirrelScriptAPI<Quat>::OpMulOverrides<const Quat&, float>)
		.addFunction("__div", &Quat::operator /)
		.addFunction("Dot", &Quat::Dot)
		.addFunction("Norm", &Quat::Norm)
		.addFunction("Length", &Quat::Length)
		.addFunction("Conjugate", &Quat::Conjugate)
		.addFunction("RotateVec", &Quat::RotateVec)
		.addFunction("Normalize", &Quat::Normalize)
		.addStaticFunction("Slerp", &Quat::Slerp)
		.addStaticFunction("Squad", &Quat::Squad)
		.addStaticFunction("Identity", &Quat::Identity)
//		.addStaticFunction<Quat(*)(const Vector3&, const Vector3&)>("CreateRotation", &Quat::CreateRotation)
		.addStaticFunction<Quat(*)(float, const Vector3&)>("CreateRotation", &Quat::CreateRotation)
		.addStaticFunction("CreateRotationX", &Quat::CreateRotationX)
		.addStaticFunction("CreateRotationY", &Quat::CreateRotationY)
		.addStaticFunction("CreateRotationZ", &Quat::CreateRotationZ)
		.endClass();

	// Matrix3
	luabridge::getGlobalNamespace(L).beginClass<Matrix3>("Matrix3")
		.addConstructor<void(*)(const Vector3&, const Vector3&, const Vector3&)>()
//		.addConstructor<void(*)(const Quat&)>()
		.addData("col0", &Matrix3::col0)
		.addData("col1", &Matrix3::col1)
		.addData("col2", &Matrix3::col2)
		.addFunction("SetCol", &Matrix3::SetCol)
		.addFunction("GetCol", &Matrix3::GetCol)
		.addFunction("SetRow0", &Matrix3::SetRow0)
		.addFunction("GetRow0", &Matrix3::GetRow0)
		.addFunction<Matrix3(Matrix3::*)() const>("__unm", &Matrix3::operator -)
		.addFunction("__add", &Matrix3::operator +)
		.addFunction<Matrix3(Matrix3::*)(const Matrix3&) const>("__sub", &Matrix3::operator -)
//		.SquirrelFunc("__mul", &SquirrelScriptAPI<Matrix3>::OpMulOverrides<float, const Vector3&, const Matrix3&>)
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
//		.addStaticFunction<Matrix3(*)(float, const Vector3&)>("CreateRotation", &Matrix3::CreateRotation)
		.addStaticFunction<Matrix3(*)(const Quat&)>("CreateRotation", &Matrix3::CreateRotation)
		.addStaticFunction("CreateScale", &Matrix3::CreateScale)
		.endClass();

	// Matrix4
	luabridge::getGlobalNamespace(L).beginClass<Matrix4>("Matrix4")
		.addConstructor<void(*)(const Vector4&, const Vector4&, const Vector4&, const Vector4&)>()
//		.addConstructor<void(*)(const Matrix3&, const Vector3&)>()
//		.addConstructor<const Quat&, const Vector3&>()
		.addFunction("SetUpper3x3", &Matrix4::SetUpper3x3)
		.addFunction("GetUpper3x3", &Matrix4::GetUpper3x3)
		.addFunction("SetTranslation", &Matrix4::SetTranslation)
		.addFunction("GetTranslation", &Matrix4::GetTranslation)
		.addData("col0", &Matrix4::col0)
		.addData("col1", &Matrix4::col1)
		.addData("col2", &Matrix4::col2)
		.addData("col3", &Matrix4::col3)
		.addFunction("SetCol", &Matrix4::SetCol)
		.addFunction("GetCol", &Matrix4::GetCol)
		.addFunction("SetRow0", &Matrix4::SetRow0)
		.addFunction("GetRow0", &Matrix4::GetRow0)
		.addFunction<Matrix4(Matrix4::*)() const>("__unm", &Matrix4::operator -)
		.addFunction("__add", &Matrix4::operator +)
		.addFunction<Matrix4(Matrix4::*)(const Matrix4&) const>("__sub", &Matrix4::operator -)
//		.SquirrelFunc("__mul", &SquirrelScriptAPI<Matrix4>::OpMulOverrides<float, const Vector4&, const Vector3&, const Matrix4&>)
		.addFunction("AppendScale", &Matrix4::AppendScale)
		.addFunction("PrependScale", &Matrix4::PrependScale)
		.addFunction("Transpose", &Matrix4::Transpose)
		.addFunction("Invert", &Matrix4::Invert)
		.addFunction("AffineInvert", &Matrix4::AffineInvert)
		.addFunction("OrthoInvert", &Matrix4::OrthoInvert)
		.addFunction("CalculateDeterminant", &Matrix4::CalculateDeterminant)
		.addStaticFunction("CreateTranslation", &Matrix4::CreateTranslation)
		.addStaticFunction("CreateRotationX", &Matrix4::CreateRotationX)
		.addStaticFunction("CreateRotationY", &Matrix4::CreateRotationY)
		.addStaticFunction("CreateRotationZ", &Matrix4::CreateRotationZ)
		.addStaticFunction("CreateRotationZYX", &Matrix4::CreateRotationZYX)
//		.addStaticFunction<Matrix4(*)(float, const Vector3&)>("CreateRotation", &Matrix4::CreateRotation)
		.addStaticFunction<Matrix4(*)(const Quat&)>("CreateRotation", &Matrix4::CreateRotation)
		.addStaticFunction("CreateScale", &Matrix4::CreateScale)
		.endClass();
}

void RegisterMathFunctions_Lua(lua_State *L)
{
	luabridge::Namespace rootTable = luabridge::getGlobalNamespace(L);

	// Trigonometric functions
	rootTable.addFunction("cos", std::cosf);
	rootTable.addFunction("sin", std::sinf);
	rootTable.addFunction("tan", &std::tanf);
	rootTable.addFunction("acos", &std::acosf);
	rootTable.addFunction("asin", &std::asinf);
	rootTable.addFunction("atan", &std::atanf);
	rootTable.addFunction("atan2", &std::atan2f);
	// Hyberbolic functions
	rootTable.addFunction("cosh", &std::coshf);
	rootTable.addFunction("sinh", &std::sinhf);
	rootTable.addFunction("tanh", &std::tanhf);
	// Exponential and logarithmic functions
	rootTable.addFunction("log", &std::logf);
	rootTable.addFunction("log10", &std::log10f);
	// Power functions
	rootTable.addFunction("pow", &std::powf);
	rootTable.addFunction("sqrt", &std::sqrtf);
	// Absolute value functions
	rootTable.addFunction("fabsf", &std::fabsf);
	rootTable.addFunction<int(*)(int)>("abs", &std::abs);

	// Math
	luabridge::getGlobalNamespace(L).beginNamespace("Math")
		.addFunction("RandFloat", &Math::RandFloat)
		.addFunction("RandInt", &Math::RandInt)
		.addFunction("Round32", &Math::Round32)
		.addFunction("RoundUp", &Math::RoundUp)
		.addFunction("Clamp", &Math::Clamp)
		.addFunction("Clamp01", &Math::Clamp01)
		.endNamespace();
}

void RegisterCoreAPI_Lua(lua_State *L)
{
	RegisterVectormathFunctions_Lua(L);
	RegisterMathFunctions_Lua(L);

	// Timer
	luabridge::getGlobalNamespace(L).beginClass<Timer>("ITimer")
		.addFunction("GetTimeMicroSec", &Timer::GetTimeMicroSec)
		.addFunction("GetTimeMilliSec", &Timer::GetTimeMilliSec)
		.addFunction("GetTimeSec", &Timer::GetTimeSec)
		.addFunction("GetMicroDelta", &Timer::GetMicroDelta)
		.addFunction("GetMilliDelta", &Timer::GetMilliDelta)
		.addFunction("GetSecDelta", &Timer::GetSecDelta)
		.endClass();

	luabridge::setGlobal(L, Timer::Get(), "Timer");
}
