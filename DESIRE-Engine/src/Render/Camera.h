#pragma once

#include "Core/math/vectormath.h"

class Camera
{
public:
	Camera();
	~Camera();

	const Matrix4& GetViewMatrix() const;
	const Matrix4& GetProjectionMatrix() const;
	void CalculateFrustum(Vector3(&points)[8]) const;

	static Matrix4 CreateViewMatrix(const Vector3& eyePos, const Vector3& lookAtPos, const Vector3& upVec = Vector3(0.0f, 1.0f, 0.0f));
	static Matrix4 CreatePerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar);
	static Matrix4 CreateOrthographicProjectonMatrix(float width, float height, float zNear, float zFar);

private:
	Matrix4 viewMat;
	Matrix4 projMat;
};
