#pragma once

#include "Engine/Core/Math/Vector3.h"

// --------------------------------------------------------------------------------------------------------------------
//	Axis Aligned Bounding Box in 3D space
// --------------------------------------------------------------------------------------------------------------------

class AABB
{
public:
	AABB(const Vector3& minEdge = Vector3::Zero(), const Vector3& maxEdge = Vector3::Zero());

	bool IntersectsWithAABB(const AABB& other) const;
	bool IntersectsWithAABB2D(const AABB& other) const;

	bool IsAABBFullyInside(const AABB& other) const;
	bool IsAABBFullyInside2D(const AABB& other) const;

	bool IsPointInside(const Vector3& point) const;
	bool IsPointInside2D(const Vector3& point) const;

	void GetPoints(Vector3(&points)[8]) const;
	void GetPoints2D(Vector3(&points)[4]) const;

	Vector3 GetCenter() const;
	Vector3 GetSize() const;

	const Vector3& GetMinEdge() const;
	const Vector3& GetMaxEdge() const;

	void AddPoint(const Vector3& other);
	void AddAABB(const AABB& other);

private:
	Vector3 m_minEdge;
	Vector3 m_maxEdge;
};
