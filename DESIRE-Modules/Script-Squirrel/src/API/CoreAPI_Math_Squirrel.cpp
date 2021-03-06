#include "stdafx_Squirrel.h"
#include "API/SquirrelScriptAPI.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/Math/math.h"
#include "Engine/Core/Math/Rand.h"
#include "Engine/Core/Math/Transform.h"

void RegisterCoreAPI_Math_Squirrel(Sqrat::RootTable& rootTable)
{
	HSQUIRRELVM vm = rootTable.GetVM();

	// Vector3
	rootTable.Bind("Vector3", Sqrat::Class<Vector3>(vm, "Vector3")
		.Ctor()
		.Ctor<Vector3>()
		.Ctor<float, float, float>()
		.Func("SetX", &Vector3::SetX)
		.Func("SetY", &Vector3::SetY)
		.Func("SetZ", &Vector3::SetZ)
		.Func("GetX", &Vector3::GetX)
		.Func("GetY", &Vector3::GetY)
		.Func("GetZ", &Vector3::GetZ)
		.Func("WithX", &Vector3::WithX)
		.Func("WithY", &Vector3::WithY)
		.Func("WithZ", &Vector3::WithZ)
		.Func<Vector3(Vector3::*)() const>("_unm", &Vector3::operator -)
		.Func("_add", &Vector3::operator +)
		.Func<Vector3(Vector3::*)(const Vector3&) const>("_sub", &Vector3::operator -)
		.SquirrelFunc("_mul", &SquirrelScriptAPI<Vector3>::OpMulOverrides<
			const Vector3&,
			float >)
		.SquirrelFunc("_div", &SquirrelScriptAPI<Vector3>::OpDivOverrides<
			const Vector3&,
			float >)
		.Func("Dot", &Vector3::Dot)
		.Func("Cross", &Vector3::Cross)
		.Func("LengthSqr", &Vector3::LengthSqr)
		.Func("Length", &Vector3::Length)
		.Func("Normalize", &Vector3::Normalize)
		.Func("Normalized", &Vector3::Normalized)
		.Func("AbsPerElem", &Vector3::AbsPerElem)
		.Func("GetMaxElem", &Vector3::GetMaxElem)
		.Func("GetMinElem", &Vector3::GetMinElem)
		.StaticFunc("MaxPerElem", &Vector3::MaxPerElem)
		.StaticFunc("MinPerElem", &Vector3::MinPerElem)
		.StaticFunc("Zero", &Vector3::Zero)
		.StaticFunc("One", &Vector3::One)
		.StaticFunc("AxisX", &Vector3::AxisX)
		.StaticFunc("AxisY", &Vector3::AxisY)
		.StaticFunc("AxisZ", &Vector3::AxisZ)
	);

	// Vector4
	rootTable.Bind("Vector4", Sqrat::Class<Vector4>(vm, "Vector4")
		.Ctor()
		.Ctor<Vector4>()
		.Ctor<float, float, float, float>()
		.Ctor<const Vector3&, float>()
//		.Ctor<const Vector3&>()
		.Func("SetXYZ", &Vector4::SetXYZ)
		.Func("GetXYZ", &Vector4::GetXYZ)
		.Func("SetX", &Vector4::SetX)
		.Func("SetY", &Vector4::SetY)
		.Func("SetZ", &Vector4::SetZ)
		.Func("SetW", &Vector4::SetW)
		.Func("GetX", &Vector4::GetX)
		.Func("GetY", &Vector4::GetY)
		.Func("GetZ", &Vector4::GetZ)
		.Func("GetW", &Vector4::GetW)
		.Func("WithX", &Vector4::WithX)
		.Func("WithY", &Vector4::WithY)
		.Func("WithZ", &Vector4::WithZ)
		.Func("WithW", &Vector4::WithW)
		.Func<Vector4(Vector4::*)() const>("_unm", &Vector4::operator -)
		.Func("_add", &Vector4::operator +)
		.Func<Vector4(Vector4::*)(const Vector4&) const>("_sub", &Vector4::operator -)
		.SquirrelFunc("_mul", &SquirrelScriptAPI<Vector4>::OpMulOverrides<
			const Vector4&,
			float >)
		.SquirrelFunc("_div", &SquirrelScriptAPI<Vector4>::OpDivOverrides<
			const Vector4&,
			float >)
		.Func("Dot", &Vector4::Dot)
		.Func("LengthSqr", &Vector4::LengthSqr)
		.Func("Length", &Vector4::Length)
		.Func("Normalize", &Vector4::Normalize)
		.Func("Normalized", &Vector4::Normalized)
		.Func("AbsPerElem", &Vector4::AbsPerElem)
		.Func("GetMaxElem", &Vector4::GetMaxElem)
		.Func("GetMinElem", &Vector4::GetMinElem)
		.StaticFunc("MaxPerElem", &Vector4::MaxPerElem)
		.StaticFunc("MinPerElem", &Vector4::MinPerElem)
		.StaticFunc("AxisX", &Vector4::AxisX)
		.StaticFunc("AxisY", &Vector4::AxisY)
		.StaticFunc("AxisZ", &Vector4::AxisZ)
		.StaticFunc("AxisZ", &Vector4::AxisW)
	);

	// Quat
	rootTable.Bind("Quat", Sqrat::Class<Quat>(vm, "Quat")
		.Ctor()
		.Ctor<Quat>()
		.Ctor<float, float, float, float>()
//		.Ctor<const Matrix3&>()
		.Func<Quat(Quat::*)() const>("_unm", &Quat::operator -)
		.Func("_add", &Quat::operator +)
		.Func<Quat(Quat::*)(const Quat&) const>("_sub", &Quat::operator -)
		.Func("_mul", &Quat::operator *)
		.Func("Dot", &Quat::Dot)
		.Func("Norm", &Quat::Norm)
		.Func("Length", &Quat::Length)
		.Func("Conjugate", &Quat::Conjugate)
		.Func("EulerAngles", &Quat::EulerAngles)
		.Func("RotateVec", &Quat::RotateVec)
		.Func("Normalize", &Quat::Normalize)
		.Func("Normalized", &Quat::Normalized)
		.StaticFunc("Identity", &Quat::Identity)
		.StaticFunc("CreateRotation", &Quat::CreateRotation)
		.StaticFunc("CreateRotationX", &Quat::CreateRotationX)
		.StaticFunc("CreateRotationY", &Quat::CreateRotationY)
		.StaticFunc("CreateRotationZ", &Quat::CreateRotationZ)
		.StaticFunc("CreateRotationFromEulerAngles", &Quat::CreateRotationFromEulerAngles)
	);

	// Matrix3
	rootTable.Bind("Matrix3", Sqrat::Class<Matrix3>(vm, "Matrix3")
		.Ctor()
		.Ctor<const Matrix3&>()
		.Ctor<const Vector3&, const Vector3&, const Vector3&>()
//		.Ctor<const Quat&>()
		.Var("m_col0", &Matrix3::m_col0)
		.Var("m_col1", &Matrix3::m_col1)
		.Var("m_col2", &Matrix3::m_col2)
		.Func("SetCol", &Matrix3::SetCol)
		.Func("GetCol", &Matrix3::GetCol)
		.Func("SetRow0", &Matrix3::SetRow0)
		.Func("GetRow0", &Matrix3::GetRow0)
		.Func<Matrix3(Matrix3::*)() const>("_unm", &Matrix3::operator -)
		.Func("_add", &Matrix3::operator +)
		.Func<Matrix3(Matrix3::*)(const Matrix3&) const>("_sub", &Matrix3::operator -)
		.SquirrelFunc("_mul", &SquirrelScriptAPI<Matrix3>::OpMulOverrides<
			const Matrix3&,
			const Vector3&,
			float >)
		.Func("AppendScale", &Matrix3::AppendScale)
		.Func("PrependScale", &Matrix3::PrependScale)
		.Func("Transpose", &Matrix3::Transpose)
		.Func("Invert", &Matrix3::Invert)
		.Func("CalculateDeterminant", &Matrix3::CalculateDeterminant)
		.StaticFunc("Identity", &Matrix3::Identity)
		.StaticFunc("CreateRotationX", &Matrix3::CreateRotationX)
		.StaticFunc("CreateRotationY", &Matrix3::CreateRotationY)
		.StaticFunc("CreateRotationZ", &Matrix3::CreateRotationZ)
		.StaticFunc("CreateRotationZYX", &Matrix3::CreateRotationZYX)
		.StaticFunc("CreateRotation", &Matrix3::CreateRotation)
		.StaticFunc("CreateScale", &Matrix3::CreateScale)
	);

	// Matrix4
	rootTable.Bind("Matrix4", Sqrat::Class<Matrix4>(vm, "Matrix4")
		.Ctor()
		.Ctor<const Matrix4&>()
		.Ctor<const Vector4&, const Vector4&, const Vector4&, const Vector4&>()
		.Ctor<const Matrix3&, const Vector3&>()
//		.Ctor<const Quat&, const Vector3&>()
		.Func("SetUpper3x3", &Matrix4::SetUpper3x3)
		.Func("GetUpper3x3", &Matrix4::GetUpper3x3)
		.Func("SetTranslation", &Matrix4::SetTranslation)
		.Func("GetTranslation", &Matrix4::GetTranslation)
		.Var("m_col0", &Matrix4::m_col0)
		.Var("m_col1", &Matrix4::m_col1)
		.Var("m_col2", &Matrix4::m_col2)
		.Var("m_col3", &Matrix4::m_col3)
		.Func("SetCol", &Matrix4::SetCol)
		.Func("GetCol", &Matrix4::GetCol)
		.Func("SetRow0", &Matrix4::SetRow0)
		.Func("GetRow0", &Matrix4::GetRow0)
		.Func<Matrix4(Matrix4::*)() const>("_unm", &Matrix4::operator -)
		.Func("_add", &Matrix4::operator +)
		.Func<Matrix4(Matrix4::*)(const Matrix4&) const>("_sub", &Matrix4::operator -)
		.SquirrelFunc("_mul", &SquirrelScriptAPI<Matrix4>::OpMulOverrides<
			const Matrix4&,
			const Vector4&,
			const Vector3&,
			float >)
		.Func("AppendScale", &Matrix4::AppendScale)
		.Func("PrependScale", &Matrix4::PrependScale)
		.Func("Transpose", &Matrix4::Transpose)
		.Func("Invert", &Matrix4::Invert)
		.Func("AffineInvert", &Matrix4::AffineInvert)
		.Func("OrthoInvert", &Matrix4::OrthoInvert)
		.Func("CalculateDeterminant", &Matrix4::CalculateDeterminant)
		.StaticFunc("CreateTranslation", &Matrix4::CreateTranslation)
		.StaticFunc("CreateRotationX", &Matrix4::CreateRotationX)
		.StaticFunc("CreateRotationY", &Matrix4::CreateRotationY)
		.StaticFunc("CreateRotationZ", &Matrix4::CreateRotationZ)
		.StaticFunc("CreateRotationZYX", &Matrix4::CreateRotationZYX)
		.StaticFunc("CreateRotation", &Matrix4::CreateRotation)
		.StaticFunc("CreateScale", &Matrix4::CreateScale)
	);

	// Transform
	rootTable.Bind("Transform", Sqrat::Class<Transform, Sqrat::NoConstructor<Transform>>(vm, "Transform")
		.Func("SetLocalPosition", &Transform::SetLocalPosition)
		.Func("SetLocalRotation", &Transform::SetLocalRotation)
		.Func("SetLocalScale", &Transform::SetLocalScale)
		.Func("SetPosition", &Transform::SetPosition)
		.Func("SetRotation", &Transform::SetRotation)
		.Func("SetScale", &Transform::SetScale)
		.Func("GetLocalPosition", &Transform::GetLocalPosition)
		.Func("GetLocalRotation", &Transform::GetLocalRotation)
		.Func("GetLocalScale", &Transform::GetLocalScale)
		.Func("GetPosition", &Transform::GetPosition)
		.Func("GetRotation", &Transform::GetRotation)
		.Func("GetScale", &Transform::GetScale)
	);

	// Rand
	rootTable.Bind("Rand", Sqrat::Class<Rand, Sqrat::DefaultAllocator<Rand>>(vm, "Rand")
		.Func("GetInt", &Rand::GetInt)
		.Func("GetUint", &Rand::GetUint)
		.Func("GetFloat", &Rand::GetFloat)
		.Func("GetDouble", &Rand::GetDouble)
		.Func("GetBool", &Rand::GetBool)
	);
	rootTable.SetInstance("s_globalRand", &Rand::s_globalRand);

	// Math
	rootTable.Bind("Math", Sqrat::RootTable(vm)
		.Func("Round32", &Math::Round32)
		.Func("RoundUp", &Math::RoundUp)
	);

	// Trigonometric functions
	rootTable.Func("cos", &std::cosf);
	rootTable.Func("sin", &std::sinf);
	rootTable.Func("tan", &std::tanf);
	rootTable.Func("acos", &std::acosf);
	rootTable.Func("asin", &std::asinf);
	rootTable.Func("atan", &std::atanf);
	rootTable.Func("atan2", &std::atan2f);
	// Hyberbolic functions
	rootTable.Func("cosh", &std::coshf);
	rootTable.Func("sinh", &std::sinhf);
	rootTable.Func("tanh", &std::tanhf);
	// Exponential and logarithmic functions
	rootTable.Func("log", &std::logf);
	rootTable.Func("log10", &std::log10f);
	// Power functions
	rootTable.Func("pow", &std::powf);
	rootTable.Func("sqrt", &std::sqrtf);
	// Absolute value functions
	rootTable.Func("fabsf", &std::fabsf);
	rootTable.Func<int(int)>("abs", &std::abs);
}
