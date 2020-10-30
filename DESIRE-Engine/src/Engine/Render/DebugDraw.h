#pragma once

class AABB;
class Vector3;
class String;

class DebugDraw
{
public:
	DebugDraw();
	~DebugDraw();

	void AddLine(const Vector3& a, const Vector3& b, const Vector3& color);
	void AddPoint(const Vector3& a, float pointSize, const Vector3& color);
	void AddAABB(const AABB& aabb, const Vector3& color);
	void AddText(const Vector3& pos, const String& text);

	void Reset();
};
