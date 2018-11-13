#pragma once

class Box2DGlobalSettings
{
public:
	// A small length/angle used as a collision and constraint tolerance. Usually it is chosen to be numerically significant, but visually insignificant.
	static void SetLinearSlop(float value);
	static float GetLinearSlop();
	static void SetAngularSlop(float value);
	static float GetAngularSlop();

	// A velocity threshold for elastic collisions. Any collision with a relative linear velocity below this threshold will be treated as inelastic.
	static void SetVelocityThreshold(float value);
	static float GetVelocityThreshold();

	// The maximum linear position correction used when solving constraints (which helps to prevent overshoot)
	static void SetMaxLinearCorrection(float value);
	static float GetMaxLinearCorrection();

	// The maximum angular position correction used when solving constraints (which helps to prevent overshoot)
	static void SetMaxAngularCorrection(float value);
	static float GetMaxAngularCorrection();

	// This scale factor controls how fast overlap is resolved. Ideally this would be 1 so that overlap is removed in one time step. However using values close to 1 often lead to overshoot.
	static void SetBaumgarteScale(float value);
	static float GetBaumgarteScale();
	static void SetBaugarteTimeOfImpactScale(float value);
	static float GetBaugarteTimeOfImpactScale();

	// The time that a body must be still before it will go to sleep
	static void SetTimeToSleep(float value);
	static float GetTimeToSleep();

	// A body cannot sleep if its linear/angular velocity is above this tolerance
	static void SetLinearSleepTolerance(float value);
	static float GetLinearSleepTolerance();
	static void SetAngularSleepTolerance(float value);
	static float GetAngularSleepTolerance();
};
