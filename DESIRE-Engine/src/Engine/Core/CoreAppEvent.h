#pragma once

#include "Engine/Core/EAppEventType.h"

class CoreAppEvent
{
public:
	CoreAppEvent(EAppEventType eventType)
		: eventType(eventType)
	{

	}

	EAppEventType eventType;
};

// --------------------------------------------------------------------------------------------------------------------

class NotificationEvent : public CoreAppEvent
{
public:
	enum ENotificationType
	{
		NOTIFICATION_REMOTE,
		NOTIFICATION_LOCAL
	};

	NotificationEvent(ENotificationType notifType, const char *message)
		: CoreAppEvent(EAppEventType::NOTIFICATION)
		, notifType(notifType)
		, message(message)
	{

	}

	ENotificationType notifType;
	const char *message;
};

// --------------------------------------------------------------------------------------------------------------------

class KeyboardWillShowEvent : public CoreAppEvent
{
public:
	KeyboardWillShowEvent(float width, float height)
		: CoreAppEvent(EAppEventType::KEYBOARD_WILL_SHOW)
		, width(width)
		, height(height)
	{

	}

	float width;
	float height;
};
