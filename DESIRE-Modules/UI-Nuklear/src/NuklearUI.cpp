#include "stdafx_Nuklear.h"
#include "NuklearUI.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/String/String.h"

NuklearUI::NuklearUI()
	: allocator(std::make_unique<nk_allocator>())
{
	allocator->userdata.ptr = this;
}

NuklearUI::~NuklearUI()
{
}

void NuklearUI::Init()
{
}

void NuklearUI::Kill()
{
}

void NuklearUI::BeginFrame(OSWindow* pWindow)
{
}

void NuklearUI::EndFrame()
{
}

void NuklearUI::Render()
{
}

void NuklearUI::BeginWindow(const String& label)
{
}

void NuklearUI::EndWindow()
{
}

void NuklearUI::Text(const String& label)
{
}

bool NuklearUI::Button(const String& label, const Vector2& size)
{
	return false;
}

bool NuklearUI::RadioButton(const String& label, bool isActive)
{
	return false;
}

bool NuklearUI::InputField(const String& label, float& value)
{
	return false;
}

bool NuklearUI::InputField(const String& label, Vector3& value)
{
	return false;
}

bool NuklearUI::Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue)
{
	return false;
}

bool NuklearUI::Slider(const String& label, float& value, float minValue, float maxValue)
{
	return false;
}

bool NuklearUI::Checkbox(const String& label, bool& isChecked)
{
	return false;
}

void NuklearUI::SameLine()
{
}
