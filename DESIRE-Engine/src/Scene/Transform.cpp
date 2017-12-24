#include "stdafx.h"
#include "Scene/Transform.h"
#include "Core/math/math.h"

Transform::Transform()
	: localPosition(0.0f)
	, localRotation(0.0f, 0.0f, 0.0f, 1.0f)
	, localScale(1.0f)
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
	localPosition = Vector3(0.0f);
	localRotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);
	localScale = Vector3(1.0f);
	flags = IS_IDENTITY | POSITION_CHANGED | ROTATION_CHANGED | SCALE_CHANGED;
}

void Transform::SetLocalPosition(const Vector3& position)
{
	localPosition = position;
	flags |= POSITION_CHANGED;
	flags &= ~IS_IDENTITY;
}

void Transform::SetLocalRotation(const Quat& rotation)
{
	localRotation = rotation;
	flags |= ROTATION_CHANGED;
	flags &= ~IS_IDENTITY;
}

void Transform::SetLocalScale(const Vector3& scale)
{
	localScale = scale;
	flags |= SCALE_CHANGED;
	flags &= ~IS_IDENTITY;
}

Vector3 Transform::GetLocalRotationEulerAngles() const
{
	const float ysqr = localRotation.GetY() * localRotation.GetY();

	const float t0 = 2.0f * (localRotation.GetW() * localRotation.GetX() + localRotation.GetY() * localRotation.GetZ());
	const float t1 = 1.0f - 2.0f * (localRotation.GetX() * localRotation.GetX() + ysqr);
	const float x = std::atan2(t0, t1);

	const float t2 = 2.0f * (localRotation.GetW() * localRotation.GetY() - localRotation.GetZ() * localRotation.GetX());
	const float y = std::asin(Math::Clamp(t2, -1.0f, 1.0f));

	const float t3 = 2.0f * (localRotation.GetW() * localRotation.GetZ() + localRotation.GetX() * localRotation.GetY());
	const float t4 = 1.0f - 2.0f * (ysqr + localRotation.GetZ() * localRotation.GetZ());
	const float z = std::atan2(t3, t4);

	Vector3 eulerAngles(x, y, z);
	return Math::RadToDeg(eulerAngles);
}

const Matrix4& Transform::GetWorldMatrix() const
{
	return worldMatrix;
}

void Transform::SetPosition(const Vector3& position)
{
	if(parentWorldMatrix != nullptr)
	{
		ASSERT(false && "TODO");
	}
	else
	{
		SetLocalPosition(position);
	}
}

Vector3 Transform::GetPosition() const
{
	return worldMatrix.GetTranslation();
}

Quat Transform::GetRotation() const
{
	return Quat(worldMatrix.GetUpper3x3());
}

void Transform::SetRotation(const Quat& rotation)
{
	if(parentWorldMatrix != nullptr)
	{
		Quat parentRotation(parentWorldMatrix->GetUpper3x3());
		SetLocalRotation(parentRotation.Conjugate() * rotation);
	}
	else
	{
		SetLocalRotation(rotation);
	}
}

uint8_t Transform::GetFlags() const
{
	return flags;
}

Matrix4 Transform::ConstructLocalMatrix() const
{
	Matrix4 mat = Matrix4(localRotation, localPosition);
	mat.AppendScale(localScale);
	return mat;
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
			worldMatrix = *parentWorldMatrix * worldMatrix;
		}
	}

	flags &= ~WORLD_MATRIX_DIRTY;
}
