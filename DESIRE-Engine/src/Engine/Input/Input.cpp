#include "Engine/stdafx.h"
#include "Engine/Input/Input.h"

Input::Input()
{
}

Input::~Input()
{
}

void Input::Init(OSWindow& window)
{
	Init_internal(window);
}

void Input::Kill()
{
	Kill_internal();

	// Reset input devices
	m_keyboards.Clear();
	m_mouses.Clear();
	m_gameControllers.Clear();
}

void Input::Update()
{
	Update_internal();

	// Keyboard
	for(Keyboard& keyboard : m_keyboards)
	{
		keyboard.Update();
	}

	// Mouse
	for(Mouse& mouse : m_mouses)
	{
		mouse.Update();
	}

	// Game Controller
	for(GameController& gamepad : m_gameControllers)
	{
		gamepad.Update();
	}

	m_typingCharacters.Clear();
}

void Input::Reset()
{
	// Keyboard
	for(Keyboard& keyboard : m_keyboards)
	{
		keyboard.Reset();
	}

	// Mouse
	for(Mouse& mouse : m_mouses)
	{
		mouse.Reset();
	}

	// Game Controller
	for(GameController& gamepad : m_gameControllers)
	{
		gamepad.Reset();
	}
}

const Array<Keyboard>& Input::GetKeyboards() const
{
	return m_keyboards;
}

const Array<Mouse>& Input::GetMouses() const
{
	return m_mouses;
}

const Array<GameController>& Input::GetControllers() const
{
	return m_gameControllers;
}

const InputDevice* Input::GetInputDeviceByHandle(const void* pHandle) const
{
	for(const Keyboard& device : m_keyboards)
	{
		if(device.GetHandle() == pHandle)
		{
			return &device;
		}
	}

	for(const Mouse& device : m_mouses)
	{
		if(device.GetHandle() == pHandle)
		{
			return &device;
		}
	}

	for(const GameController& device : m_gameControllers)
	{
		if(device.GetHandle() == pHandle)
		{
			return &device;
		}
	}

	return nullptr;
}

bool Input::IsKeyDown(EKeyCode keyCode, EKeyModifier modifier) const
{
	for(const Keyboard& keyboard : m_keyboards)
	{
		if(keyboard.IsDown(keyCode) && (modifier == EKeyModifier::DontCare || modifier == keyboard.GetActiveKeyModifier()))
		{
			return true;
		}
	}

	return false;
}

bool Input::WasKeyPressed(EKeyCode keyCode, EKeyModifier modifier) const
{
	for(const Keyboard& keyboard : m_keyboards)
	{
		if(keyboard.WasPressed(keyCode) && (modifier == EKeyModifier::DontCare || modifier == keyboard.GetActiveKeyModifier()))
		{
			return true;
		}
	}

	return false;
}

const String& Input::GetTypingCharacters() const
{
	return m_typingCharacters;
}

bool Input::IsMouseButtonDown(Mouse::EButton button) const
{
	for(const Mouse& mouse : m_mouses)
	{
		if(mouse.IsDown(button))
		{
			return true;
		}
	}

	return false;
}

bool Input::WasMouseButtonPressed(Mouse::EButton button) const
{
	for(const Mouse& mouse : m_mouses)
	{
		if(mouse.WasPressed(button))
		{
			return true;
		}
	}

	return false;
}

float Input::GetMouseAxisDelta(Mouse::EAxis axis) const
{
	float delta = 0.0f;
	for(const Mouse& mouse : m_mouses)
	{
		delta += mouse.GetAxisDelta(axis);
	}

	return delta;
}

const Vector2& Input::GetOsMouseCursorPos() const
{
	return m_mouseCursorPos;
}

bool Input::IsOsMouseCursorClipped() const
{
	return m_isOsMouseCursorClipped;
}

bool Input::IsOsMouseCursorVisible() const
{
	return m_isOsMouseCursorVisible;
}

Keyboard& Input::GetKeyboardByHandle(void* pHandle)
{
	for(Keyboard& keyboard : m_keyboards)
	{
		if(keyboard.GetHandle() == pHandle)
		{
			return keyboard;
		}
	}

	// New keyboard found
	m_keyboards.Add(Keyboard(pHandle));
	return m_keyboards.GetLast();
}

Mouse& Input::GetMouseByHandle(void* pHandle)
{
	for(Mouse& mouse : m_mouses)
	{
		if(mouse.GetHandle() == pHandle)
		{
			return mouse;
		}
	}

	// New mouse found
	m_mouses.Add(Mouse(pHandle));
	return m_mouses.GetLast();
}
