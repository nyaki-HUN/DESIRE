#include "Engine/stdafx.h"
#include "Engine/UI/UI.h"

#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/String/String.h"

bool UI::Button(const String& label)
{
	return Button(label, Vector2::Zero());
}

bool UI::Checkbox(bool& isChecked)
{
	return Checkbox(isChecked, String::kEmptyString);
}
