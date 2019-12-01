#include "Engine/stdafx.h"
#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Math/math.h"
#include "Engine/Core/Object.h"

Transform::Transform()
{

}

void Transform::SetLocalPosition(const Vector3& position)
{
	localPosition = position;
	flags |= POSITION_CHANGED;

	if(owner != nullptr)
	{
		owner->MarkAllChildrenTransformDirty();
	}
}

void Transform::SetLocalRotation(const Quat& rotation)
{
	localRotation = rotation;
	flags |= ROTATION_CHANGED;

	if(owner != nullptr)
	{
		owner->MarkAllChildrenTransformDirty();
	}
}

void Transform::SetLocalScale(const Vector3& scale)
{
	localScale = scale;
	flags |= SCALE_CHANGED;

	if(owner != nullptr)
	{
		owner->MarkAllChildrenTransformDirty();
	}
}

Matrix4 Transform::ConstructLocalMatrix() const
{
	Matrix4 mat = Matrix4(localRotation, localPosition);
	mat.AppendScale(localScale);
	return mat;
}

void Transform::CopyTo(Transform& other) const
{
	other.SetLocalPosition(localPosition);
	other.SetLocalRotation(localRotation);
	other.SetLocalScale(localScale);
}

void Transform::ResetToIdentity()
{
	localPosition = Vector3::Zero();
	localRotation = Quat::Identity();
	localScale = Vector3(1.0f);

	worldMatrix = (parent != nullptr) ? parent->GetWorldMatrix() : Matrix4::Identity();
	flags &= ~WORLD_MATRIX_DIRTY;

	if(owner != nullptr)
	{
		owner->MarkAllChildrenTransformDirty();
	}
}

Vector3 Transform::GetPosition() const
{
	return GetWorldMatrix().GetTranslation();
}

void Transform::SetPosition(const Vector3& position)
{
	if(parent != nullptr)
	{
		Matrix4 mat = parent->GetWorldMatrix();
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
	Matrix3 mat = GetWorldMatrix().GetUpper3x3();
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
	const Matrix3 mat = GetWorldMatrix().GetUpper3x3();
	return Vector3(mat.col0.Length(), mat.col1.Length(), mat.col2.Length());
}

void Transform::SetScale(const Vector3& scale)
{
	if(parent != nullptr)
	{
		const Vector3 parentScale = parent->GetScale();
		SetLocalScale(scale / parentScale);
	}
	else
	{
		SetLocalScale(scale);
	}
}

const Matrix4& Transform::GetWorldMatrix() const
{
	if(flags & WORLD_MATRIX_DIRTY)
	{
		worldMatrix = ConstructLocalMatrix();
		if(parent != nullptr)
		{
			worldMatrix = parent->GetWorldMatrix() * worldMatrix;
		}

		flags &= ~WORLD_MATRIX_DIRTY;
	}

	return worldMatrix;
}
