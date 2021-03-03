#pragma once

#include <map>

class String;
class WritableString;

struct OSWindowCreationParams;
struct OSWindowImpl;

class OSWindow
{
public:
	typedef void(*MessageHandler_t)(const void*);

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
		CURSOR_NOT_ALLOWED,
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

	void RegisterMessageHandler(int32_t msgType, MessageHandler_t messageHandler);

	void SetWindowTitle(const String& newTitle);

	// Change the OS cursor
	void SetCursor(ECursor cursor);

	// Clipboard
	bool SetClipboardString(const String& string);
	void GetClipboardString(WritableString& outString) const;

private:
	void SetSize(uint16_t width, uint16_t height);

	static constexpr uint16_t kWindowMinSize = 100;

	uint16_t m_width;
	uint16_t m_height;
	bool m_isFullscreen = false;
	bool m_isActive = false;
	std::map<int32_t, MessageHandler_t> m_additionalMessageHandlers;
	std::unique_ptr<OSWindowImpl> m_spImpl;

	friend OSWindowImpl;
};
