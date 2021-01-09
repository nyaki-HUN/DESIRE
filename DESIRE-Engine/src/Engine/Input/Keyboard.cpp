#include "Engine/stdafx.h"
#include "Engine/Input/Keyboard.h"

#include "Engine/Input/Input.h"

Keyboard::Keyboard(void* pHandle)
	: InputDevice(pHandle, offsetof(Keyboard, m_keyStates), static_cast<uint16_t>(DESIRE_ASIZEOF(m_keyStates)))
{
}

EKeyModifier Keyboard::GetActiveKeyModifier() const
{
	const bool isCtrl = IsDown(KEY_LCONTROL) || IsDown(KEY_RCONTROL);
	const bool isShift= IsDown(KEY_LSHIFT) || IsDown(KEY_RSHIFT);
	const bool isAlt = IsDown(KEY_LALT) || IsDown(KEY_RALT);

	if(isCtrl)
	{
		if(isShift)
		{
			return isAlt ? EKeyModifier::Ctrl_Shift_Alt : EKeyModifier::Ctrl_Shift;
		}
		else
		{
			return isAlt ? EKeyModifier::Ctrl_Alt : EKeyModifier::Ctrl;
		}
	}
	else if(isShift)
	{
		return isAlt ? EKeyModifier::Shift_Alt : EKeyModifier::Shift;
	}
	else if(isAlt)
	{
		return EKeyModifier::Alt;
	}

	return EKeyModifier::None;
}
