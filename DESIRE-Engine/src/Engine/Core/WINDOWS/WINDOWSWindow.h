#pragma once

#include "Engine/Core/IWindow.h"
#include "Engine/Core/WINDOWS/os.h"

#include <map>

class WINDOWSWindow : public IWindow
{
public:
	typedef void(*MessageHandler_t)(WPARAM wParam, LPARAM lParam);

	WINDOWSWindow(const IWindow::CreationParams& creationParams);
	~WINDOWSWindow() override;

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetWindowTitle(const char *newTitle) override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const char *str) override;
	String GetClipboardString() override;

	void RegisterMessageHandler(UINT msgType, MessageHandler_t messageHandler);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND hWnd;
	HCURSOR cursors[NUM_CURSORS];

	std::map<UINT, MessageHandler_t> additionalMessageHandlers;
	bool isInSizeMove;
};
