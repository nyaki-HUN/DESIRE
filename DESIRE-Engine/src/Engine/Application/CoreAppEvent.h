#pragma once

#include "Engine/Application/EAppEventType.h"

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
		: CoreAppEvent(EAppEventType::Notification)
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
		: CoreAppEvent(EAppEventType::KeyboardWillShow)
		, width(width)
		, height(height)
	{

	}

	float width;
	float height;
};
