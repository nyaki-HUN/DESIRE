#include "stdafx.h"
#include "Input/ANDROID/ANDROIDInput.h"
#include "Input/Input.h"

void Input::Init(IWindow *window)
{
	ALooper *looper = ALooper_forThread();
	if(looper == nullptr)
	{
		looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
		if(looper == nullptr)
		{
			return;
		}
	}

	sensorManager = ASensorManager_getInstance();
	if(sensorManager == nullptr)
	{
		return;
	}

	sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, ALOOPER_POLL_CALLBACK, nullptr, nullptr);
	if(sensorEventQueue == nullptr)
	{
		return;
	}

	accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	if(accelerometerSensor != nullptr)
	{
		ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, ASensor_getMinDelay(accelerometerSensor));
		ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
	}

	gyroscopeSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
	if(gyroscopeSensor != nullptr)
	{
		ASensorEventQueue_setEventRate(sensorEventQueue, gyroscopeSensor, ASensor_getMinDelay(gyroscopeSensor));
		ASensorEventQueue_enableSensor(sensorEventQueue, gyroscopeSensor);
	}

	magneticSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
	if(magneticSensor != nullptr)
	{
		ASensorEventQueue_setEventRate(sensorEventQueue, magneticSensor, ASensor_getMinDelay(magneticSensor));
		ASensorEventQueue_enableSensor(sensorEventQueue, magneticSensor);
	}
}

void Input::Kill()
{
	if(sensorManager != nullptr && sensorEventQueue != nullptr)
	{
		if(accelerometerSensor != nullptr)
		{
			ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
			accelerometerSensor = nullptr;
		}

		if(gyroscopeSensor != nullptr)
		{
			ASensorEventQueue_disableSensor(sensorEventQueue, gyroscopeSensor);
			gyroscopeSensor = nullptr;
		}

		if(magneticSensor != nullptr)
		{
			ASensorEventQueue_disableSensor(sensorEventQueue, magneticSensor);
			magneticSensor = nullptr;
		}

		ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
	}
	sensorManager = nullptr;
	sensorEventQueue = nullptr;

	// Reset input devices
	Input::Get()->keyboards.clear();
	Input::Get()->mouses.clear();
	Input::Get()->gameControllers.clear();
}

void InputImpl::Update()
{
	ASensorEvent event;
	while(ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0)
	{
		if(event.type == ASENSOR_TYPE_ACCELEROMETER)
		{
			gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_X, event.acceleration.x / ASENSOR_STANDARD_GRAVITY);
			gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_Y, event.acceleration.y / ASENSOR_STANDARD_GRAVITY);
			gamepad.HandleAxisAbsolute(SENSOR_ACCELERATION_Z, event.acceleration.z / ASENSOR_STANDARD_GRAVITY);
		}
		else if(event.type == ASENSOR_TYPE_GYROSCOPE)
		{
			gamepad.HandleAxisAbsolute(SENSOR_GYRO_X, event.vector.x / ASENSOR_STANDARD_GRAVITY);
			gamepad.HandleAxisAbsolute(SENSOR_GYRO_Y, event.vector.y / ASENSOR_STANDARD_GRAVITY);
			gamepad.HandleAxisAbsolute(SENSOR_GYRO_Z, event.vector.z / ASENSOR_STANDARD_GRAVITY);
		}
		else if(event.type == ASENSOR_TYPE_MAGNETIC_FIELD)
		{
			gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_X, event.magnetic.x / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
			gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_Y, event.magnetic.y / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
			gamepad.HandleAxisAbsolute(SENSOR_MAGNETIC_Z, event.magnetic.z / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
		}
	}
}
