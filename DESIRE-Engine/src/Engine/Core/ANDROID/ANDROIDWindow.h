#pragma once

#include "Engine/Core/IWindow.h"

struct android_app;
struct ANativeWindow;

class ANDROIDWindow : public IWindow
{
public:
	ANDROIDWindow(const IWindow::CreationParams& creationParams, ANativeWindow *nativeWindow);
	~ANDROIDWindow() override;

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const String& string) override;
	String GetClipboardString() override;

	static void HandleOnAppCmd(android_app *androidApp, int32_t cmd);

private:
	ANativeWindow *nativeWindow;
};
