#include "stdafx.h"
#include "Render/Camera.h"
#include "Core/math/math.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

const Matrix4& Camera::GetViewMatrix() const
{
	return viewMat;
}

const Matrix4& Camera::GetProjectionMatrix() const
{
	return projMat;
}

void Camera::CalculateFrustum(Vector3(&points)[8]) const
{
	Matrix4 invView = viewMat;
	invView.OrthoInvert();
	Matrix4 invProj = projMat;
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
	Vector3 v3Y = upVec;
	v3Y.Normalize();
	const Vector3 v3Z = (eyePos - lookAtPos).Normalize();
	const Vector3 v3X = (v3Y.Cross(v3Z)).Normalize();
	v3Y = v3Z.Cross(v3X);
	Matrix4 matView = Matrix4(Vector4(v3X), Vector4(v3Y), Vector4(v3Z), Vector4(eyePos));
	matView.OrthoInvert();
	return matView;
}

Matrix4 Camera::CreatePerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar)
{
	const float yScale = tanf(PI_2 - (Math::DegToRad(fov) * 0.5f));
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
