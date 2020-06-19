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

	// Keyboard
	for(Keyboard& keyboard : keyboards)
	{
		// Handle hotkeys
		const uint8_t modifiers = keyboard.GetModifierMask();
		for(const Hotkey& hotkey : hotkeys)
		{
			if(keyboard.WentDown(hotkey.keyCode) && (hotkey.modifiers == MODIFIER_DONT_CARE || hotkey.modifiers == modifiers))
			{
				hotkey.callback(hotkey.userData);
			}
		}

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

bool Input::RegisterHotkey(EKeyCode keyCode, uint8_t modifiers, HotkeyCallback_t callback, void* userData)
{
	ASSERT(callback != nullptr);

	for(const Hotkey& hotkey : hotkeys)
	{
		if(hotkey.keyCode == keyCode && hotkey.modifiers == modifiers)
		{
			return false;
		}
	}

	Hotkey hotkey;
	hotkey.keyCode = keyCode;
	hotkey.modifiers = modifiers;
	hotkey.callback = callback;
	hotkey.userData = userData;
	hotkeys.Add(hotkey);

	return true;
}

void Input::UnregisterHotkey(EKeyCode keyCode, uint8_t modifiers)
{
	const size_t idx = hotkeys.SpecializedFind([keyCode, modifiers](const Hotkey& hotkey)
	{
		return (hotkey.keyCode == keyCode && hotkey.modifiers == modifiers);
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

const String& Input::GetTypingCharacters() const
{
	return typingCharacters;
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
