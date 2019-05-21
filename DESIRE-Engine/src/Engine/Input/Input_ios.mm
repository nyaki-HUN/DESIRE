#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_IOS)

#include "Engine/Input/Input.h"

#import <CoreMotion/CoreMotion.h>

static CMs_motionManager *s_motionManager = nil;
static CMAttitude *s_referenceAttitude = nil;

// --------------------------------------------------------------------------------------------------------------------
//	InputImpl
// --------------------------------------------------------------------------------------------------------------------

class InputImpl
{
public:
	static void InputImpl::ResetGyroscope()
	{
		[s_referenceAttitude release];
		s_referenceAttitude = [s_motionManager.deviceMotion.attitude retain];
	}
};

// --------------------------------------------------------------------------------------------------------------------
//	Input
// --------------------------------------------------------------------------------------------------------------------

void Input::Init_internal(OSWindow *window)
{
	s_motionManager = [[CMs_motionManager alloc] init];

	// Set refresh rates (10-100 Hz)
	s_motionManager.deviceMotionUpdateInterval = 1.0f / 60.0f;
	s_motionManager.accelerometerUpdateInterval = 1.0f / 60.0f;
	s_motionManager.gyroUpdateInterval = 1.0f / 60.0f;
	s_motionManager.magnetometerUpdateInterval = 1.0f / 60.0f;

	[s_motionManager startDeviceMotionUpdates];

	InputImpl::ResetGyroscope();
}

void Input::Kill_internal()
{
	[s_motionManager stopDeviceMotionUpdates];
	[s_motionManager release];
	s_motionManager = nil;

	[s_referenceAttitude release];
	s_referenceAttitude = nil;
}

void Input::Update_internal()
{
	if(s_motionManager.deviceMotion == nil)
	{
		return;
	}

	// Accelerometer
	if([s_motionManager isAccelerometerAvailable])
	{
		gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_X, s_motionManager.deviceMotion.userAcceleration.x);
		gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_Y, s_motionManager.deviceMotion.userAcceleration.y);
		gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_Z, s_motionManager.deviceMotion.userAcceleration.z);
	}

	// Gyroscope
	if([s_motionManager isGyroAvailable])
	{
		CMAttitude *attitude = s_motionManager.deviceMotion.attitude;
		if(s_referenceAttitude != nil)
		{
			[attitude multiplyByInverseOfAttitude:s_referenceAttitude];
		}

		gamepad.HandleAxisAbsolute(SENSOR_GYRO_X, attitude.pitch);
		gamepad.HandleAxisAbsolute(SENSOR_GYRO_Y, attitude.roll);
		gamepad.HandleAxisAbsolute(SENSOR_GYRO_Z, attitude.yaw);
	}

	// Magnetometer
	if([s_motionManager isMagnetometerAvailable])
	{
		gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_X, s_motionManager.deviceMotion.magneticField.field.x);
		gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_Y, s_motionManager.deviceMotion.magneticField.field.y);
		gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_Z, s_motionManager.deviceMotion.magneticField.field.z);
	}
}

#endif	// #if defined(DESIRE_PLATFORM_IOS)
