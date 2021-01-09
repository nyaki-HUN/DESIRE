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
	Input();
	~Input();

	void Init(OSWindow& window);
	void Kill();
	void Update();
	void Reset();

	const Array<Keyboard>& GetKeyboards() const;
	const Array<Mouse>& GetMouses() const;
	const Array<GameController>& GetControllers() const;

	const InputDevice* GetInputDeviceByHandle(const void* pHandle) const;

	// Keyboard
	bool IsKeyDown(EKeyCode keyCode, EKeyModifier modifierType = EKeyModifier::None) const;
	bool WasKeyPressed(EKeyCode keyCode, EKeyModifier modifierType = EKeyModifier::None) const;

	// Returns a string from the characters typed since the last frame
	const String& GetTypingCharacters() const;

	// Mouse
	bool IsMouseButtonDown(Mouse::EButton button) const;
	bool WasMouseButtonPressed(Mouse::EButton button) const;
	float GetMouseAxisDelta(Mouse::EAxis axis) const;

	const Vector2& GetOsMouseCursorPos() const;

	void SetOsMouseCursorClipped(bool isClipped);
	bool IsOsMouseCursorClipped() const;

	void SetOsMouseCursorVisible(bool isVisible);
	bool IsOsMouseCursorVisible() const;

private:
	Keyboard& GetKeyboardByHandle(void* pHandle);
	Mouse& GetMouseByHandle(void* pHandle);

	void Init_internal(OSWindow& window);
	void Kill_internal();
	void Update_internal();

	Array<Keyboard> m_keyboards;
	Array<Mouse> m_mouses;
	Array<GameController> m_gameControllers;

	static constexpr size_t kMaxNumTypingCharacters = 8;
	StackString<kMaxNumTypingCharacters> m_typingCharacters;

	Vector2 m_mouseCursorPos = Vector2::Zero();
	bool m_isOsMouseCursorClipped = false;
	bool m_isOsMouseCursorVisible = true;

	friend class InputImpl;
};
