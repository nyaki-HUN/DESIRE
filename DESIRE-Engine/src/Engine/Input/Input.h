#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/math/Vector2.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Input/Mouse.h"
#include "Engine/Input/GameController.h"

class IWindow;

class Input
{
	friend class InputImpl;

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

	Input();
	~Input();

	static void Init(IWindow *window);
	static void Kill();

	void Update();
	void Reset();

	bool RegisterHotkey(EKeyCode keyCode, uint8_t modifiers, HotkeyCallback_t callback, void *userData = nullptr);
	void UnregisterHotkey(EKeyCode keyCode, uint8_t modifiers);

	const Array<Keyboard>& GetKeyboards() const;
	const Array<Mouse>& GetMouses() const;
	const Array<GameController>& GetControllers() const;

	const InputDevice* GetInputDeviceByHandle(const void *handle) const;

	// Returns a string from the characters typed since the last frame (maximized by MAX_NUM_TYPING_CHARACTERS)
	const char* GetTypingCharacters() const;

	const Vector2& GetOsMouseCursorPos() const;

private:
	Keyboard& GetKeyboardByHandle(void *handle);
	Mouse& GetMouseByHandle(void *handle);

	struct Hotkey
	{
		EKeyCode keyCode;
		uint8_t modifiers;
		HotkeyCallback_t callback;
		void *userData;
	};

	Array<Hotkey> hotkeys;

	Array<Keyboard> keyboards;
	Array<Mouse> mouses;
	Array<GameController> gameControllers;

	char typingCharacters[MAX_NUM_TYPING_CHARACTERS];

	Vector2 mouseCursorPos;
};
