#pragma once

#include "Core/Singleton.h"
#include "Core/math/Point.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GameController.h"

#include <vector>

class InputImpl;
class IWindow;

class Input
{
	friend class InputImpl;
	DESIRE_SINGLETON(Input)

public:
	static const int MAX_NUM_TYPING_CHARACTERS = 8;

	typedef void(*HotkeyCallback_t)(void *userData);

	enum EModifierType
	{
		MODIFIER_NONE		= 0x00,
		MODIFIER_CTRL		= 0x01,
		MODIFIER_SHIFT		= 0x02,
		MODIFIER_ALT		= 0x04,
		MODIFIER_COMMAND	= 0x08,
		MODIFIER_DONT_CARE	= 0xff,
	};

	static void Init(IWindow *window);
	static void Kill();

	void Update();

	bool RegisterHotkey(EKeyCode keyCode, uint8_t modifiers, HotkeyCallback_t callback, void *userData = nullptr);
	void UnregisterHotkey(EKeyCode keyCode, uint8_t modifiers);

	const std::vector<Keyboard>& GetKeyboards() const;
	const std::vector<Mouse>& GetMouses() const;
	const std::vector<GameController>& GetControllers() const;

	const InputDevice* GetInputDeviceByHandle(const void *handle) const;

	// Returns a string from the characters typed since the last frame (maximized by MAX_NUM_TYPING_CHARACTERS)
	const char* GetTypingCharacters() const;

	const Point<int16_t>& GetOsMouseCursorPos() const;

private:
	struct Hotkey
	{
		EKeyCode keyCode;
		uint8_t modifiers;
		HotkeyCallback_t callback;
		void *userData;
	};

	std::vector<Hotkey> hotkeys;

	std::vector<Keyboard> keyboards;
	std::vector<Mouse> mouses;
	std::vector<GameController> gameControllers;

	char typingCharacters[MAX_NUM_TYPING_CHARACTERS];

	Point<int16_t> mouseCursorPos;
};
