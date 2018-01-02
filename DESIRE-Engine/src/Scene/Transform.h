#pragma once

#include "Core/math/vectormath.h"

class Object;

class Transform
{
	friend class Object;

public:
	enum EFlags
	{
		POSITION_CHANGED			= 0x01,
		ROTATION_CHANGED			= 0x02,
		SCALE_CHANGED				= 0x04,
		WORLD_MATRIX_DIRTY			= (POSITION_CHANGED | ROTATION_CHANGED | SCALE_CHANGED),
		IS_IDENTITY					= 0x08,
	};

	Transform();
	~Transform();

	// Reset the matrix to have the elements of the identity matrix
	void ResetToIdentity();

	inline const Vector3& GetLocalPosition() const { return localPosition; }
	inline const Quat& GetLocalRotation() const { return localRotation; }
	inline const Vector3& GetLocalScale() const { return localScale; }

	void SetLocalPosition(const Vector3& position);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalScale(const Vector3& scale);

	Matrix4 ConstructLocalMatrix() const;

	// Returns the rotation as Euler angles in degrees
	Vector3 GetLocalRotationEulerAngles() const;

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

	// Returns a combination of flags from EFlags
	uint8_t GetFlags() const;

	// Returns the absolute transformation matrix from the last UpdateWorldMatrix() call
	const Matrix4& GetWorldMatrix() const;

	void UpdateWorldMatrix();

private:
	Vector3 localPosition = Vector3(0.0f);
	Quat localRotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);
	Vector3 localScale = Vector3(1.0f);

	Matrix4 worldMatrix = Matrix4::Identity();

	const Transform *parent = nullptr;
	Object *owner = nullptr;
	uint8_t flags = IS_IDENTITY;
};
