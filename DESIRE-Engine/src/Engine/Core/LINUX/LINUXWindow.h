#pragma once

#include "Engine/Application/OSWindow.h"

#include <X11/Xlib.h>
#include <map>

class LINUXWindow : public OSWindow
{
public:
	typedef void(*MessageHandler_t)(const XEvent& event);

	LINUXWindow(const OSWindowCreationParams& creationParams);
	~LINUXWindow() override;

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetWindowTitle(const char *newTitle) override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const String& string) override;
	void GetClipboardString(WritableString& outString) override;

	void RegisterMessageHandler(int msgType, MessageHandler_t messageHandler);

private:
	Display *display;
	Window windowHandle;
	Cursor cursors[NUM_CURSORS];

	std::map<int, MessageHandler_t> additionalMessageHandlers;
};
