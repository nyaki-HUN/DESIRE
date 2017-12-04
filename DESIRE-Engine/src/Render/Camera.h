#pragma once

#include "Core/math/vectormath.h"

class Camera
{
public:
	Camera();
	~Camera();

	void SetViewMatrix(const Matrix4& mat);
	void SetProjectionMatrix(const Matrix4& mat);

	const Matrix4& GetViewMatrix() const;
	const Matrix4& GetProjectionMatrix() const;

	void SetPosition(const Vector3& pos);
	void SetTarget(const Vector3& targetPos);
	void SetFov(float newFov);
	void SetAspectRatio(float aspectRatio);

	const Vector3& GetPosition() const;
	const Vector3& GetTarget() const;
	float GetFov() const;
	float GetAspectRatio() const;

	void RecalculateMatrices();

	void CalculateFrustum(Vector3(&points)[8]) const;

	static Matrix4 CreateViewMatrix(const Vector3& eyePos, const Vector3& lookAtPos, const Vector3& upVec = Vector3(0.0f, 1.0f, 0.0f));
	static Matrix4 CreatePerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar);
	static Matrix4 CreateOrthographicProjectonMatrix(float width, float height, float zNear, float zFar);

private:
	enum EFlags
	{
		VIEW_MATRIX_IS_DIRTY			= 1 << 0,
		PROJECTION_MATRIX_IS_DIRTY		= 1 << 1,

		ALL_MATRICES_ARE_DIRTY = VIEW_MATRIX_IS_DIRTY | PROJECTION_MATRIX_IS_DIRTY
	};

	Matrix4 viewMat;
	Matrix4 projMat;

	Vector3 position;
	Vector3 target;
	float fov = 60.0f;
	float aspect = 16.0f / 9.0f;
	float zNear = 0.1f;
	float zFar = 1000.0f;

	int flags = 0;
};
