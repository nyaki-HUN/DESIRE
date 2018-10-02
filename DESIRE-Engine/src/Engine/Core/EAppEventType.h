#pragma once

enum class EAppEventType
{
	WillResignActive,
	SetBecameActive,
	EnterBackground,
	EnterForeground,
	LowMemory,
	Notification,				// class NotificationEvent
	KeyboardWillShow,			// class KeyboardWillShowEvent
	KeyboardWillHide,
};
