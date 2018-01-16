#pragma once

enum class EAppEventType
{
	WILL_RESIGN_ACTIVE,
	SET_BECAME_ACTIVE,
	ENTER_BACKGROUND,
	ENTER_FOREGROUND,
	LOW_MEMORY,
	NOTIFICATION,				// class NotificationEvent
	KEYBOARD_WILL_SHOW,			// class KeyboardWillShowEvent
	KEYBOARD_WILL_HIDE,
};
