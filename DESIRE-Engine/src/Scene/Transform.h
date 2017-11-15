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

	void SetPosition(const Vector3& newPosition);
	void SetRotation(const Quat& newRotation);
	void SetScale(const Vector3& newScale);

	inline const Vector3& GetPosition() const		{ return position; }
	inline const Quat& GetRotation() const			{ return rotation; }
	inline const Vector3& GetScale() const			{ return scale; }

	// Returns a combination of flags from EFlags
	uint8_t GetFlags() const;

	Matrix4 ConstructLocalMatrix() const;

	// Returns the absolute transformation matrix from the last UpdateWorldMatrix() call
	const Matrix4& GetWorldMatrix() const;
	// Returns the position from the world matrix
	Vector3 GetWorldPosition() const;

	void UpdateWorldMatrix();

private:
	Vector3 position;
	Quat rotation;
	Vector3 scale;

	Matrix4 worldMatrix;

	const Matrix4 *parentWorldMatrix;
	Object *owner;
	uint8_t flags;
};
