#pragma once

class Vector3;
class AABB;

class DebugDraw
{
public:
	DebugDraw();
	~DebugDraw();

	void AddLine(const Vector3& a, const Vector3& b, const Vector3& color);
	void AddPoint(const Vector3& a, float pointSize, const Vector3& color);
	void AddAABB(const AABB& aabb, const Vector3& color);
	void AddText(const Vector3& pos, const char* text);

	void Reset();
};
