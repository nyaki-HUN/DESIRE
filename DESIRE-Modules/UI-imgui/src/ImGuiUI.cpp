#include "stdafx_imgui.h"

#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/String/String.h"

#include "Engine/UI/UI.h"

static ImVec2 GetImVec2(const Vector2& vec)
{
	return ImVec2(vec.GetX(), vec.GetY());
}

bool UI::Button(const String& label, const Vector2& size)
{
	return ImGui::Button(label.Str(), GetImVec2(size));
}

bool UI::InputField(const String& label, Vector3& value)
{
	float elements[3];
	value.StoreXYZ(elements);
	if(ImGui::InputFloat3(label.Str(), elements))
	{
		value.LoadXYZ(elements);
		return true;
	}

	return false;
}

bool UI::Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue)
{
	return ImGui::SliderInt(label.Str(), &value, minValue, maxValue);
}

bool UI::Slider(const String& label, float& value, float minValue, float maxValue)
{
	return ImGui::SliderFloat(label.Str(), &value, minValue, maxValue);
}
