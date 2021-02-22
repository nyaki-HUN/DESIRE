#include "Engine/stdafx.h"
#include "Engine/Core/Math/Transform.h"

#include "Engine/Core/Math/math.h"
#include "Engine/Core/Object.h"

const Vector3& Transform::GetLocalPosition() const
{ 
	return m_localPosition;
}

const Quat& Transform::GetLocalRotation() const
{ 
	return m_localRotation;
}

const Vector3& Transform::GetLocalScale() const
{
	return m_localScale;
}

void Transform::SetLocalPosition(const Vector3& position)
{
	m_localPosition = position;
	m_flags |= POSITION_CHANGED;

	if(m_pOwner)
	{
		m_pOwner->MarkAllChildrenTransformDirty();
	}
}

void Transform::SetLocalRotation(const Quat& rotation)
{
	m_localRotation = rotation;
	m_flags |= ROTATION_CHANGED;

	if(m_pOwner)
	{
		m_pOwner->MarkAllChildrenTransformDirty();
	}
}

void Transform::SetLocalScale(const Vector3& scale)
{
	m_localScale = scale;
	m_flags |= SCALE_CHANGED;

	if(m_pOwner)
	{
		m_pOwner->MarkAllChildrenTransformDirty();
	}
}

Matrix4 Transform::ConstructLocalMatrix() const
{
	Matrix4 mat = Matrix4(m_localRotation, m_localPosition);
	mat.AppendScale(m_localScale);
	return mat;
}

void Transform::CopyTo(Transform& other) const
{
	other.SetLocalPosition(m_localPosition);
	other.SetLocalRotation(m_localRotation);
	other.SetLocalScale(m_localScale);
}

void Transform::ResetToIdentity()
{
	m_localPosition = Vector3::Zero();
	m_localRotation = Quat::Identity();
	m_localScale = Vector3::One();

	m_worldMatrix = m_pParent ? m_pParent->GetWorldMatrix() : Matrix4::Identity();
	m_flags &= ~WORLD_MATRIX_DIRTY;

	if(m_pOwner)
	{
		m_pOwner->MarkAllChildrenTransformDirty();
	}
}

Vector3 Transform::GetPosition() const
{
	return GetWorldMatrix().GetTranslation();
}

void Transform::SetPosition(const Vector3& position)
{
	if(m_pParent)
	{
		Matrix4 mat = m_pParent->GetWorldMatrix();
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
	if(m_pParent)
	{
		const Quat parentRotation = m_pParent->GetRotation();
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
	if(m_pParent)
	{
		const Vector3 parentScale = m_pParent->GetScale();
		SetLocalScale(scale / parentScale);
	}
	else
	{
		SetLocalScale(scale);
	}
}

const Matrix4& Transform::GetWorldMatrix() const
{
	if(m_flags & WORLD_MATRIX_DIRTY)
	{
		m_worldMatrix = ConstructLocalMatrix();
		if(m_pParent)
		{
			m_worldMatrix = m_pParent->GetWorldMatrix() * m_worldMatrix;
		}

		m_flags &= ~WORLD_MATRIX_DIRTY;
	}

	return m_worldMatrix;
}
