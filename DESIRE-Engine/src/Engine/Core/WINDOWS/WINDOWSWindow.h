#pragma once

#include "Engine/Application/OSWindow.h"
#include "Engine/Core/WINDOWS/os.h"

#include <map>

class WINDOWSWindow : public OSWindow
{
public:
	typedef void(*MessageHandler_t)(WPARAM wParam, LPARAM lParam);

	WINDOWSWindow(const OSWindow::CreationParams& creationParams);
	~WINDOWSWindow() override;

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetWindowTitle(const char *newTitle) override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const String& string) override;
	void GetClipboardString(WritableString& outString) override;

	void RegisterMessageHandler(UINT msgType, MessageHandler_t messageHandler);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND hWnd;
	HCURSOR cursors[NUM_CURSORS];

	std::map<UINT, MessageHandler_t> additionalMessageHandlers;
	bool isInSizeMove;
};
