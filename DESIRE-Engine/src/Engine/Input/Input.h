#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Input/Keyboard.h"
#include "Engine/Input/Mouse.h"
#include "Engine/Input/GameController.h"

class OSWindow;

class Input
{
public:
	typedef void(*HotkeyCallback_t)(void* userData);

	Input();
	~Input();

	void Init(OSWindow* window);
	void Kill();
	void Update();
	void Reset();

	bool RegisterHotkey(EKeyCode keyCode, EKeyModifier modifier, HotkeyCallback_t callback, void* userData = nullptr);
	void UnregisterHotkey(EKeyCode keyCode, EKeyModifier modifier);

	const Array<Keyboard>& GetKeyboards() const;
	const Array<Mouse>& GetMouses() const;
	const Array<GameController>& GetControllers() const;

	const InputDevice* GetInputDeviceByHandle(const void* handle) const;

	// Keyboard
	bool IsKeyDown(EKeyCode keyCode, EKeyModifier modifierType = EKeyModifier::DontCare) const;
	bool WasKeyPressed(EKeyCode keyCode, EKeyModifier modifierType = EKeyModifier::DontCare) const;

	// Returns a string from the characters typed since the last frame
	const String& GetTypingCharacters() const;

	// Mouse
	bool IsMouseButtonDown(Mouse::EButton button) const;
	bool WasMouseButtonPressed(Mouse::EButton button) const;

	const Vector2& GetOsMouseCursorPos() const;

private:
	Keyboard& GetKeyboardByHandle(void* handle);
	Mouse& GetMouseByHandle(void* handle);

	void Init_internal(OSWindow* window);
	void Kill_internal();
	void Update_internal();

	struct Hotkey
	{
		EKeyCode keyCode;
		EKeyModifier modifier;
		HotkeyCallback_t callback;
		void* userData;
	};

	Array<Hotkey> hotkeys;

	Array<Keyboard> keyboards;
	Array<Mouse> mouses;
	Array<GameController> gameControllers;

	static constexpr size_t kMaxNumTypingCharacters = 8;
	StackString<kMaxNumTypingCharacters> typingCharacters;

	Vector2 mouseCursorPos = Vector2::Zero();

	friend class InputImpl;
};
