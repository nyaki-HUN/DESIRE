#include "Engine/stdafx.h"
#include "Engine/Render/DebugDraw.h"
#include "Engine/Core/math/AABB.h"

DebugDraw::DebugDraw()
{

}

DebugDraw::~DebugDraw()
{

}

void DebugDraw::AddLine(const Vector3& a, const Vector3& b, const Vector3& color)
{

}

void DebugDraw::AddPoint(const Vector3& a, float pointSize, const Vector3& color)
{
	AddLine(a + Vector3(pointSize, 0.0f, 0.0f), a - Vector3(pointSize, 0.0f, 0.0f), color);
	AddLine(a + Vector3(0.0f, pointSize, 0.0f), a - Vector3(0.0f, pointSize, 0.0f), color);
	AddLine(a + Vector3(0.0f, 0.0f, pointSize), a - Vector3(0.0f, 0.0f, pointSize), color);
}

void DebugDraw::AddAABB(const AABB& aabb, const Vector3& color)
{
	Vector3 points[8];
	aabb.GetPoints(points);

	// Bottom quad
	AddLine(points[0], points[1], color);
	AddLine(points[2], points[3], color);
	AddLine(points[0], points[2], color);
	AddLine(points[1], points[3], color);
	// Top quad
	AddLine(points[4], points[5], color);
	AddLine(points[6], points[7], color);
	AddLine(points[4], points[6], color);
	AddLine(points[5], points[7], color);
	// Sides
	AddLine(points[0], points[4], color);
	AddLine(points[1], points[5], color);
	AddLine(points[2], points[6], color);
	AddLine(points[3], points[7], color);
}

void DebugDraw::AddText(const Vector3& pos, const char *text)
{

}

void DebugDraw::Reset()
{

}
