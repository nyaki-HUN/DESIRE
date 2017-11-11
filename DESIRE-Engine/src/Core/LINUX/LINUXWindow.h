#pragma once

#include "Core/IWindow.h"

#include <X11/Xlib.h>
#include <map>

class LINUXWindow : public IWindow
{
public:
	typedef void(*MessageHandler_t)(const XEvent& event);

	LINUXWindow(const IWindow::CreationParams& creationParams);
	~LINUXWindow();

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetWindowTitle(const char *newTitle) override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const char *str) override;
	String GetClipboardString() override;

	void RegisterMessageHandler(int msgType, MessageHandler_t messageHandler);

private:
	Display *display;
	Window windowHandle;
	Cursor cursors[NUM_CURSORS];

	std::map<int, MessageHandler_t> additionalMessageHandlers;
};
