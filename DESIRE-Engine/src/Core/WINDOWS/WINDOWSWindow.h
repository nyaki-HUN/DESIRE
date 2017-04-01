#pragma once

#include "Core/IWindow.h"
#include "os.h"

class WINDOWSWindow : public IWindow
{
public:
	typedef void(*MessageHandler_t)(WPARAM wParam, LPARAM lParam);

	WINDOWSWindow(const IWindow::SCreationParams& creationParams);
	~WINDOWSWindow();

	void HandleWindowMessages() override;
	void* GetHandle() const override;

	void SetWindowTitle(const char *newTitle) override;

	void SetCursor(ECursor cursor) override;

	bool SetClipboardString(const char *str) override;
	String GetClipboardString() override;

	void RegisterMessageHandler(uint32_t msgType, MessageHandler_t messageHandler);

private:
	void Activated();
	void Deactivated();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND hWnd;
	HCURSOR cursors[NUM_CURSORS];

	MessageHandler_t messageHandler_WM_INPUT;
	MessageHandler_t messageHandler_WM_MOUSEMOVE;
	MessageHandler_t messageHandler_WM_CHAR;

	bool isInSizeMove;
};
