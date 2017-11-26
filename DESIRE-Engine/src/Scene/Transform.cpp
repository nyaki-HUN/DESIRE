#include "stdafx.h"
#include "Scene/Transform.h"
#include "Core/math/math.h"

Transform::Transform()
	: position(0.0f)
	, rotation(0.0f, 0.0f, 0.0f, 1.0f)
	, scale(1.0f)
	, worldMatrix(Matrix4::Identity())
	, parentWorldMatrix(nullptr)
	, owner(nullptr)
	, flags(IS_IDENTITY)
{

}

Transform::~Transform()
{

}

void Transform::ResetToIdentity()
{
	position = Vector3(0.0f);
	rotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);
	scale = Vector3(1.0f);
	flags = IS_IDENTITY | POSITION_CHANGED | ROTATION_CHANGED | SCALE_CHANGED;
}

void Transform::SetPosition(const Vector3& newPosition)
{
	position = newPosition;
	flags |= POSITION_CHANGED;
	flags &= ~IS_IDENTITY;
}

void Transform::SetRotation(const Quat& newRotation)
{
	rotation = newRotation;
	flags |= ROTATION_CHANGED;
	flags &= ~IS_IDENTITY;
}

void Transform::SetScale(const Vector3& newScale)
{
	scale = newScale;
	flags |= SCALE_CHANGED;
	flags &= ~IS_IDENTITY;
}

Vector3 Transform::GetRotationEulerAngles() const
{
	const float ysqr = rotation.GetY() * rotation.GetY();

	const float t0 = 2.0f * (rotation.GetW() * rotation.GetX() + rotation.GetY() * rotation.GetZ());
	const float t1 = 1.0f - 2.0f * (rotation.GetX() * rotation.GetX() + ysqr);
	const float x = std::atan2(t0, t1);

	const float t2 = 2.0f * (rotation.GetW() * rotation.GetY() - rotation.GetZ() * rotation.GetX());
	const float y = std::asin(Math::Clamp(t2, -1.0f, 1.0f));

	const float t3 = 2.0f * (rotation.GetW() * rotation.GetZ() + rotation.GetX() * rotation.GetY());
	const float t4 = 1.0f - 2.0f * (ysqr + rotation.GetZ() * rotation.GetZ());
	const float z = std::atan2(t3, t4);

	Vector3 eulerAngles(x, y, z);
	return Math::RadToDeg(eulerAngles);
}

uint8_t Transform::GetFlags() const
{
	return flags;
}

Matrix4 Transform::ConstructLocalMatrix() const
{
	Matrix4 mat = Matrix4(rotation, position);
	mat.AppendScale(scale);
	return mat;
}

const Matrix4& Transform::GetWorldMatrix() const
{
	return worldMatrix;
}

Vector3 Transform::GetWorldPosition() const
{
	return worldMatrix.GetTranslation();
}

void Transform::UpdateWorldMatrix()
{
	if(flags & IS_IDENTITY)
	{
		if(parentWorldMatrix != nullptr)
		{
			worldMatrix = *parentWorldMatrix;
		}
		else
		{
			worldMatrix = Matrix4::Identity();
		}
	}
	else
	{
		worldMatrix = ConstructLocalMatrix();

		if(parentWorldMatrix != nullptr)
		{
			worldMatrix *= *parentWorldMatrix;
		}
	}

	flags &= ~WORLD_MATRIX_DIRTY;
}
