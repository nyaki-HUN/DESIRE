#pragma once

#include "Engine/Core/String.h"

#include <stdint.h>

class IWindow
{
public:
	enum ECursor
	{
		CURSOR_ARROW,
		CURSOR_MOVE,
		CURSOR_SIZE_BOTTOMLEFT,
		CURSOR_SIZE_BOTTOMRIGHT,
		CURSOR_SIZE_NS,
		CURSOR_SIZE_WE,
		CURSOR_HAND,
		CURSOR_IBEAM,
		CURSOR_UP,
		NUM_CURSORS
	};

	struct CreationParams
	{
		static const int POS_CENTERED_ON_SCREEN = INT32_MAX;

		CreationParams()
			: posX(POS_CENTERED_ON_SCREEN)
			, posY(POS_CENTERED_ON_SCREEN)
			, width(1366)
			, height(768)
			, isFullscreen(false)
		{

		}

		int posX;
		int posY;
		uint16_t width;
		uint16_t height;
		bool isFullscreen;
	};

	IWindow(const CreationParams& creationParams);
	virtual ~IWindow();

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;
	bool IsFullscreen() const;
	bool IsActive() const;

	virtual void HandleWindowMessages() = 0;
	virtual void* GetHandle() const = 0;

	virtual void SetWindowTitle(const char *newTitle) = 0;

	// Change the OS cursor
	virtual void SetCursor(ECursor cursor) = 0;

	// Clipboard
	virtual bool SetClipboardString(const String& string) = 0;
	virtual String GetClipboardString() = 0;

	// Create a new IWindow
	static IWindow* Create(const CreationParams& creationParams);

protected:
	void SetSize(uint16_t width, uint16_t height);

	static const uint16_t WINDOW_MIN_SIZE = 100u;

	uint16_t width;
	uint16_t height;
	bool isFullscreen;
	bool isActive;
};
