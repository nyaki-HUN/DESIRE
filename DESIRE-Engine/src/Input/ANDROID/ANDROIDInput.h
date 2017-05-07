#pragma once

#include <android/sensor.h>

class InputImpl
{
public:
	static void Update();

private:
	static ASensorManager *sensorManager;
	static ASensorEventQueue *sensorEventQueue;
	static const ASensor *accelerometerSensor;
	static const ASensor *gyroscopeSensor;
	static const ASensor *magneticSensor;
};
