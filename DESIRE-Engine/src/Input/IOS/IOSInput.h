#pragma once

#import <CoreMotion/CoreMotion.h>

class InputImpl
{
public:
	static void Update();

	static void ResetGyroscope();

private:
	static CMMotionManager *motionManager;
	static CMAttitude *referenceAttitude;
};
