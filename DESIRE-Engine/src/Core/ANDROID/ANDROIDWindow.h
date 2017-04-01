#pragma once

#include "Core/IWindow.h"

struct android_app;
struct ANativeWindow;

class ANDROIDWindow : public IWindow
{
public:
	ANDROIDWindow(const IWindow::SCreationParams& creationParams, ANativeWindow *nativeWindow);
	~ANDROIDWindow();

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const char *str) override;
	String GetClipboardString() override;

	static void HandleOnAppCmd(android_app *androidApp, int32_t cmd);

private:
	ANativeWindow *nativeWindow;
};
