#include "Engine/stdafx.h"

#if defined(DESIRE_PLATFORM_ANDROID)

#include "Engine/Input/Input.h"

#include <android/sensor.h>

static As_sensorManager *s_sensorManager = nullptr;
static As_sensorEventQueue *s_sensorEventQueue = nullptr;
static const ASensor *s_accelerometerSensor = nullptr;
static const ASensor *s_gyroscopeSensor = nullptr;
static const ASensor *s_magneticSensor = nullptr;

// --------------------------------------------------------------------------------------------------------------------
//	Input
// --------------------------------------------------------------------------------------------------------------------

void Input::Init_internal(OSWindow *window)
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

	s_sensorManager = As_sensorManager_getInstance();
	if(s_sensorManager == nullptr)
	{
		return;
	}

	s_sensorEventQueue = As_sensorManager_createEventQueue(s_sensorManager, looper, ALOOPER_POLL_CALLBACK, nullptr, nullptr);
	if(s_sensorEventQueue == nullptr)
	{
		return;
	}

	s_accelerometerSensor = As_sensorManager_getDefaultSensor(s_sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	if(s_accelerometerSensor != nullptr)
	{
		As_sensorEventQueue_setEventRate(s_sensorEventQueue, s_accelerometerSensor, ASensor_getMinDelay(s_accelerometerSensor));
		As_sensorEventQueue_enableSensor(s_sensorEventQueue, s_accelerometerSensor);
	}

	s_gyroscopeSensor = As_sensorManager_getDefaultSensor(s_sensorManager, ASENSOR_TYPE_GYROSCOPE);
	if(s_gyroscopeSensor != nullptr)
	{
		As_sensorEventQueue_setEventRate(s_sensorEventQueue, s_gyroscopeSensor, ASensor_getMinDelay(s_gyroscopeSensor));
		As_sensorEventQueue_enableSensor(s_sensorEventQueue, s_gyroscopeSensor);
	}

	s_magneticSensor = As_sensorManager_getDefaultSensor(s_sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
	if(s_magneticSensor != nullptr)
	{
		As_sensorEventQueue_setEventRate(s_sensorEventQueue, s_magneticSensor, ASensor_getMinDelay(s_magneticSensor));
		As_sensorEventQueue_enableSensor(s_sensorEventQueue, s_magneticSensor);
	}
}

void Input::Kill_internal()
{
	if(s_sensorManager != nullptr && s_sensorEventQueue != nullptr)
	{
		if(s_accelerometerSensor != nullptr)
		{
			As_sensorEventQueue_disableSensor(s_sensorEventQueue, s_accelerometerSensor);
			s_accelerometerSensor = nullptr;
		}

		if(s_gyroscopeSensor != nullptr)
		{
			As_sensorEventQueue_disableSensor(s_sensorEventQueue, s_gyroscopeSensor);
			s_gyroscopeSensor = nullptr;
		}

		if(s_magneticSensor != nullptr)
		{
			As_sensorEventQueue_disableSensor(s_sensorEventQueue, s_magneticSensor);
			s_magneticSensor = nullptr;
		}

		As_sensorManager_destroyEventQueue(s_sensorManager, s_sensorEventQueue);
	}
	s_sensorManager = nullptr;
	s_sensorEventQueue = nullptr;
}

void Input::Update_internal()
{
	ASensorEvent event;
	while(As_sensorEventQueue_getEvents(s_sensorEventQueue, &event, 1) > 0)
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

#endif	// #if defined(DESIRE_PLATFORM_ANDROID)
