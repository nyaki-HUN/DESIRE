#pragma once

#include "Engine/Core/Math/Matrix4.h"

class Object;

class Transform
{
public:
	Transform() = default;

	const Vector3& GetLocalPosition() const;
	const Quat& GetLocalRotation() const;
	const Vector3& GetLocalScale() const;

	void SetLocalPosition(const Vector3& position);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalScale(const Vector3& scale);

	Matrix4 ConstructLocalMatrix() const;
	void CopyTo(Transform& other) const;
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
	DESIRE_NO_COPY_AND_MOVE(Transform)

	enum EFlags
	{
		POSITION_CHANGED	= 0x01,
		ROTATION_CHANGED	= 0x02,
		SCALE_CHANGED		= 0x04,
		WORLD_MATRIX_DIRTY	= (POSITION_CHANGED | ROTATION_CHANGED | SCALE_CHANGED)
	};

	Vector3 m_localPosition = Vector3::Zero();
	Quat m_localRotation = Quat::Identity();
	Vector3 m_localScale = Vector3::One();

	mutable Matrix4 m_worldMatrix = Matrix4::Identity();
	mutable uint8_t m_flags = 0;

	Transform* m_pParent = nullptr;
	Object* m_pOwner = nullptr;

	friend class Object;
};
