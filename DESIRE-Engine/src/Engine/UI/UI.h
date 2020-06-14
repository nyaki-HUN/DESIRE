#pragma once

#include "Engine/Core/Math/Vector2.h"

class String;
class Vector3;

class UI
{
public:
	static bool Button(const String& label, const Vector2& size = Vector2::Zero());

	static bool InputField(const String& label, Vector3& value);

	static bool Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue);
	static bool Slider(const String& label, float& value, float minValue, float maxValue);
};
