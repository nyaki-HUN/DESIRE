#pragma once

class Box2DGlobalSettings
{
public:
	// Collision
	static void SetLinearSlop(float value);
	static float GetLinearSlop();
	static void SetAngularSlop(float value);
	static float GetAngularSlop();

	// Dynamics
	static void SetVelocityThreshold(float value);
	static float GetVelocityThreshold();

	static void SetMaxLinearCorrection(float value);
	static float GetMaxLinearCorrection();
	static void SetMaxAngularCorrection(float value);
	static float GetMaxAngularCorrection();

	static void SetBaumgarteScale(float value);
	static float GetBaumgarteScale();
	static void SetBaugarteTimeOfImpactScale(float value);
	static float GetBaugarteTimeOfImpactScale();

	// Sleep
	static void SetTimeToSleep(float value);
	static float GetTimeToSleep();

	static void SetLinearSleepTolerance(float value);
	static float GetLinearSleepTolerance();
	static void SetAngularSleepTolerance(float value);
	static float GetAngularSleepTolerance();
};
