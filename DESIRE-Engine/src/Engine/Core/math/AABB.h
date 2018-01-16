#pragma once

#include "Engine/Core/math/vectormath.h"

// --------------------------------------------------------------------------------------------------------------------
//	Axis Aligned Bounding Box in 3D space
// --------------------------------------------------------------------------------------------------------------------

class AABB
{
public:
	AABB(const Vector3& minEdge = Vector3(0.0f), const Vector3& maxEdge = Vector3(0.0f));

	bool IntersectsWithAABB(const AABB& other) const;
	bool IntersectsWithAABB2D(const AABB& other) const;

	bool IsAABBFullyInside(const AABB& other) const;
	bool IsAABBFullyInside2D(const AABB& other) const;

	bool IsPointInside(const Vector3& point) const;
	bool IsPointInside2D(const Vector3& point) const;

	void GetPoints(Vector3(&points)[8]) const;
	void GetPoints2D(Vector3(&points)[4]) const;

	inline Vector3 GetCenter() const
	{
		return (minEdge + maxEdge) * 0.5f;
	}

	inline Vector3 GetSize() const
	{
		return maxEdge - minEdge;
	}

	inline const Vector3& GetMinEdge() const
	{
		return minEdge;
	}

	inline const Vector3& GetMaxEdge() const
	{
		return maxEdge;
	}

	inline void AddPoint(const Vector3& other)
	{
		minEdge = Vector3::MinPerElem(minEdge, other);
		maxEdge = Vector3::MaxPerElem(maxEdge, other);
	}

	inline void AddAABB(const AABB& other)
	{
		AddPoint(other.minEdge);
		AddPoint(other.maxEdge);
	}

private:
	Vector3 minEdge;
	Vector3 maxEdge;
};
