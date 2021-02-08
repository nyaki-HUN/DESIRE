#include "Engine/stdafx.h"
#include "Engine/Core/Math/AABB.h"

AABB::AABB(const Vector3& m_minEdge, const Vector3& m_maxEdge)
	: m_minEdge(m_minEdge)
	, m_maxEdge(m_maxEdge)
{
	// Make sure it is valid
	ASSERT(m_minEdge <= m_maxEdge);
}

bool AABB::IntersectsWithAABB(const AABB& other) const
{
	return (m_minEdge <= other.m_maxEdge && m_maxEdge >= other.m_minEdge);
}

bool AABB::IntersectsWithAABB2D(const AABB& other) const
{
	return (m_minEdge.GetX() <= other.m_maxEdge.GetX() && m_minEdge.GetZ() <= other.m_maxEdge.GetZ() &&
			m_maxEdge.GetX() >= other.m_minEdge.GetX() && m_maxEdge.GetZ() >= other.m_minEdge.GetZ());
}

bool AABB::IsAABBFullyInside(const AABB& other) const
{
	return (m_minEdge >= other.m_minEdge && m_maxEdge <= other.m_maxEdge);
}

bool AABB::IsAABBFullyInside2D(const AABB& other) const
{
	return (m_minEdge.GetX() >= other.m_minEdge.GetX() && m_minEdge.GetZ() >= other.m_minEdge.GetZ() &&
			m_maxEdge.GetX() <= other.m_maxEdge.GetX() && m_maxEdge.GetZ() <= other.m_maxEdge.GetZ());
}

bool AABB::IsPointInside(const Vector3& point) const
{
	return (m_minEdge <= point && point <= m_maxEdge);
}

bool AABB::IsPointInside2D(const Vector3& point) const
{
	return (m_minEdge.GetX() <= point.GetX() && point.GetX() <= m_maxEdge.GetX() &&
			m_minEdge.GetZ() <= point.GetZ() && point.GetZ() <= m_maxEdge.GetZ());
}

void AABB::GetPoints(Vector3(&points)[8]) const
{
	const Vector3 center = GetCenter();
	const Vector3 diag = center - m_maxEdge;

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

	points[0] = m_minEdge;
	points[1] = Vector3(m_maxEdge.GetX(), m_minEdge.GetY(), m_minEdge.GetZ());
	points[2] = Vector3(m_minEdge.GetX(), m_minEdge.GetY(), m_maxEdge.GetZ());
	points[3] = Vector3(m_maxEdge.GetX(), m_minEdge.GetY(), m_maxEdge.GetZ());
	points[4] = Vector3(m_minEdge.GetX(), m_maxEdge.GetY(), m_minEdge.GetZ());
	points[5] = Vector3(m_maxEdge.GetX(), m_maxEdge.GetY(), m_minEdge.GetZ());
	points[6] = Vector3(m_minEdge.GetX(), m_maxEdge.GetY(), m_maxEdge.GetZ());
	points[7] = m_maxEdge;
}

void AABB::GetPoints2D(Vector3(&points)[4]) const
{
/*	Points are stored in this way:
	   2---------3
	  /         /
	 /         /
	0---------1
*/

	points[0] = Vector3(m_minEdge.GetX(), 0.0f, m_minEdge.GetZ());
	points[1] = Vector3(m_maxEdge.GetX(), 0.0f, m_minEdge.GetZ());
	points[2] = Vector3(m_minEdge.GetX(), 0.0f, m_maxEdge.GetZ());
	points[3] = Vector3(m_maxEdge.GetX(), 0.0f, m_maxEdge.GetZ());
}

Vector3 AABB::GetCenter() const
{
	return (m_minEdge + m_maxEdge) * 0.5f; 
}

Vector3 AABB::GetSize() const
{
	return m_maxEdge - m_minEdge; 
}

const Vector3& AABB::GetMinEdge() const
{
	return m_minEdge;
}

const Vector3& AABB::GetMaxEdge() const
{
	return m_maxEdge;
}

void AABB::AddPoint(const Vector3& other)
{
	m_minEdge = Vector3::MinPerElem(m_minEdge, other);
	m_maxEdge = Vector3::MaxPerElem(m_maxEdge, other);
}

void AABB::AddAABB(const AABB& other)
{
	AddPoint(other.m_minEdge);
	AddPoint(other.m_maxEdge);
}
