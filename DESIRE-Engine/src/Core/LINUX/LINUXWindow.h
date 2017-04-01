#pragma once

#include "Core/IWindow.h"

#include <X11/Xlib.h>

class LINUXWindow : public IWindow
{
public:
	LINUXWindow(const IWindow::SCreationParams& creationParams);
	~LINUXWindow();

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetWindowTitle(const char *newTitle) override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const char *str) override;
	String GetClipboardString() override;

private:
	Display *display;
	Window windowHandle;
	Cursor cursors[NUM_CURSORS];
};
