#pragma once

#include "Core/IWindow.h"

class IOSWindow : public IWindow
{
public:
	IOSWindow(const IWindow::CreationParams& creationParams);
	~IOSWindow() override;

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const char *str) override;
	String GetClipboardString() override;

private:
};