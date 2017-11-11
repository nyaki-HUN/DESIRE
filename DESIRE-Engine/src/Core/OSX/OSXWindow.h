#pragma once

#include "Core/IWindow.h"

#include <AppKit/NSCursor.h>

class OSXWindow : public IWindow
{
public:
	OSXWindow(const IWindow::CreationParams& creationParams);
	~OSXWindow();

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetWindowTitle(const char *newTitle) override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const char *str) override;
	String GetClipboardString() override;

private:
	NSCursor *cursors[NUM_CURSORS];
};
