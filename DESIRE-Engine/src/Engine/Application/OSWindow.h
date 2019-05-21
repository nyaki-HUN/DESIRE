#pragma once

#include <map>
#include <memory>

class OSWindowImpl;
class String;
class WritableString;
struct OSWindowCreationParams;

class OSWindow
{
public:
	typedef void(*MessageHandler_t)(const void *);

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

	OSWindow(const OSWindowCreationParams& creationParams);
	~OSWindow();

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;
	bool IsFullscreen() const;
	bool IsActive() const;

	void HandleWindowMessages();
	void* GetHandle() const;

	void RegisterMessageHandler(int msgType, MessageHandler_t messageHandler);

	void SetWindowTitle(const char *newTitle);

	// Change the OS cursor
	void SetCursor(ECursor cursor);

	// Clipboard
	bool SetClipboardString(const String& string);
	void GetClipboardString(WritableString& outString);

private:
	void SetSize(uint16_t width, uint16_t height);

	static constexpr uint16_t kWindowMinSize = 100;

	uint16_t width;
	uint16_t height;
	bool isFullscreen = false;
	bool isActive = false;
	std::map<int, MessageHandler_t> additionalMessageHandlers;
	std::unique_ptr<OSWindowImpl> impl;

	friend class OSWindowImpl;
};
