#include "Engine/stdafx.h"
#include "Engine/Input/Input.h"

Input::Input()
{
}

Input::~Input()
{
}

void Input::Init(OSWindow* window)
{
	Init_internal(window);
}

void Input::Kill()
{
	Kill_internal();

	// Reset input devices
	keyboards.Clear();
	mouses.Clear();
	gameControllers.Clear();
}

void Input::Update()
{
	Update_internal();

	// Handle hotkeys
	for(const Hotkey& hotkey : hotkeys)
	{
		if(WasKeyPressed(hotkey.keyCode, hotkey.modifier))
		{
			hotkey.callback(hotkey.userData);
		}
	}

	// Keyboard
	for(Keyboard& keyboard : keyboards)
	{
		keyboard.Update();
	}

	// Mouse
	for(Mouse& mouse : mouses)
	{
		mouse.Update();
	}

	// Game Controller
	for(GameController& gamepad : gameControllers)
	{
		gamepad.Update();
	}

	typingCharacters.Clear();
}

void Input::Reset()
{
	// Keyboard
	for(Keyboard& keyboard : keyboards)
	{
		keyboard.Reset();
	}

	// Mouse
	for(Mouse& mouse : mouses)
	{
		mouse.Reset();
	}

	// Game Controller
	for(GameController& gamepad : gameControllers)
	{
		gamepad.Reset();
	}
}

bool Input::RegisterHotkey(EKeyCode keyCode, EKeyModifier modifier, HotkeyCallback_t callback, void* userData)
{
	ASSERT(callback != nullptr);

	for(const Hotkey& hotkey : hotkeys)
	{
		if(hotkey.keyCode == keyCode && hotkey.modifier == modifier)
		{
			return false;
		}
	}

	Hotkey hotkey;
	hotkey.keyCode = keyCode;
	hotkey.modifier = modifier;
	hotkey.callback = callback;
	hotkey.userData = userData;
	hotkeys.Add(hotkey);

	return true;
}

void Input::UnregisterHotkey(EKeyCode keyCode, EKeyModifier modifier)
{
	const size_t idx = hotkeys.SpecializedFind([keyCode, modifier](const Hotkey& hotkey)
	{
		return (hotkey.keyCode == keyCode && hotkey.modifier == modifier);
	});

	if(idx != SIZE_MAX)
	{
		hotkeys.RemoveFastAt(idx);
	}
}

const Array<Keyboard>& Input::GetKeyboards() const
{
	return keyboards;
}

const Array<Mouse>& Input::GetMouses() const
{
	return mouses;
}

const Array<GameController>& Input::GetControllers() const
{
	return gameControllers;
}

const InputDevice* Input::GetInputDeviceByHandle(const void* handle) const
{
	for(const Keyboard& device : keyboards)
	{
		if(device.handle == handle)
		{
			return &device;
		}
	}

	for(const Mouse& device : mouses)
	{
		if(device.handle == handle)
		{
			return &device;
		}
	}

	for(const GameController& device : gameControllers)
	{
		if(device.handle == handle)
		{
			return &device;
		}
	}

	return nullptr;
}

bool Input::IsKeyDown(EKeyCode keyCode, EKeyModifier modifier) const
{
	for(const Keyboard& keyboard : keyboards)
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
	for(const Keyboard& keyboard : keyboards)
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
	return typingCharacters;
}

bool Input::IsMouseButtonDown(Mouse::EButton button) const
{
	for(const Mouse& mouse : mouses)
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
	for(const Mouse& mouse : mouses)
	{
		if(mouse.WasPressed(button))
		{
			return true;
		}
	}

	return false;
}

const Vector2& Input::GetOsMouseCursorPos() const
{
	return mouseCursorPos;
}

Keyboard& Input::GetKeyboardByHandle(void* handle)
{
	for(Keyboard& keyboard : keyboards)
	{
		if(keyboard.handle == handle)
		{
			return keyboard;
		}
	}

	// New keyboard found
	keyboards.Add(Keyboard(handle));
	return keyboards.GetLast();
}

Mouse& Input::GetMouseByHandle(void* handle)
{
	for(Mouse& mouse : mouses)
	{
		if(mouse.handle == handle)
		{
			return mouse;
		}
	}

	// New mouse found
	mouses.Add(Mouse(handle));
	return mouses.GetLast();
}
