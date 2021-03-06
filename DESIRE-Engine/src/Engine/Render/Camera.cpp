#include "Engine/stdafx.h"
#include "Engine/Render/Camera.h"

#include "Engine/Core/Math/math.h"

Camera::Camera()
{
	SetPosition(Vector3(0.0f, 0.0f, -0.5f));
	SetTarget(Vector3::Zero());
}

Camera::~Camera()
{
}

void Camera::SetViewMatrix(const Matrix4& mat)
{
	m_viewMat = mat;
	m_flags &= ~VIEW_MATRIX_IS_DIRTY;
}

void Camera::SetProjectionMatrix(const Matrix4& mat)
{
	m_projMat = mat;
	m_flags &= PROJECTION_MATRIX_IS_DIRTY;
}

const Matrix4& Camera::GetViewMatrix() const
{
	return m_viewMat;
}

const Matrix4& Camera::GetProjectionMatrix() const
{
	return m_projMat;
}

void Camera::SetPosition(const Vector3& pos)
{
	m_position = pos;
	m_flags |= VIEW_MATRIX_IS_DIRTY;
}

void Camera::SetTarget(const Vector3& targetPos)
{
	m_target = targetPos;
	m_flags |= VIEW_MATRIX_IS_DIRTY;
}

void Camera::SetFov(float newFov)
{
	if(m_fov != newFov)
	{
		m_fov = newFov;
		m_flags |= PROJECTION_MATRIX_IS_DIRTY;
	}
}

void Camera::SetAspectRatio(float aspectRatio)
{
	if(m_aspect != aspectRatio)
	{
		m_aspect = aspectRatio;
		m_flags |= PROJECTION_MATRIX_IS_DIRTY;
	}
}

const Vector3& Camera::GetPosition() const
{
	return m_position;
}

const Vector3& Camera::GetTarget() const
{
	return m_target;
}

float Camera::GetFov() const
{
	return m_fov;
}

float Camera::GetAspectRatio() const
{
	return m_aspect;
}

void Camera::RecalculateMatrices()
{
	if(m_flags & VIEW_MATRIX_IS_DIRTY)
	{
		m_viewMat = CreateViewMatrix(m_position, m_target);
	}

	if(m_flags & PROJECTION_MATRIX_IS_DIRTY)
	{
		m_projMat = CreatePerspectiveProjectionMatrix(m_fov, m_aspect, m_zNear, m_zFar);
	}

	m_flags &= ~ALL_MATRICES_ARE_DIRTY;
}

void Camera::CalculateFrustum(Vector3(&points)[8]) const
{
	Matrix4 invView = m_viewMat;
	invView.OrthoInvert();
	Matrix4 invProj = m_projMat;
	invProj.Invert();

	for(uint8_t i = 0; i < 8; i++)
	{
		Vector4 vec = Vector4(
			((i & 0x1) ? 1.0f : -1.0f),
			((i & 0x2) ? 1.0f : -1.0f),
			((i & 0x4) ? 1.0f : 0.0f),
			1.0f
		);

		vec = invView * invProj * vec;
		if(vec.GetW() != 0.0f)
		{
			vec /= vec.GetW();
		}
		points[i] = vec.GetXYZ();
	}
}

Matrix4 Camera::CreateViewMatrix(const Vector3& eyePos, const Vector3& lookAtPos, const Vector3& upVec)
{
	Vector3 y = upVec.Normalized();
	const Vector3 z = (lookAtPos - eyePos).Normalized();
	const Vector3 x = (y.Cross(z)).Normalized();
	y = z.Cross(x);

	Matrix4 matView = Matrix4(Vector4(x), Vector4(y), Vector4(z), Vector4(eyePos, 1.0f));
	matView.OrthoInvert();
	return matView;
}

Matrix4 Camera::CreatePerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar)
{
	// TODO: swap near and far for better depth precision

	const float yScale = std::tanf(Math::Pi_2 - (Math::DegToRad(fov) * 0.5f));
	const float xScale = yScale / aspect;
	const float invFN = 1.0f / (zFar - zNear);
	return Matrix4(
		Vector4(xScale,	0.0f,	0.0f,					0.0f),
		Vector4(0.0f,	yScale,	0.0f,					0.0f),
		Vector4(0.0f,	0.0f,	zFar * invFN,			1.0f),
		Vector4(0.0f,	0.0f,	-zNear * zFar * invFN,	0.0f)
	);
}

Matrix4 Camera::CreateOrthographicProjectonMatrix(float width, float height, float zNear, float zFar)
{
	return Matrix4(
		Vector4(2.0f / width,	0.0f,			0.0f,					0.0f),
		Vector4(0.0f,			-2.0f / height,	0.0f,					0.0f),
		Vector4(0.0f,			0.0f,			1.0f / (zFar - zNear),	0.0f),
		Vector4(-1.0f,			1.0f,			zNear / (zNear - zFar),	1.0f)
	);
}
