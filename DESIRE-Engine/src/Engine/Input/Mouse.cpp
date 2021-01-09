#include "Engine/stdafx.h"
#include "Engine/Input/Mouse.h"

Mouse::Mouse(void* pHandle)
	: InputDevice(pHandle, offsetof(Mouse, m_buttons), static_cast<uint16_t>(DESIRE_ASIZEOF(m_buttons)), offsetof(Mouse, m_axes), static_cast<uint16_t>(DESIRE_ASIZEOF(m_axes)))
{
}
