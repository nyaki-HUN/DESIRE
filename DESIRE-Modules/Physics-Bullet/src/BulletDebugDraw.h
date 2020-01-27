#pragma once

#include "BulletVectormathExt.h"

#include "Engine/Render/DebugDraw.h"

#include "LinearMath/btIDebugDraw.h"

class BulletDebugDraw : public btIDebugDraw
{
public:
	BulletDebugDraw(DebugDraw& debugDraw)
		: debugDraw(debugDraw)
		, debugMode(0)
	{
	}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
	{
		debugDraw.AddLine(GetVector3(from), GetVector3(to), GetVector3(color));
	}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override
	{
		DESIRE_UNUSED(toColor);
		drawLine(from, to, fromColor);
	}

	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
	{
		DESIRE_UNUSED(normalOnB);
		DESIRE_UNUSED(distance);
		DESIRE_UNUSED(lifeTime);
		debugDraw.AddPoint(GetVector3(PointOnB), 0.1f, GetVector3(color));
	}

	void reportErrorWarning(const char* warningString) override
	{
		LOG_ERROR("Bullet Error: %s", warningString);
	}

	void draw3dText(const btVector3& location, const char* textString) override
	{
		debugDraw.AddText(GetVector3(location), textString);
	}

	void setDebugMode(int i_debugMode) override
	{
		debugMode = i_debugMode;
	}

	int getDebugMode() const override
	{
		return debugMode;
	}

	DebugDraw& debugDraw;

private:
	int debugMode;
};
