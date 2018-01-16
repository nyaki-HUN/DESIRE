#pragma once

#import <CoreMotion/CoreMotion.h>

class InputImpl
{
public:
	static void Update();
	static void ResetGyroscope();

	static CMMotionManager *motionManager;
	static CMAttitude *referenceAttitude;
};
