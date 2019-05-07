#pragma once

#include <memory>

class String;
class WritableString;

class OSWindow
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
		static constexpr int kPosCenteredOnScreen = INT32_MAX;

		int posX;
		int posY;
		uint16_t width;
		uint16_t height;
		bool isFullscreen;

		CreationParams()
			: posX(kPosCenteredOnScreen)
			, posY(kPosCenteredOnScreen)
			, width(1366)
			, height(768)
			, isFullscreen(false)
		{

		}
	};

	OSWindow(const CreationParams& creationParams);
	virtual ~OSWindow();

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
	virtual void GetClipboardString(WritableString& outString) = 0;

	// Create a new window
	static std::unique_ptr<OSWindow> Create(const CreationParams& creationParams);

protected:
	void SetSize(uint16_t width, uint16_t height);

	static constexpr uint16_t kWindowMinSize = 100;

	uint16_t width;
	uint16_t height;
	bool isFullscreen;
	bool isActive;
};
