#pragma once

#include "BulletVectormathExt.h"

#include "Engine/Core/String/String.h"

#include "Engine/Render/DebugDraw.h"

class BulletDebugDraw : public btIDebugDraw
{
public:
	BulletDebugDraw(DebugDraw& debugDraw)
		: m_debugDraw(debugDraw)
	{
	}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
	{
		m_debugDraw.AddLine(GetVector3(from), GetVector3(to), GetVector3(color));
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
		m_debugDraw.AddPoint(GetVector3(PointOnB), 0.1f, GetVector3(color));
	}

	void reportErrorWarning(const char* pWarningString) override
	{
		LOG_ERROR("Bullet Error: %s", pWarningString);
	}

	void draw3dText(const btVector3& location, const char* pTextString) override
	{
		m_debugDraw.AddText(GetVector3(location), String(pTextString, strlen(pTextString)));
	}

	void setDebugMode(int debugMode) override
	{
		m_debugMode = debugMode;
	}

	int getDebugMode() const override
	{
		return m_debugMode;
	}

	DebugDraw& m_debugDraw;

private:
	int m_debugMode = 0;
};
