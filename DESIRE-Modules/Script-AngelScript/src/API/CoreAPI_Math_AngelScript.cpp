#include "API/AngelScriptAPI.h"

#include "Engine/Core/Math/Matrix4.h"
#include "Engine/Core/Math/math.h"
#include "Engine/Core/Math/Rand.h"
#include "Engine/Core/Math/Transform.h"

static Vector3* Vector3_Cross(const Vector3& vec0, const Vector3& vec1)	{ return new Vector3(vec0.Cross(vec1)); }
static Vector3* Transform_GetPosition(const Transform& transform)		{ return new Vector3(transform.GetPosition()); }
static Quat* Transform_GetRotation(const Transform& transform)			{ return new Quat(transform.GetRotation()); }
static Vector3* Transform_GetScale(const Transform& transform)			{ return new Vector3(transform.GetScale()); }

void RegisterCoreAPI_Math_AngelScript(asIScriptEngine* engine)
{
	int result = asSUCCESS;

	// Vector3
	result = engine->RegisterObjectType("Vector3", 0, asOBJ_REF | asOBJ_SCOPED);																																								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Vector3>::Release), asCALL_CDECL_OBJFIRST);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f()", asFUNCTION(AngelScriptAPI<Vector3>::Factory), asCALL_CDECL);																							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::FactoryWithArgs<const Vector3&>), asCALL_CDECL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f(float, float, float)", asFUNCTION((AngelScriptAPI<Vector3>::FactoryWithArgs<float, float, float>)), asCALL_CDECL);										ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_FACTORY, "Vector3@ f(float)", asFUNCTION(AngelScriptAPI<Vector3>::FactoryWithArgs<float>), asCALL_CDECL);																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void set_x(float) property", asMETHOD(Vector3, SetX), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void set_y(float) property", asMETHOD(Vector3, SetY), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void set_z(float) property", asMETHOD(Vector3, SetZ), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float get_x() const property", asMETHOD(Vector3, GetX), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float get_y() const property", asMETHOD(Vector3, GetY), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float get_z() const property", asMETHOD(Vector3, GetZ), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void opAssign(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::OpAssign<Vector3&>), asCALL_CDECL_OBJFIRST);																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opNeg()", asFUNCTION(AngelScriptAPI<Vector3>::OpNeg), asCALL_CDECL_OBJFIRST);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opAdd(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Vector3>::OpAdd<const Vector3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opSub(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Vector3>::OpSub<const Vector3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opMul(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Vector3>::OpMul<const Vector3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opMul(float) const", asFUNCTION(AngelScriptAPI<Vector3>::OpMul<float>), asCALL_CDECL_OBJFIRST);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Vector3>::OpMul<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opDiv(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Vector3>::OpDiv<const Vector3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ opDiv(float) const", asFUNCTION(AngelScriptAPI<Vector3>::OpDiv<float>), asCALL_CDECL_OBJFIRST);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void opAddAssign(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::OpAddAssign<const Vector3&>), asCALL_CDECL_OBJFIRST);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void subAssign(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector3>::OpSubAssign<const Vector3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Vector3>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void opDivAssign(float)", asFUNCTION(AngelScriptAPI<Vector3>::OpDivAssign<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float Dot(const Vector3& in) const", asMETHOD(Vector3, Dot), asCALL_THISCALL);																											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "Vector3@ Cross(const Vector3& in) const", asFUNCTION(Vector3_Cross), asCALL_CDECL_OBJFIRST);																								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float LengthSqr() const", asMETHOD(Vector3, LengthSqr), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float Length() const", asMETHOD(Vector3, Length), asCALL_THISCALL);																														ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "void Normalize()", asMETHOD(Vector3, Normalize), asCALL_THISCALL);																														ASSERT(result >= asSUCCESS);
//	result = engine->RegisterObjectMethod("Vector3", "Vector3@ Normalized() const", asMETHOD(Vector3, Normalized), asCALL_THISCALL);																											ASSERT(result >= asSUCCESS);
//	Vector3 AbsPerElem() const;																																																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float GetMaxElem() const", asMETHOD(Vector3, GetMaxElem), asCALL_THISCALL);																												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector3", "float GetMinElem() const", asMETHOD(Vector3, GetMinElem), asCALL_THISCALL);																												ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("Vector3");																																															ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ MaxPerElem(const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Vector3>::StaticFunc<const Vector3&, const Vector3&, &Vector3::MaxPerElem>)), asCALL_GENERIC);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ MinPerElem(const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Vector3>::StaticFunc<const Vector3&, const Vector3&, &Vector3::MinPerElem>)), asCALL_GENERIC);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ Slerp(float, const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Vector3>::StaticFunc<float, const Vector3&, const Vector3&, &Vector3::Slerp>)), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ Zero()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::Zero>), asCALL_GENERIC);																							ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ One()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::One>), asCALL_GENERIC);																							ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ AxisX()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::AxisX>), asCALL_GENERIC);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ AxisY()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::AxisY>), asCALL_GENERIC);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector3@ AxisZ()", asFUNCTION(AngelScriptGenericAPI<Vector3>::StaticFunc<&Vector3::AxisZ>), asCALL_GENERIC);																						ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("");																																																	ASSERT(result >= asSUCCESS);

	// Vector4
	result = engine->RegisterObjectType("Vector4", 0, asOBJ_REF | asOBJ_SCOPED);																																								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Vector4>::Release), asCALL_CDECL_OBJFIRST);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f()", asFUNCTION(AngelScriptAPI<Vector4>::Factory), asCALL_CDECL);																							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::FactoryWithArgs<const Vector4&>), asCALL_CDECL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(float, float, float, float)", asFUNCTION((AngelScriptAPI<Vector4>::FactoryWithArgs<float, float, float, float>)), asCALL_CDECL);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(float)", asFUNCTION(AngelScriptAPI<Vector4>::FactoryWithArgs<float>), asCALL_CDECL);																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(const Vector3& in, float)", asFUNCTION((AngelScriptAPI<Vector4>::FactoryWithArgs<const Vector3&, float>)), asCALL_CDECL);									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_FACTORY, "Vector4@ f(const Vector3& in)", asFUNCTION(AngelScriptAPI<Vector4>::FactoryWithArgs<const Vector3&>), asCALL_CDECL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void SetXYZ(const Vector3& in)", asMETHOD(Vector4, SetXYZ), asCALL_THISCALL);																												ASSERT(result >= asSUCCESS);
//	Vector3 GetXYZ() const																																																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void set_x(float) property", asMETHOD(Vector4, SetX), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void set_y(float) property", asMETHOD(Vector4, SetY), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void set_z(float) property", asMETHOD(Vector4, SetZ), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void set_w(float) property", asMETHOD(Vector4, SetW), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float get_x() const property", asMETHOD(Vector4, GetX), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float get_y() const property", asMETHOD(Vector4, GetY), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float get_z() const property", asMETHOD(Vector4, GetZ), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float get_w() const property", asMETHOD(Vector4, GetW), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void opAssign(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::OpAssign<Vector4&>), asCALL_CDECL_OBJFIRST);																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opNeg()", asFUNCTION(AngelScriptAPI<Vector4>::OpNeg), asCALL_CDECL_OBJFIRST);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opAdd(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Vector4>::OpAdd<const Vector4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opSub(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Vector4>::OpSub<const Vector4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opMul(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Vector4>::OpMul<const Vector4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opMul(float) const", asFUNCTION(AngelScriptAPI<Vector4>::OpMul<float>), asCALL_CDECL_OBJFIRST);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Vector4>::OpMul<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opDiv(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Vector4>::OpDiv<const Vector4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "Vector4@ opDiv(float) const", asFUNCTION(AngelScriptAPI<Vector4>::OpDiv<float>), asCALL_CDECL_OBJFIRST);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void opAddAssign(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::OpAddAssign<const Vector4&>), asCALL_CDECL_OBJFIRST);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void subAssign(const Vector4& in)", asFUNCTION(AngelScriptAPI<Vector4>::OpSubAssign<const Vector4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Vector4>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void opDivAssign(float)", asFUNCTION(AngelScriptAPI<Vector4>::OpDivAssign<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float GetMaxElem() const", asMETHOD(Vector4, GetMaxElem), asCALL_THISCALL);																												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float GetMinElem() const", asMETHOD(Vector4, GetMinElem), asCALL_THISCALL);																												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float Dot(const Vector4& in) const", asMETHOD(Vector4, Dot), asCALL_THISCALL);																											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float LengthSqr() const", asMETHOD(Vector4, LengthSqr), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "float Length() const", asMETHOD(Vector4, Length), asCALL_THISCALL);																														ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Vector4", "void Normalize()", asMETHOD(Vector4, Normalize), asCALL_THISCALL);																														ASSERT(result >= asSUCCESS);
//	result = engine->RegisterObjectMethod("Vector4", "Vector4@ Normalized() const", asMETHOD(Vector4, Normalized), asCALL_THISCALL);																											ASSERT(result >= asSUCCESS);
//	Vector4 AbsPerElem() const																																																					ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("Vector4");																																															ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector4@ MaxPerElem(const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptGenericAPI<Vector4>::StaticFunc<const Vector4&, const Vector4&, &Vector4::MaxPerElem>)), asCALL_GENERIC);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector4@ MinPerElem(const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptGenericAPI<Vector4>::StaticFunc<const Vector4&, const Vector4&, &Vector4::MinPerElem>)), asCALL_GENERIC);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector4@ Slerp(float, const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptGenericAPI<Vector4>::StaticFunc<float, const Vector4&, const Vector4&, &Vector4::Slerp>)), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector4@ AxisX()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisX>), asCALL_GENERIC);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector4@ AxisY()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisY>), asCALL_GENERIC);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector4@ AxisZ()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisZ>), asCALL_GENERIC);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Vector4@ AxisW()", asFUNCTION(AngelScriptGenericAPI<Vector4>::StaticFunc<&Vector4::AxisW>), asCALL_GENERIC);																						ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("");																																																	ASSERT(result >= asSUCCESS);

	// Quat
	result = engine->RegisterObjectType("Quat", 0, asOBJ_REF | asOBJ_SCOPED);																																																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Quat", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Quat>::Release), asCALL_CDECL_OBJFIRST);																																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f()", asFUNCTION(AngelScriptAPI<Quat>::Factory), asCALL_CDECL);																																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::FactoryWithArgs<const Quat&>), asCALL_CDECL);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f(float, float, float, float)", asFUNCTION((AngelScriptAPI<Quat>::FactoryWithArgs<float, float, float, float>)), asCALL_CDECL);																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "void opAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpAssign<Quat&>), asCALL_CDECL_OBJFIRST);																														ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "Quat@ opNeg()", asFUNCTION(AngelScriptAPI<Quat>::OpNeg), asCALL_CDECL_OBJFIRST);																																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "Quat@ opAdd(const Quat& in) const", asFUNCTION(AngelScriptAPI<Quat>::OpAdd<const Quat&>), asCALL_CDECL_OBJFIRST);																												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "Quat@ opSub(const Quat& in) const", asFUNCTION(AngelScriptAPI<Quat>::OpSub<const Quat&>), asCALL_CDECL_OBJFIRST);																												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "Quat@ opMul(const Quat& in) const", asFUNCTION(AngelScriptAPI<Quat>::OpMul<const Quat&>), asCALL_CDECL_OBJFIRST);																												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "void opAddAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpAddAssign<const Quat&>), asCALL_CDECL_OBJFIRST);																											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "void subAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpSubAssign<const Quat&>), asCALL_CDECL_OBJFIRST);																												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "void opMulAssign(const Quat& in)", asFUNCTION(AngelScriptAPI<Quat>::OpMulAssign<const Quat&>), asCALL_CDECL_OBJFIRST);																											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "float Dot(const Quat& in) const", asMETHOD(Quat, Dot), asCALL_THISCALL);																																							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "float Norm()", asMETHOD(Quat, Norm), asCALL_THISCALL);																																											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "float Length() const", asMETHOD(Quat, Length), asCALL_THISCALL);																																									ASSERT(result >= asSUCCESS);
//	Quat Conjugate() const;																																																															ASSERT(result >= asSUCCESS);
//	Vector3 EulerAngles() const;																																																													ASSERT(result >= asSUCCESS);
//	Vector3 RotateVec(const Vector3& vec) const;																																																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Quat", "void Normalize()", asMETHOD(Quat, Normalize), asCALL_THISCALL);																																									ASSERT(result >= asSUCCESS);
//	result = engine->RegisterObjectMethod("Quat", "Quat@ Normalized() const", asMETHOD(Quat, Normalized), asCALL_THISCALL);																																							ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("Quat");																																																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ Slerp(float, const Quat& in, const Quat& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, const Quat&, const Quat&, &Quat::Slerp>)), asCALL_GENERIC);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ Squad(float, const Quat& in, const Quat& in, const Quat& in, const Quat& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, const Quat&, const Quat&, const Quat&, const Quat&, &Quat::Squad>)), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ Identity()", asFUNCTION(AngelScriptGenericAPI<Quat>::StaticFunc<&Quat::Identity>), asCALL_GENERIC);																																ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ CreateRotation(float, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, const Vector3&, &Quat::CreateRotation>)), asCALL_GENERIC);																	ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ CreateRotationX(float)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, &Quat::CreateRotationX>)), asCALL_GENERIC);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ CreateRotationY(float)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, &Quat::CreateRotationY>)), asCALL_GENERIC);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ CreateRotationZ(float)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<float, &Quat::CreateRotationZ>)), asCALL_GENERIC);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ CreateRotationFromEulerAngles(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<const Vector3&, &Quat::CreateRotationFromEulerAngles>)), asCALL_GENERIC);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Quat@ CreateRotationFromTo(const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Quat>::StaticFunc<const Vector3&, const Vector3&, &Quat::CreateRotationFromTo>)), asCALL_GENERIC);								ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("");																																																										ASSERT(result >= asSUCCESS);

	// Matrix3
	result = engine->RegisterObjectType("Matrix3", 0, asOBJ_REF | asOBJ_SCOPED);																																								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Matrix3>::Release), asCALL_CDECL_OBJFIRST);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f()", asFUNCTION(AngelScriptAPI<Matrix3>::Factory), asCALL_CDECL);																							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::FactoryWithArgs<const Matrix3&>), asCALL_CDECL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f(const Vector3& in, const Vector3& in, const Vector3& in)", asFUNCTION((AngelScriptAPI<Matrix3>::FactoryWithArgs<const Vector3&, const Vector3&, const Vector3&>)), asCALL_CDECL);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix3", asBEHAVE_FACTORY, "Matrix3@ f(const Quat& in)", asFUNCTION(AngelScriptAPI<Matrix3>::FactoryWithArgs<const Quat&>), asCALL_CDECL);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectProperty("Matrix3", "Vector3& col0", asOFFSET(Matrix3, col0));																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectProperty("Matrix3", "Vector3& col1", asOFFSET(Matrix3, col1));																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectProperty("Matrix3", "Vector3& col2", asOFFSET(Matrix3, col2));																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void SetCol(int, const Vector3& in)", asMETHOD(Matrix3, SetCol), asCALL_THISCALL);																										ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Vector3& GetCol(int) const", asMETHOD(Matrix3, GetCol), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void SetRow0(const Vector3& in)", asMETHOD(Matrix3, SetRow0), asCALL_THISCALL);																											ASSERT(result >= asSUCCESS);
//	Vector3 GetRow0() const																																																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void opAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpAssign<Matrix3&>), asCALL_CDECL_OBJFIRST);																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Matrix3@ opNeg()", asFUNCTION(AngelScriptAPI<Matrix3>::OpNeg), asCALL_CDECL_OBJFIRST);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Matrix3@ opAdd(const Matrix3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpAdd<const Matrix3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Matrix3@ opSub(const Matrix3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpSub<const Matrix3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Matrix3@ opMul(float) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul<float>), asCALL_CDECL_OBJFIRST);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Matrix3@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Vector3@ opMul(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul_2<Vector3>), asCALL_CDECL_OBJFIRST);																	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "Matrix3@ opMul(const Matrix3& in) const", asFUNCTION(AngelScriptAPI<Matrix3>::OpMul<const Matrix3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void opAddAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpAddAssign<const Matrix3&>), asCALL_CDECL_OBJFIRST);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void subAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpSubAssign<const Matrix3&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Matrix3>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void opMulAssign(const Matrix3& in)", asFUNCTION(AngelScriptAPI<Matrix3>::OpMulAssign<const Matrix3&>), asCALL_CDECL_OBJFIRST);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void AppendScale(const Vector3& in)", asMETHOD(Matrix3, AppendScale), asCALL_THISCALL);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void PrependScale(const Vector3& in)", asMETHOD(Matrix3, PrependScale), asCALL_THISCALL);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void Transpose()", asMETHOD(Matrix3, Transpose), asCALL_THISCALL);																														ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "void Invert()", asMETHOD(Matrix3, Invert), asCALL_THISCALL);																																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix3", "float CalculateDeterminant() const", asMETHOD(Matrix3, CalculateDeterminant), asCALL_THISCALL);																							ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("Matrix3");																																															ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix3@ Identity()", asFUNCTION(AngelScriptGenericAPI<Matrix3>::StaticFunc<&Matrix3::Identity>), asCALL_GENERIC);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix3@ CreateRotationX(float)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, &Matrix3::CreateRotationX>)), asCALL_GENERIC);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix3@ CreateRotationY(float)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, &Matrix3::CreateRotationY>)), asCALL_GENERIC);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix3@ CreateRotationZ(float)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, &Matrix3::CreateRotationZ>)), asCALL_GENERIC);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix3@ CreateRotationZYX(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<const Vector3&, &Matrix3::CreateRotationZYX>)), asCALL_GENERIC);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix3@ CreateRotation(float, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<float, const Vector3&, &Matrix3::CreateRotation>)), asCALL_GENERIC);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix3@ CreateScale(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix3>::StaticFunc<const Vector3&, &Matrix3::CreateScale>)), asCALL_GENERIC);										ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("");																																																	ASSERT(result >= asSUCCESS);

	result = engine->RegisterObjectBehaviour("Quat", asBEHAVE_FACTORY, "Quat@ f(const Matrix3& in)", asFUNCTION((AngelScriptAPI<Quat>::FactoryWithArgs<const Matrix3&>)), asCALL_CDECL);	ASSERT(result >= asSUCCESS);

	// Matrix4
	result = engine->RegisterObjectType("Matrix4", 0, asOBJ_REF | asOBJ_SCOPED);																																								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_RELEASE, "void f()", asFUNCTION(AngelScriptAPI<Matrix4>::Release), asCALL_CDECL_OBJFIRST);																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f()", asFUNCTION(AngelScriptAPI<Matrix4>::Factory), asCALL_CDECL);																							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::FactoryWithArgs<const Matrix4&>), asCALL_CDECL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Vector4& in, const Vector4& in, const Vector4& in, const Vector4& in)", asFUNCTION((AngelScriptAPI<Matrix4>::FactoryWithArgs<const Vector4&, const Vector4&, const Vector4&, const Vector4&>)), asCALL_CDECL);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Matrix3& in, const Vector3& in)", asFUNCTION((AngelScriptAPI<Matrix4>::FactoryWithArgs<const Matrix3&, const Vector3&>)), asCALL_CDECL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("Matrix4", asBEHAVE_FACTORY, "Matrix4@ f(const Quat& in, const Vector3& in)", asFUNCTION((AngelScriptAPI<Matrix4>::FactoryWithArgs<const Quat&, const Vector3&>)), asCALL_CDECL);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void SetUpper3x3(const Matrix3& in)", asMETHOD(Matrix4, SetUpper3x3), asCALL_THISCALL);																									ASSERT(result >= asSUCCESS);
//	Matrix3 GetUpper3x3() const																																																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void SetTranslation(const Vector3& in)", asMETHOD(Matrix4, SetTranslation), asCALL_THISCALL);																								ASSERT(result >= asSUCCESS);
//	Vector3 GetTranslation() const																																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectProperty("Matrix4", "Vector4& col0", asOFFSET(Matrix4, col0));																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectProperty("Matrix4", "Vector4& col1", asOFFSET(Matrix4, col1));																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectProperty("Matrix4", "Vector4& col2", asOFFSET(Matrix4, col2));																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectProperty("Matrix4", "Vector4& col3", asOFFSET(Matrix4, col3));																																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void SetCol(int, const Vector4& in)", asMETHOD(Matrix4, SetCol), asCALL_THISCALL);																										ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Vector4& GetCol(int) const", asMETHOD(Matrix4, GetCol), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void SetRow0(const Vector4& in)", asMETHOD(Matrix4, SetRow0), asCALL_THISCALL);																											ASSERT(result >= asSUCCESS);
//	Vector4 GetRow0() const																																																						ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void opAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpAssign<Matrix4&>), asCALL_CDECL_OBJFIRST);																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Matrix4@ opNeg()", asFUNCTION(AngelScriptAPI<Matrix4>::OpNeg), asCALL_CDECL_OBJFIRST);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Matrix4@ opAdd(const Matrix4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpAdd<const Matrix4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Matrix4@ opSub(const Matrix4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpSub<const Matrix4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Matrix4@ opMul(float) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul<float>), asCALL_CDECL_OBJFIRST);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Matrix4@ opMul_r(float) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Vector4@ opMul(const Vector4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul_2<Vector4>), asCALL_CDECL_OBJFIRST);																	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Vector4@ opMul(const Vector3& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul_2<Vector3>), asCALL_CDECL_OBJFIRST);																	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "Matrix4@ opMul(const Matrix4& in) const", asFUNCTION(AngelScriptAPI<Matrix4>::OpMul<const Matrix4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void opAddAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpAddAssign<const Matrix4&>), asCALL_CDECL_OBJFIRST);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void subAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpSubAssign<const Matrix4&>), asCALL_CDECL_OBJFIRST);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void opMulAssign(float)", asFUNCTION(AngelScriptAPI<Matrix4>::OpMulAssign<float>), asCALL_CDECL_OBJFIRST);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void opMulAssign(const Matrix4& in)", asFUNCTION(AngelScriptAPI<Matrix4>::OpMulAssign<const Matrix4&>), asCALL_CDECL_OBJFIRST);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void AppendScale(const Vector3& in)", asMETHOD(Matrix4, AppendScale), asCALL_THISCALL);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void PrependScale(const Vector3& in)", asMETHOD(Matrix4, PrependScale), asCALL_THISCALL);																									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void Transpose()", asMETHOD(Matrix4, Transpose), asCALL_THISCALL);																														ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void Invert()", asMETHOD(Matrix4, Invert), asCALL_THISCALL);																																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void AffineInvert()", asMETHOD(Matrix4, AffineInvert), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "void OrthoInvert()", asMETHOD(Matrix4, OrthoInvert), asCALL_THISCALL);																													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Matrix4", "float CalculateDeterminant() const", asMETHOD(Matrix4, CalculateDeterminant), asCALL_THISCALL);																							ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("Matrix4");																																															ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ Identity()", asFUNCTION(AngelScriptGenericAPI<Matrix4>::StaticFunc<&Matrix4::Identity>), asCALL_GENERIC);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ CreateTranslation(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<const Vector3&, &Matrix4::CreateTranslation>)), asCALL_GENERIC);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ CreateRotationX(float)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, &Matrix4::CreateRotationX>)), asCALL_GENERIC);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ CreateRotationY(float)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, &Matrix4::CreateRotationY>)), asCALL_GENERIC);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ CreateRotationZ(float)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, &Matrix4::CreateRotationZ>)), asCALL_GENERIC);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ CreateRotationZYX(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<const Vector3&, &Matrix4::CreateRotationZYX>)), asCALL_GENERIC);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ CreateRotation(float, const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<float, const Vector3&, &Matrix4::CreateRotation>)), asCALL_GENERIC);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("Matrix4@ CreateScale(const Vector3& in)", asFUNCTION((AngelScriptGenericAPI<Matrix4>::StaticFunc<const Vector3&, &Matrix4::CreateScale>)), asCALL_GENERIC);										ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("");																																																	ASSERT(result >= asSUCCESS);

	// Transform
	result = engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOHANDLE);																					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "void set_localPosition(const Vector3& in) property", asMETHOD(Transform, SetLocalPosition), asCALL_THISCALL);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "const Vector3& get_localPosition() const property", asMETHOD(Transform, GetLocalPosition), asCALL_THISCALL);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "void set_localRotation(const Quat& in) property", asMETHOD(Transform, SetLocalRotation), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "const Quat& get_localRotation() const property", asMETHOD(Transform, GetLocalRotation), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "void set_localScale(const Vector3& in) property", asMETHOD(Transform, SetLocalScale), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "const Vector3& get_localScale() const property", asMETHOD(Transform, GetLocalScale), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "void set_position(const Vector3& in) property", asMETHOD(Transform, SetPosition), asCALL_THISCALL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "Vector3@ get_position() const property", asFUNCTION(Transform_GetPosition), asCALL_CDECL_OBJFIRST);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "void set_rotation(const Quat& in) property", asMETHOD(Transform, SetRotation), asCALL_THISCALL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "Quat& get_rotation() const property", asFUNCTION(Transform_GetRotation), asCALL_CDECL_OBJFIRST);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "void set_scale(const Vector3& in) property", asMETHOD(Transform, SetScale), asCALL_THISCALL);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Transform", "Vector3& get_scale() const property", asFUNCTION(Transform_GetScale), asCALL_CDECL_OBJFIRST);					ASSERT(result >= asSUCCESS);

	// Rand
	result = engine->RegisterObjectType("Rand", 0, asOBJ_REF | asOBJ_NOHANDLE);											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Rand", "int GetInt()", asMETHOD(Rand, GetInt), asCALL_THISCALL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Rand", "uint GetUint()", asMETHOD(Rand, GetUint), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Rand", "float GetFloat()", asMETHOD(Rand, GetFloat), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Rand", "double GetDouble()", asMETHOD(Rand, GetDouble), asCALL_THISCALL);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("Rand", "bool GetBool()", asMETHOD(Rand, GetBool), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalProperty("Rand s_globalRand", &Rand::s_globalRand);									ASSERT(result >= asSUCCESS);

	// Math
	result = engine->SetDefaultNamespace("Math");																		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("int Round32(float)", asFUNCTION(Math::Round32), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("int RoundUp(float, int)", asFUNCTION(Math::RoundUp), asCALL_CDECL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float Clamp(float, float, float)", asFUNCTION(Math::Clamp), asCALL_CDECL);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float Clamp01(float)", asFUNCTION(Math::Clamp01), asCALL_CDECL);			ASSERT(result >= asSUCCESS);
	result = engine->SetDefaultNamespace("");																			ASSERT(result >= asSUCCESS);

	// Trigonometric functions
	result = engine->RegisterGlobalFunction("float cos(float)", asFUNCTION(std::cosf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float sin(float)", asFUNCTION(std::sinf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float tan(float)", asFUNCTION(std::tanf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float acos(float)", asFUNCTION(std::acosf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float asin(float)", asFUNCTION(std::asinf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float atan(float)", asFUNCTION(std::atanf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float atan2(float, float)", asFUNCTION(std::atan2f), asCALL_CDECL);	ASSERT(result >= asSUCCESS);
	// Hyberbolic functions
	result = engine->RegisterGlobalFunction("float cosh(float)", asFUNCTION(std::coshf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float sinh(float)", asFUNCTION(std::sinhf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float tanh(float)", asFUNCTION(std::tanhf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	// Exponential and logarithmic functions
	result = engine->RegisterGlobalFunction("float log(float)", asFUNCTION(std::logf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float log10(float)", asFUNCTION(std::log10f), asCALL_CDECL);			ASSERT(result >= asSUCCESS);
	// Power functions
	result = engine->RegisterGlobalFunction("float pow(float, float)", asFUNCTION(std::powf), asCALL_CDECL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("float sqrt(float)", asFUNCTION(std::sqrtf), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
	// Absolute value functions
	result = engine->RegisterGlobalFunction("float fabsf(float)", asFUNCTION(std::fabsf), asCALL_CDECL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalFunction("int64 abs(int64)", asFUNCTION(std::llabs), asCALL_CDECL);				ASSERT(result >= asSUCCESS);
}
