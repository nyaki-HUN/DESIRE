#include "stdafx.h"
#include "Script/AngelScript/API/AngelScriptAPI.h"
#include "Core/math/vectormath.h"
#include "Core/math/math.h"
#include "Core/Timer.h"

Vector3* Vector3_Cross(const Vector3& vec0, const Vector3& vec1)
{
	return new Vector3(vec0.Cross(vec1));
}

void RegisterVectormathFunctions_AngelScript(asIScriptEngine *engine)
{
	// Vector3
	engine->RegisterObjectType("Vector3", 0, asOBJ_REF | asOBJ_SCOPED);
	engine->RegisterObjectBehaviour("Vector3", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Vector3>::Release), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f()", asFUNCTION(AngelScriptAPI<Vector3>::Factory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::FactoryWithArgs<const Vector3&>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f(float, float, float)", asFUNCTION((AngelScriptAPI<Vector3>::FactoryWithArgs<float, float, float>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f(float)", asFUNCTION(AngelScriptAPI<Vector3>::FactoryWithArgs<float>), asCALL_CDECL);
	engine->RegisterObjectMethod("Vector3", "void set_x(float)", asMETHODPR(Vector3, SetX, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "void set_y(float)", asMETHODPR(Vector3, SetY, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "void set_z(float)", asMETHODPR(Vector3, SetZ, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "float get_x() const", asMETHODPR(Vector3, GetX, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "float get_y() const", asMETHODPR(Vector3, GetY, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "float get_z() const", asMETHODPR(Vector3, GetZ, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "void opAssign(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::OpAssign<Vector3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "Vector3@ opNeg()", asFUNCTION(AngelScriptAPI<Vector3>::OpNeg), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "Vector3@ opAdd(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Vector3>::OpAdd<const Vector3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "Vector3@ opSub(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Vector3>::OpSub<const Vector3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "Vector3@ opMul(float) const", asFUNCTION(AngelScriptAPI<Vector3>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "Vector3@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Vector3>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "Vector3@ opDiv(float) const", asFUNCTION(AngelScriptAPI<Vector3>::OpDiv<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "void opAddAssign(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::OpAddAssign<const Vector3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "void subAssign(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::OpSubAssign<const Vector3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Vector3>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "void opDivAssign(float)", asFUNCTION(AngelScriptAPI<Vector3>::OpDivAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "float GetMaxElem() const", asMETHODPR(Vector3, GetMaxElem, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "float GetMinElem() const", asMETHODPR(Vector3, GetMinElem, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "float Dot(const Vector3& in) const", asMETHODPR(Vector3, Dot, (const Vector3&) const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "Vector3@ Cross(const Vector3& in) const", asFUNCTION(Vector3_Cross), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector3", "float LengthSqr() const", asMETHODPR(Vector3, LengthSqr, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "float Length() const", asMETHODPR(Vector3, Length, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector3", "void Normalize()", asMETHODPR(Vector3, Normalize, (), Vector3&), asCALL_THISCALL);
//	Vector3 MulPerElem(const Vector3& vec) const;
//	Vector3 DivPerElem(const Vector3& vec) const;
//	Vector3 AbsPerElem() const;
	engine->SetDefaultNamespace("Vector3");
	engine->RegisterGlobalFunction("Vector3@ MaxPerElem(const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Vector3>::StaticFunc<const Vector3&, const Vector3&, &Vector3::MaxPerElem>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector3@ MinPerElem(const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Vector3>::StaticFunc<const Vector3&, const Vector3&, &Vector3::MinPerElem>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector3@ Slerp(float, const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Vector3>::StaticFunc<float, const Vector3&, const Vector3&, &Vector3::Slerp>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector3@ AxisX()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::AxisX>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector3@ AxisY()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::AxisY>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector3@ AxisZ()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::AxisZ>), asCALL_GENERIC);
	engine->SetDefaultNamespace("");

	// Vector4
	engine->RegisterObjectType("Vector4", 0, asOBJ_REF | asOBJ_SCOPED);
	engine->RegisterObjectBehaviour("Vector4", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Vector4>::Release), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f()", asFUNCTION(AngelScriptAPI<Vector4>::Factory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::FactoryWithArgs<const Vector4&>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(float, float, float, float)", asFUNCTION((AngelScriptAPI<Vector4>::FactoryWithArgs<float, float, float, float>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(float)", asFUNCTION(AngelScriptAPI<Vector4>::FactoryWithArgs<float>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(const Vector3& in, float)", asFUNCTION((AngelScriptAPI<Vector4>::FactoryWithArgs<const Vector3&, float>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector4>::FactoryWithArgs<const Vector3&>), asCALL_CDECL);
	engine->RegisterObjectMethod("Vector4", "void SetXYZ(const Vector3& in)", asMETHODPR(Vector4, SetXYZ, (const Vector3&), void), asCALL_THISCALL);
//	Vector3 GetXYZ() const
	engine->RegisterObjectMethod("Vector4", "void set_x(float)", asMETHODPR(Vector4, SetX, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "void set_y(float)", asMETHODPR(Vector4, SetY, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "void set_z(float)", asMETHODPR(Vector4, SetZ, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "void set_w(float)", asMETHODPR(Vector4, SetW, (float), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float get_x() const", asMETHODPR(Vector4, GetX, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float get_y() const", asMETHODPR(Vector4, GetY, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float get_z() const", asMETHODPR(Vector4, GetZ, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float get_w() const", asMETHODPR(Vector4, GetW, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "void opAssign(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::OpAssign<Vector4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "Vector4@ opNeg()", asFUNCTION(AngelScriptAPI<Vector4>::OpNeg), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "Vector4@ opAdd(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Vector4>::OpAdd<const Vector4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "Vector4@ opSub(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Vector4>::OpSub<const Vector4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "Vector4@ opMul(float) const", asFUNCTION(AngelScriptAPI<Vector4>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "Vector4@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Vector4>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "Vector4@ opDiv(float) const", asFUNCTION(AngelScriptAPI<Vector4>::OpDiv<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "void opAddAssign(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::OpAddAssign<const Vector4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "void subAssign(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::OpSubAssign<const Vector4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Vector4>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "void opDivAssign(float)", asFUNCTION(AngelScriptAPI<Vector4>::OpDivAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vector4", "float GetMaxElem() const", asMETHODPR(Vector4, GetMaxElem, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float GetMinElem() const", asMETHODPR(Vector4, GetMinElem, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float Dot(const Vector4& in) const", asMETHODPR(Vector4, Dot, (const Vector4&) const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float LengthSqr() const", asMETHODPR(Vector4, LengthSqr, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "float Length() const", asMETHODPR(Vector4, Length, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vector4", "void Normalize()", asMETHODPR(Vector4, Normalize, (), Vector4&), asCALL_THISCALL);
//	Vector4 MulPerElem(const Vector4& vec) const
//	Vector4 DivPerElem(const Vector4& vec) const
//	Vector4 AbsPerElem() const
	engine->SetDefaultNamespace("Vector4");
	engine->RegisterGlobalFunction("Vector4@ MaxPerElem(const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptGenericAPI<Vector4>::StaticFunc<const Vector4&, const Vector4&, &Vector4::MaxPerElem>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector4@ MinPerElem(const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptGenericAPI<Vector4>::StaticFunc<const Vector4&, const Vector4&, &Vector4::MinPerElem>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector4@ Slerp(float, const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptGenericAPI<Vector4>::StaticFunc<float, const Vector4&, const Vector4&, &Vector4::Slerp>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector4@ AxisX()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisX>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector4@ AxisY()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisY>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector4@ AxisZ()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisZ>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Vector4@ AxisW()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisW>), asCALL_GENERIC);
	engine->SetDefaultNamespace("");

	// Quat
	engine->RegisterObjectType("Quat", 0, asOBJ_REF | asOBJ_SCOPED);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Quat>::Release), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f()", asFUNCTION(AngelScriptAPI<Quat>::Factory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::FactoryWithArgs<const Quat&>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f(float, float, float, float)", asFUNCTION((AngelScriptAPI<Quat>::FactoryWithArgs<float, float, float, float>)), asCALL_CDECL);
	engine->RegisterObjectMethod("Quat", "void opAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpAssign<Quat&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat@ opNeg()", asFUNCTION(AngelScriptAPI<Quat>::OpNeg), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat@ opAdd(const Quat& in) const", asFUNCTION(AngelScriptAPI<Quat>::OpAdd<const Quat&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat@ opSub(const Quat& in) const", asFUNCTION(AngelScriptAPI<Quat>::OpSub<const Quat&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat@ opMul(const Quat& in) const", asFUNCTION(AngelScriptAPI<Quat>::OpMul<const Quat&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat@ opMul(float) const", asFUNCTION(AngelScriptAPI<Quat>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Quat>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat@ opDiv(float) const", asFUNCTION(AngelScriptAPI<Quat>::OpDiv<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "void opAddAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpAddAssign<const Quat&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "void subAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpSubAssign<const Quat&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "void opMulAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpMulAssign<const Quat&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Quat>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "void opDivAssign(float)", asFUNCTION(AngelScriptAPI<Quat>::OpDivAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "float Dot(const Quat& in) const", asMETHODPR(Quat, Dot, (const Quat&) const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Quat", "float Norm()", asMETHODPR(Quat, Norm, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("Quat", "float Length() const", asMETHODPR(Quat, Length, () const, float), asCALL_THISCALL);
//	Quat Conjugate() const;
//	Vector3 RotateVec(const Vector3& vec) const;
	engine->RegisterObjectMethod("Quat", "void Normalize()", asMETHODPR(Quat, Normalize, (), Quat&), asCALL_THISCALL);
	engine->SetDefaultNamespace("Quat");
	engine->RegisterGlobalFunction("Quat@ Slerp(float, const Quat& in, const Quat& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, const Quat&, const Quat&, &Quat::Slerp>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ Squad(float, const Quat& in, const Quat& in, const Quat& in, const Quat& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, const Quat&, const Quat&, const Quat&, const Quat&, &Quat::Squad>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ Identity()", asFUNCTION(AngelScriptGenericAPI<Quat>::StaticFunc<&Quat::Identity>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ CreateRotation(float, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, const Vector3&, &Quat::CreateRotation>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ CreateRotationX(float)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, &Quat::CreateRotationX>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ CreateRotationY(float)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, &Quat::CreateRotationY>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ CreateRotationZ(float)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, &Quat::CreateRotationZ>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ CreateRotationFromEulerAngles(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<const Vector3&, &Quat::CreateRotationFromEulerAngles>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Quat@ CreateRotationFromTo(const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<const Vector3&, const Vector3&, &Quat::CreateRotationFromTo>)), asCALL_GENERIC);
	engine->SetDefaultNamespace("");

	// Matrix3
	engine->RegisterObjectType("Matrix3", 0, asOBJ_REF | asOBJ_SCOPED);
	engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Matrix3>::Release), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f()", asFUNCTION(AngelScriptAPI<Matrix3>::Factory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::FactoryWithArgs<const Matrix3&>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f(const Vector3& in, const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptAPI<Matrix3>::FactoryWithArgs<const Vector3&, const Vector3&, const Vector3&>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f(const Quat& in)", asFUNCTION(AngelScriptAPI<Matrix3>::FactoryWithArgs<const Quat&>), asCALL_CDECL);
	engine->RegisterObjectProperty("Matrix3", "Vector3& col0", asOFFSET(Matrix3, col0));
	engine->RegisterObjectProperty("Matrix3", "Vector3& col1", asOFFSET(Matrix3, col1));
	engine->RegisterObjectProperty("Matrix3", "Vector3& col2", asOFFSET(Matrix3, col2));
	engine->RegisterObjectMethod("Matrix3", "void SetCol(int, const Vector3& in)", asMETHODPR(Matrix3, SetCol, (int, const Vector3&), Matrix3&), asCALL_THISCALL);
//	const Vector3& GetCol(int col) const
	engine->RegisterObjectMethod("Matrix3", "void SetRow0(const Vector3& in)", asMETHODPR(Matrix3, SetRow0, (const Vector3&), void), asCALL_THISCALL);
//	Vector3 GetRow0() const
	engine->RegisterObjectMethod("Matrix3", "void opAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpAssign<Matrix3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "Matrix3@ opNeg()", asFUNCTION(AngelScriptAPI<Matrix3>::OpNeg), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "Matrix3@ opAdd(const Matrix3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpAdd<const Matrix3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "Matrix3@ opSub(const Matrix3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpSub<const Matrix3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "Matrix3@ opMul(float) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "Matrix3@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "Vector3@ opMul(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul_2<Vector3>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "Matrix3@ opMul(const Matrix3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul<const Matrix3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "void opAddAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpAddAssign<const Matrix3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "void subAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpSubAssign<const Matrix3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Matrix3>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "void opMulAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpMulAssign<const Matrix3&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix3", "void AppendScale(const Vector3& in)", asMETHODPR(Matrix3, AppendScale, (const Vector3&), Matrix3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix3", "void PrependScale(const Vector3& in)", asMETHODPR(Matrix3, PrependScale, (const Vector3&), Matrix3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix3", "void Transpose()", asMETHODPR(Matrix3, Transpose, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix3", "void Invert()", asMETHODPR(Matrix3, Invert, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix3", "float CalculateDeterminant() const", asMETHODPR(Matrix3, CalculateDeterminant, () const, float), asCALL_THISCALL);
	engine->SetDefaultNamespace("Matrix3");
	engine->RegisterGlobalFunction("Matrix3@ Identity()", asFUNCTION(AngelScriptGenericAPI<Matrix3>::StaticFunc<&Matrix3::Identity>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix3@ CreateRotationX(float)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, &Matrix3::CreateRotationX>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix3@ CreateRotationY(float)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, &Matrix3::CreateRotationY>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix3@ CreateRotationZ(float)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, &Matrix3::CreateRotationZ>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix3@ CreateRotationZYX(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<const Vector3&, &Matrix3::CreateRotationZYX>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix3@ CreateRotation(float, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, const Vector3&, &Matrix3::CreateRotation>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix3@ CreateRotation(const Quat& in)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<const Quat&, &Matrix3::CreateRotation>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix3@ CreateScale(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<const Vector3&, &Matrix3::CreateScale>)), asCALL_GENERIC);
	engine->SetDefaultNamespace("");

	engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f(const Matrix3& in)", asFUNCTION((AngelScriptAPI<Quat>::FactoryWithArgs<const Matrix3&>)), asCALL_CDECL);

	// Matrix4
	engine->RegisterObjectType("Matrix4", 0, asOBJ_REF | asOBJ_SCOPED);
	engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Matrix4>::Release), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f()", asFUNCTION(AngelScriptAPI<Matrix4>::Factory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::FactoryWithArgs<const Matrix4&>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Vector4& in, const Vector4& in, const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptAPI<Matrix4>::FactoryWithArgs<const Vector4&, const Vector4&, const Vector4&, const Vector4&>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Matrix3& in, const Vector3& in)", asFUNCTION((AngelScriptAPI<Matrix4>::FactoryWithArgs<const Matrix3&, const Vector3&>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Quat& in, const Vector3& in)", asFUNCTION((AngelScriptAPI<Matrix4>::FactoryWithArgs<const Quat&, const Vector3&>)), asCALL_CDECL);
	engine->RegisterObjectMethod("Matrix4", "void SetUpper3x3(const Matrix3& in)", asMETHODPR(Matrix4, SetUpper3x3, (const Matrix3&), Matrix4&), asCALL_THISCALL);
//	Matrix3 GetUpper3x3() const
	engine->RegisterObjectMethod("Matrix4", "void SetTranslation(const Vector3& in)", asMETHODPR(Matrix4, SetTranslation, (const Vector3&), Matrix4&), asCALL_THISCALL);
//	Vector3 GetTranslation() const
	engine->RegisterObjectProperty("Matrix4", "Vector4& col0", asOFFSET(Matrix4, col0));
	engine->RegisterObjectProperty("Matrix4", "Vector4& col1", asOFFSET(Matrix4, col1));
	engine->RegisterObjectProperty("Matrix4", "Vector4& col2", asOFFSET(Matrix4, col2));
	engine->RegisterObjectProperty("Matrix4", "Vector4& col3", asOFFSET(Matrix4, col3));
	engine->RegisterObjectMethod("Matrix4", "void SetCol(int, const Vector4& in)", asMETHODPR(Matrix4, SetCol, (int, const Vector4&), Matrix4&), asCALL_THISCALL);
//	const Vector4& GetCol(int col) const
	engine->RegisterObjectMethod("Matrix4", "void SetRow0(const Vector4& in)", asMETHODPR(Matrix4, SetRow0, (const Vector4&), void), asCALL_THISCALL);
//	Vector4 GetRow0() const
	engine->RegisterObjectMethod("Matrix4", "void opAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpAssign<Matrix4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Matrix4@ opNeg()", asFUNCTION(AngelScriptAPI<Matrix4>::OpNeg), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Matrix4@ opAdd(const Matrix4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpAdd<const Matrix4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Matrix4@ opSub(const Matrix4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpSub<const Matrix4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Matrix4@ opMul(float) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Matrix4@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Vector4@ opMul(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul_2<Vector4>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Vector4@ opMul(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul_2<Vector3>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "Matrix4@ opMul(const Matrix4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul<const Matrix4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "void opAddAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpAddAssign<const Matrix4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "void subAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpSubAssign<const Matrix4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Matrix4>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "void opMulAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpMulAssign<const Matrix4&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Matrix4", "void AppendScale(const Vector3& in)", asMETHODPR(Matrix4, AppendScale, (const Vector3&), Matrix4&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix4", "void PrependScale(const Vector3& in)", asMETHODPR(Matrix4, PrependScale, (const Vector3&), Matrix4&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix4", "void Transpose()", asMETHODPR(Matrix4, Transpose, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix4", "void Invert()", asMETHODPR(Matrix4, Invert, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix4", "void AffineInvert()", asMETHODPR(Matrix4, AffineInvert, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix4", "void OrthoInvert()", asMETHODPR(Matrix4, OrthoInvert, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Matrix4", "float CalculateDeterminant() const", asMETHODPR(Matrix4, CalculateDeterminant, () const, float), asCALL_THISCALL);
	engine->SetDefaultNamespace("Matrix4");
	engine->RegisterGlobalFunction("Matrix4@ Identity()", asFUNCTION(AngelScriptGenericAPI<Matrix4>::StaticFunc<&Matrix4::Identity>), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateTranslation(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<const Vector3&, &Matrix4::CreateTranslation>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateRotationX(float)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, &Matrix4::CreateRotationX>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateRotationY(float)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, &Matrix4::CreateRotationY>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateRotationZ(float)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, &Matrix4::CreateRotationZ>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateRotationZYX(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<const Vector3&, &Matrix4::CreateRotationZYX>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateRotation(float, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, const Vector3&, &Matrix4::CreateRotation>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateRotation(const Quat& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<const Quat&, &Matrix4::CreateRotation>)), asCALL_GENERIC);
	engine->RegisterGlobalFunction("Matrix4@ CreateScale(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<const Vector3&, &Matrix4::CreateScale>)), asCALL_GENERIC);
	engine->SetDefaultNamespace("");
}

void RegisterMathFunctions_AngelScript(asIScriptEngine *engine)
{
	// Trigonometric functions
	engine->RegisterGlobalFunction("float cos(float)", asFUNCTION(std::cosf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float sin(float)", asFUNCTION(std::sinf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float tan(float)", asFUNCTION(std::tanf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float acos(float)", asFUNCTION(std::acosf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float asin(float)", asFUNCTION(std::asinf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float atan(float)", asFUNCTION(std::atanf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float atan2(float, float)", asFUNCTION(std::atan2f), asCALL_CDECL);
	// Hyberbolic functions
	engine->RegisterGlobalFunction("float cosh(float)", asFUNCTION(std::coshf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float sinh(float)", asFUNCTION(std::sinhf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float tanh(float)", asFUNCTION(std::tanhf), asCALL_CDECL);
	// Exponential and logarithmic functions
	engine->RegisterGlobalFunction("float log(float)", asFUNCTION(std::logf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float log10(float)", asFUNCTION(std::log10f), asCALL_CDECL);
	// Power functions
	engine->RegisterGlobalFunction("float pow(float, float)", asFUNCTION(std::powf), asCALL_CDECL);
	engine->RegisterGlobalFunction("float sqrt(float)", asFUNCTION(std::sqrtf), asCALL_CDECL);
	// Absolute value functions
	engine->RegisterGlobalFunction("float fabsf(float)", asFUNCTION(std::fabsf), asCALL_CDECL);
	engine->RegisterGlobalFunction("int64 abs(int64)", asFUNCTION(std::llabs), asCALL_CDECL);

	// Math
	engine->SetDefaultNamespace("Math");
	engine->RegisterGlobalFunction("float RandFloat(float, float)", asFUNCTION(Math::RandFloat), asCALL_CDECL);
	engine->RegisterGlobalFunction("int RandInt(int, int)", asFUNCTION(Math::RandInt), asCALL_CDECL);
	engine->RegisterGlobalFunction("int Round32(float)", asFUNCTION(Math::Round32), asCALL_CDECL);
	engine->RegisterGlobalFunction("int RoundUp(float, int)", asFUNCTION(Math::RoundUp), asCALL_CDECL);
	engine->RegisterGlobalFunction("float Clamp(float, float, float)", asFUNCTION(Math::Clamp), asCALL_CDECL);
	engine->RegisterGlobalFunction("float Clamp01(float)", asFUNCTION(Math::Clamp01), asCALL_CDECL);
	engine->SetDefaultNamespace("");
}

void RegisterCoreAPI_AngelScript(asIScriptEngine *engine)
{
	RegisterString_AngelScript(engine);
	RegisterVectormathFunctions_AngelScript(engine);
	RegisterMathFunctions_AngelScript(engine);

	// Timer
	engine->RegisterObjectType("ITimer", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterGlobalProperty("ITimer Timer", Timer::Get());
	engine->RegisterObjectMethod("ITimer", "uint64 GetTimeMicroSec() const", asMETHODPR(Timer, GetTimeMicroSec, () const, uint64_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("ITimer", "uint GetTimeMilliSec() const", asMETHODPR(Timer, GetTimeMilliSec, () const, uint32_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("ITimer", "float GetTimeSec() const", asMETHODPR(Timer, GetTimeSec, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("ITimer", "uint64 GetMicroDelta() const", asMETHODPR(Timer, GetMicroDelta, () const, uint64_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("ITimer", "uint GetMilliDelta() const", asMETHODPR(Timer, GetMilliDelta, () const, uint32_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("ITimer", "float GetSecDelta() const", asMETHODPR(Timer, GetSecDelta, () const, float), asCALL_THISCALL);
}
