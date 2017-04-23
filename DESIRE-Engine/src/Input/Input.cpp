#include "stdafx.h"
#include "Input/Input.h"

Input::Input()
{
	memset(typingCharacters, 0, MAX_NUM_TYPING_CHARACTERS);
}

Input::~Input()
{

}

void Input::Update()
{
	// Keyboard
	for(Keyboard& keyboard : keyboards)
	{
		// Handle hotkeys
		const uint8_t modifiers = keyboard.GetModifierMask();
		for(const SHotkey& hotkey : hotkeys)
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

	memset(typingCharacters, 0, MAX_NUM_TYPING_CHARACTERS);
}

bool Input::RegisterHotkey(EKeyCode keyCode, uint8_t modifiers, HotkeyCallback_t callback, void *userData)
{
	ASSERT(callback != nullptr);

	for(const SHotkey& hotkey : hotkeys)
	{
		if(hotkey.keyCode == keyCode && hotkey.modifiers == modifiers)
		{
			return false;
		}
	}

	SHotkey hotkey;
	hotkey.keyCode = keyCode;
	hotkey.modifiers = modifiers;
	hotkey.callback = callback;
	hotkey.userData = userData;
	hotkeys.push_back(hotkey);

	return true;
}

void Input::UnregisterHotkey(EKeyCode keyCode, uint8_t modifiers)
{
	hotkeys.erase(std::remove_if(hotkeys.begin(), hotkeys.end(), [keyCode, modifiers](const SHotkey& hotkey)
	{
		return (hotkey.keyCode == keyCode && hotkey.modifiers == modifiers);
	}), hotkeys.end());
}

const std::vector<Keyboard>& Input::GetKeyboards() const
{
	return keyboards;
}

const std::vector<Mouse>& Input::GetMouses() const
{
	return mouses;
}

const std::vector<GameController>& Input::GetControllers() const
{
	return gameControllers;
}

const InputDevice* Input::GetInputDeviceByHandle(const void *handle) const
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

const char* Input::GetTypingCharacters() const
{
	return typingCharacters;
}

const SPoint<int16_t>& Input::GetOsMouseCursorPos() const
{
	return mouseCursorPos;
}
