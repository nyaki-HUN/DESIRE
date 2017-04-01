#pragma once

#include "Core/Singleton.h"
#include "Core/math/SPoint.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

#include <vector>

class InputImpl;
class IWindow;
class GameController;

class Input
{
	friend class InputImpl;
	DESIRE_DECLARE_SINGLETON(Input)

public:
	static const int MAX_NUM_TYPING_CHARACTERS = 8;
	static const uint8_t BUTTON_STATE_DOWN_FLAG = 0x80;

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
	const std::vector<GameController*>& GetControllers() const;

	// Returns a string from the characters typed since the last frame (maximized by MAX_NUM_TYPING_CHARACTERS)
	const char* GetTypingCharacters() const;

	const SPoint<int16_t>& GetOsMouseCursorPos() const;

private:
	struct SHotkey
	{
		EKeyCode keyCode;
		uint8_t modifiers;
		HotkeyCallback_t callback;
		void *userData;
	};

	std::vector<SHotkey> hotkeys;

	std::vector<Keyboard> keyboards;
	std::vector<Mouse> mouses;
	std::vector<GameController*> controllers;

	char typingCharacters[MAX_NUM_TYPING_CHARACTERS];

	SPoint<int16_t> mouseCursorPos;
};
