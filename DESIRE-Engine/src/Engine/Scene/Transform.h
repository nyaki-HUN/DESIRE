#pragma once

#include "Engine/Core/math/Matrix4.h"

class Object;

class Transform
{
public:
	Transform();

	inline const Vector3& GetLocalPosition() const	{ return localPosition; }
	inline const Quat& GetLocalRotation() const		{ return localRotation; }
	inline const Vector3& GetLocalScale() const		{ return localScale; }

	void SetLocalPosition(const Vector3& position);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalScale(const Vector3& scale);

	Matrix4 ConstructLocalMatrix() const;
	void ResetToIdentity();

	// Returns the absolute position
	Vector3 GetPosition() const;
	// Sets the the absolute position
	void SetPosition(const Vector3& position);

	// Returns the absolute rotation
	Quat GetRotation() const;
	// Sets the the absolute rotation
	void SetRotation(const Quat& rotation);

	// Returns the absolute scale
	Vector3 GetScale() const;
	// Sets the the absolute scale
	void SetScale(const Vector3& scale);

	// Returns the absolute transformation matrix
	const Matrix4& GetWorldMatrix() const;

private:
	enum EFlags
	{
		POSITION_CHANGED	= 0x01,
		ROTATION_CHANGED	= 0x02,
		SCALE_CHANGED		= 0x04,
		WORLD_MATRIX_DIRTY	= (POSITION_CHANGED | ROTATION_CHANGED | SCALE_CHANGED)
	};

	Vector3 localPosition = Vector3::Zero();
	Quat localRotation = Quat::Identity();
	Vector3 localScale = Vector3(1.0f);

	mutable Matrix4 worldMatrix = Matrix4::Identity();
	mutable uint8_t flags = 0;

	Transform* parent = nullptr;
	Object* owner = nullptr;

	friend class Object;
};
