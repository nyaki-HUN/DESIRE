#include "stdafx.h"
#include "Core/math/AABB.h"

AABB::AABB(const Vector3& minEdge, const Vector3& maxEdge)
	: minEdge(minEdge)
	, maxEdge(maxEdge)
{
	// Make sure it is valid
	ASSERT(minEdge <= maxEdge);
}

bool AABB::IntersectsWithAABB(const AABB& other) const
{
	return (minEdge <= other.maxEdge && maxEdge >= other.minEdge);
}

bool AABB::IntersectsWithAABB2D(const AABB& other) const
{
	return (minEdge.GetX() <= other.maxEdge.GetX() && minEdge.GetZ() <= other.maxEdge.GetZ() && 
			maxEdge.GetX() >= other.minEdge.GetX() && maxEdge.GetZ() >= other.minEdge.GetZ());
}

bool AABB::IsAABBFullyInside(const AABB& other) const
{
	return (minEdge >= other.minEdge && maxEdge <= other.maxEdge);
}

bool AABB::IsAABBFullyInside2D(const AABB& other) const
{
	return (minEdge.GetX() >= other.minEdge.GetX() && minEdge.GetZ() >= other.minEdge.GetZ() &&
			maxEdge.GetX() <= other.maxEdge.GetX() && maxEdge.GetZ() <= other.maxEdge.GetZ());
}

bool AABB::IsPointInside(const Vector3& point) const
{
	return (minEdge <= point && point <= maxEdge);
}

bool AABB::IsPointInside2D(const Vector3& point) const
{
	return (minEdge.GetX() <= point.GetX() && point.GetX() <= maxEdge.GetX() &&
			minEdge.GetZ() <= point.GetZ() && point.GetZ() <= maxEdge.GetZ());
}

void AABB::GetPoints(Vector3(&points)[8]) const
{
	const Vector3 center = GetCenter();
	const Vector3 diag = center - maxEdge;

/*	Edges are stored in this way:
	   6---------7
	  /|        /|
	 / |       / |
	4---------5  |
	|  2------|--3
	| /       | /
	|/        |/
	0---------1
*/

	points[0] = minEdge;
	points[1] = Vector3(maxEdge.GetX(), minEdge.GetY(), minEdge.GetZ());
	points[2] = Vector3(minEdge.GetX(), minEdge.GetY(), maxEdge.GetZ());
	points[3] = Vector3(maxEdge.GetX(), minEdge.GetY(), maxEdge.GetZ());
	points[4] = Vector3(minEdge.GetX(), maxEdge.GetY(), minEdge.GetZ());
	points[5] = Vector3(maxEdge.GetX(), maxEdge.GetY(), minEdge.GetZ());
	points[6] = Vector3(minEdge.GetX(), maxEdge.GetY(), maxEdge.GetZ());
	points[7] = maxEdge;
}

void AABB::GetPoints2D(Vector3(&points)[4]) const
{
/*	Points are stored in this way:
	   2---------3
	  /         /
	 /         /
	0---------1
*/

	points[0] = Vector3(minEdge.GetX(), 0.0f, minEdge.GetZ());
	points[1] = Vector3(maxEdge.GetX(), 0.0f, minEdge.GetZ());
	points[2] = Vector3(minEdge.GetX(), 0.0f, maxEdge.GetZ());
	points[3] = Vector3(maxEdge.GetX(), 0.0f, maxEdge.GetZ());
}
