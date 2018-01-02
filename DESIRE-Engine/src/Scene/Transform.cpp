#include "stdafx.h"
#include "Scene/Transform.h"
#include "Core/math/math.h"

Transform::Transform()
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

	if(owner != nullptr)
	{
		flags = IS_IDENTITY | POSITION_CHANGED | ROTATION_CHANGED | SCALE_CHANGED;
	}
	else
	{
		worldMatrix = Matrix4::Identity();
		flags = IS_IDENTITY;
	}
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

Matrix4 Transform::ConstructLocalMatrix() const
{
	Matrix4 mat = Matrix4(localRotation, localPosition);
	mat.AppendScale(localScale);
	return mat;
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

Vector3 Transform::GetPosition() const
{
	return worldMatrix.GetTranslation();
}

void Transform::SetPosition(const Vector3& position)
{
	if(parent != nullptr)
	{
		Matrix4 mat = parent->worldMatrix;
		mat.Invert();
		mat *= Matrix4::CreateTranslation(position);
		SetLocalPosition(mat.GetTranslation());
	}
	else
	{
		SetLocalPosition(position);
	}
}

Quat Transform::GetRotation() const
{
	Matrix3 mat = worldMatrix.GetUpper3x3();
	mat.col0.Normalize();
	mat.col1.Normalize();
	mat.col2.Normalize();
	return Quat(mat).Normalized();
}

void Transform::SetRotation(const Quat& rotation)
{
	if(parent != nullptr)
	{
		const Quat parentRotation = parent->GetRotation();
		SetLocalRotation(parentRotation.Conjugate() * rotation);
	}
	else
	{
		SetLocalRotation(rotation);
	}
}

Vector3 Transform::GetScale() const
{
	const Matrix3 mat = worldMatrix.GetUpper3x3();
	return Vector3(mat.col0.Length(), mat.col1.Length(), mat.col2.Length());
}

void Transform::SetScale(const Vector3& scale)
{
	if(parent != nullptr)
	{
		const Vector3 parentScale = parent->GetScale();
		SetLocalScale(scale.DivPerElem(parentScale));
	}
	else
	{
		SetLocalScale(scale);
	}
}

uint8_t Transform::GetFlags() const
{
	return flags;
}

void Transform::UpdateWorldMatrix()
{
	if(flags & IS_IDENTITY)
	{
		if(parent != nullptr)
		{
			worldMatrix = parent->worldMatrix;
		}
		else
		{
			worldMatrix = Matrix4::Identity();
		}
	}
	else
	{
		worldMatrix = ConstructLocalMatrix();

		if(parent != nullptr)
		{
			worldMatrix = parent->worldMatrix * worldMatrix;
		}
	}

	flags &= ~WORLD_MATRIX_DIRTY;
}
