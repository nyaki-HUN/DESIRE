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

	void SetLocalPosition(const Vector3& position);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalScale(const Vector3& scale);

	inline const Vector3& GetLocalPosition() const		{ return localPosition; }
	inline const Quat& GetLocalRotation() const			{ return localRotation; }
	inline const Vector3& GetLocalScale() const			{ return localScale; }

	// Returns the rotation as Euler angles in degrees
	Vector3 GetLocalRotationEulerAngles() const;

	// Returns the absolute transformation matrix from the last UpdateWorldMatrix() call
	const Matrix4& GetWorldMatrix() const;

	// Sets the the absolute position
	void SetPosition(const Vector3& position);
	// Returns the absolute position
	Vector3 GetPosition() const;

	// Sets the the absolute rotation
	void SetRotation(const Quat& rotation);
	// Returns the absolute rotation
	Quat GetRotation() const;

	// Returns a combination of flags from EFlags
	uint8_t GetFlags() const;

	Matrix4 ConstructLocalMatrix() const;

	void UpdateWorldMatrix();

private:
	Vector3 localPosition;
	Quat localRotation;
	Vector3 localScale;

	Matrix4 worldMatrix;

	const Matrix4 *parentWorldMatrix;
	Object *owner;
	uint8_t flags;
};
