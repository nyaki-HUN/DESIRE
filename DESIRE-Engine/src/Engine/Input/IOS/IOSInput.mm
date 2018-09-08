#include "Engine/stdafx.h"
#include "Engine/Input/IOS/IOSInput.h"
#include "Engine/Input/Input.h"
#include "Engine/Core/Modules.h"

CMMotionManager *InputImpl::motionManager = nil;
CMAttitude *InputImpl::referenceAttitude = nil;

void Input::Init(IWindow *window)
{
	motionManager = [[CMMotionManager alloc] init];

	// Set refresh rates (10-100 Hz)
	motionManager.deviceMotionUpdateInterval = 1.0f / 60.0f;
	motionManager.accelerometerUpdateInterval = 1.0f / 60.0f;
	motionManager.gyroUpdateInterval = 1.0f / 60.0f;
	motionManager.magnetometerUpdateInterval = 1.0f / 60.0f;

	[motionManager startDeviceMotionUpdates];
}

void Input::Kill()
{
	[motionManager stopDeviceMotionUpdates];
	[motionManager release];
	motionManager = nil;

	[referenceAttitude release];
	referenceAttitude = nil;

	// Reset input devices
	Modules::Input->keyboards.clear();
	Modules::Input->mouses.clear();
	Modules::Input->gameControllers.clear();
}

void InputImpl::Update()
{
	if(motionManager.deviceMotion == nil)
	{
		return;
	}

	// Accelerometer
	if([motionManager isAccelerometerAvailable])
	{
		gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_X, motionManager.deviceMotion.userAcceleration.x);
		gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_Y, motionManager.deviceMotion.userAcceleration.y);
		gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_Z, motionManager.deviceMotion.userAcceleration.z);
	}

	// Gyroscope
	if([motionManager isGyroAvailable])
	{
		CMAttitude *attitude = motionManager.deviceMotion.attitude;
		if(referenceAttitude != nil)
		{
			[attitude multiplyByInverseOfAttitude:referenceAttitude];
		}

		gamepad.HandleAxisAbsolute(SENSOR_GYRO_X, attitude.pitch);
		gamepad.HandleAxisAbsolute(SENSOR_GYRO_Y, attitude.roll);
		gamepad.HandleAxisAbsolute(SENSOR_GYRO_Z, attitude.yaw);
	}

	// Magnetometer
	if([motionManager isMagnetometerAvailable])
	{
		gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_X, motionManager.deviceMotion.magneticField.field.x);
		gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_Y, motionManager.deviceMotion.magneticField.field.y);
		gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_Z, motionManager.deviceMotion.magneticField.field.z);
	}
}

void InputImpl::ResetGyroscope()
{
	[referenceAttitude release];
	referenceAttitude = [motionManager.deviceMotion.attitude retain];
}
