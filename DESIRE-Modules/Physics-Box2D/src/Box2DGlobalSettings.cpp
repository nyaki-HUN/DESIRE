#include "stdafx_Box2D.h"
#include "Box2DGlobalSettings.h"

void Box2DGlobalSettings::SetLinearSlop(float value)
{
	b2_linearSlop = value;
}

float Box2DGlobalSettings::GetLinearSlop()
{
	return b2_linearSlop;
}

void Box2DGlobalSettings::SetAngularSlop(float value)
{
	b2_angularSlop = value;
}

float Box2DGlobalSettings::GetAngularSlop()
{
	return b2_angularSlop;
}

void Box2DGlobalSettings::SetVelocityThreshold(float value)
{
	b2_velocityThreshold = value;
}

float Box2DGlobalSettings::GetVelocityThreshold()
{
	return b2_velocityThreshold;
}

void Box2DGlobalSettings::SetMaxLinearCorrection(float value)
{
	b2_maxLinearCorrection = value;
}

float Box2DGlobalSettings::GetMaxLinearCorrection()
{
	return b2_maxLinearCorrection;
}

void Box2DGlobalSettings::SetMaxAngularCorrection(float value)
{
	b2_maxAngularCorrection = (value / 180.0f * b2_pi);
}

float Box2DGlobalSettings::GetMaxAngularCorrection()
{
	return b2_maxAngularCorrection / b2_pi * 180.0f;
}

void Box2DGlobalSettings::SetBaumgarteScale(float value)
{
	b2_baumgarte = value;
}

float Box2DGlobalSettings::GetBaumgarteScale()
{
	return b2_baumgarte;
}

void Box2DGlobalSettings::SetBaugarteTimeOfImpactScale(float value)
{
	b2_toiBaugarte = value;
}

float Box2DGlobalSettings::GetBaugarteTimeOfImpactScale()
{
	return b2_toiBaugarte;
}

void Box2DGlobalSettings::SetTimeToSleep(float value)
{
	b2_timeToSleep = value;
}

float Box2DGlobalSettings::GetTimeToSleep()
{
	return b2_timeToSleep;
}

void Box2DGlobalSettings::SetLinearSleepTolerance(float value)
{
	b2_linearSleepTolerance = value;
}

float Box2DGlobalSettings::GetLinearSleepTolerance()
{
	return b2_linearSleepTolerance;
}

void Box2DGlobalSettings::SetAngularSleepTolerance(float value)
{
	b2_angularSleepTolerance = (value / 180.0f * b2_pi);
}

float Box2DGlobalSettings::GetAngularSleepTolerance()
{
	return b2_angularSleepTolerance / b2_pi * 180.0f;
}
