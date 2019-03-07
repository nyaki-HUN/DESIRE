#include "Engine/stdafx.h"
#include "Engine/Core/WINDOWS/WINDOWSWindow.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/EAppEventType.h"
#include "Engine/Core/Modules.h"

WINDOWSWindow::WINDOWSWindow(const IWindow::CreationParams& creationParams)
	: IWindow(creationParams)
	, hWnd(0)
	, isInSizeMove(false)
{
	for(int i = 0; i < NUM_CURSORS; ++i)
	{
		cursors[i] = nullptr;
	}

	int posX = 0;
	int posY = 0;
	DWORD windowStyleFlags = 0;
	RECT rect = { 0, 0, width, height };
	if(isFullscreen)
	{
		windowStyleFlags = WS_POPUP;
	}
	else
	{
		bool resizable = true;
		if(resizable)
		{
			windowStyleFlags = WS_OVERLAPPEDWINDOW;
		}
		else
		{
			windowStyleFlags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		}

		// Calculate position and dimensions
		AdjustWindowRect(&rect, windowStyleFlags, FALSE);
		long w = rect.right - rect.left;
		long h = rect.bottom - rect.top;

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		if(w > screenWidth)
		{
			rect.left = 0;
			rect.right = screenWidth;
			w = screenWidth;
		}
		if(h > screenHeight)
		{
			rect.top = 0;
			rect.bottom = screenHeight;
			h = screenHeight;
		}

		posX = (creationParams.posX != CreationParams::POS_CENTERED_ON_SCREEN) ? creationParams.posX : (screenWidth - w) / 2;
		posY = (creationParams.posY != CreationParams::POS_CENTERED_ON_SCREEN) ? creationParams.posY : (screenHeight - h) / 2;
	}

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),
		0,
		WndProc,
		0, 0,
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(101)),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,	// (HBRUSH)GetStockObject(BLACK_BRUSH),
		nullptr,
		"DESIRE_Wnd",
		LoadIcon(hInstance, MAKEINTRESOURCE(101)),
	};
	RegisterClassEx(&wc);

	hWnd = CreateWindowExA(0, "DESIRE_Wnd", "DESIRE", windowStyleFlags, posX, posY, rect.right - rect.left, rect.bottom - rect.top, GetDesktopWindow(), nullptr, hInstance, this);

	ShowWindow(hWnd, SW_SHOW);
}

WINDOWSWindow::~WINDOWSWindow()
{
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)nullptr);
}

void WINDOWSWindow::HandleWindowMessages()
{
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void* WINDOWSWindow::GetHandle() const
{
	return hWnd;
}

void WINDOWSWindow::SetWindowTitle(const char *newTitle)
{
	SetWindowText(hWnd, newTitle);
}

void WINDOWSWindow::SetCursor(ECursor cursor)
{
	if(cursors[cursor] == nullptr)
	{
		switch(cursor)
		{
			case CURSOR_ARROW:				cursors[cursor] = LoadCursor(NULL, IDC_ARROW); break;
			case CURSOR_MOVE:				cursors[cursor] = LoadCursor(NULL, IDC_SIZEALL); break;
			case CURSOR_SIZE_BOTTOMLEFT:	cursors[cursor] = LoadCursor(NULL, IDC_SIZENESW); break;
			case CURSOR_SIZE_BOTTOMRIGHT:	cursors[cursor] = LoadCursor(NULL, IDC_SIZENWSE); break;
			case CURSOR_SIZE_NS:			cursors[cursor] = LoadCursor(NULL, IDC_SIZENS); break;
			case CURSOR_SIZE_WE:			cursors[cursor] = LoadCursor(NULL, IDC_SIZEWE); break;
			case CURSOR_HAND:				cursors[cursor] = LoadCursor(NULL, IDC_HAND); break;
			case CURSOR_IBEAM:				cursors[cursor] = LoadCursor(NULL, IDC_IBEAM); break;
			case CURSOR_UP:					cursors[cursor] = LoadCursor(NULL, IDC_UPARROW); break;
			case NUM_CURSORS:				ASSERT(false); return;
		}
	}

	CURSORINFO ci;
	memset(&ci, 0, sizeof(ci));
	ci.cbSize = sizeof(ci);
	BOOL ok = ::GetCursorInfo(&ci);
	if(ok && (ci.flags & CURSOR_SHOWING))
	{
		::SetCursor(cursors[cursor]);
	}
}

bool WINDOWSWindow::SetClipboardString(const String& string)
{
	const size_t size = string.Length() + 1;
	HANDLE stringHandle = GlobalAlloc(GMEM_MOVEABLE, size);
	if(stringHandle == nullptr)
	{
		return false;
	}

	void *ptr = GlobalLock(stringHandle);
	if(ptr == nullptr)
	{
		return false;
	}

	memcpy(ptr, string.Str(), size);
	GlobalUnlock(stringHandle);

	if(!OpenClipboard(hWnd))
	{
		GlobalFree(stringHandle);
		return false;
	}
	EmptyClipboard();
	SetClipboardData(CF_TEXT, stringHandle);
	CloseClipboard();
	return true;
}

DynamicString WINDOWSWindow::GetClipboardString()
{
	DynamicString str;

	if(IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(hWnd))
	{
		HGLOBAL stringHandle = GetClipboardData(CF_TEXT);
		if(stringHandle != nullptr)
		{
			char *ptr = (char*)GlobalLock(stringHandle);
			if(ptr != nullptr)
			{
				str = DynamicString(ptr, strlen(ptr));
				GlobalUnlock(stringHandle);
			}
		}
		CloseClipboard();
	}

	return str;
}

void WINDOWSWindow::RegisterMessageHandler(UINT msgType, MessageHandler_t messageHandler)
{
	ASSERT(messageHandler != nullptr);
	additionalMessageHandlers[msgType] = messageHandler;
}

LRESULT CALLBACK WINDOWSWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_CREATE)
	{
		// Store pointer to the WINDOWSWindow in user data area
		CREATESTRUCT *createStruct = (CREATESTRUCT*)lParam;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)createStruct->lpCreateParams);
		return 0;
	}

	// Get the window instance
	WINDOWSWindow *window = reinterpret_cast<WINDOWSWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if(window == nullptr)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	if(window->isActive)
	{
		auto it = window->additionalMessageHandlers.find(msg);
		if(it != window->additionalMessageHandlers.end())
		{
			it->second(wParam, lParam);
		}
	}

	switch(msg)
	{
		case WM_MOUSEMOVE:			// Posted to a window when the cursor moves
			return 0;

		case WM_CHAR:				// Posted to the window when a WM_KEYDOWN message is translated by TranslateMessage()
			return 0;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			// Alt + F4 should be enabled to close the window
			if(wParam == VK_F4 && lParam & 1 << 29)
			{
				break;
			}
			// Do not process the system keys since they would activate the context menu of the window or the main menu
			return 0;

/*		case WM_SETCURSOR:
			SetCursor(nullptr);
			return 1;
*/
		case WM_ACTIVATE:			// Sent to both the window being activated and the window being deactivated
			if(wParam == WA_INACTIVE)
			{
				window->isActive = false;
				Modules::Application->SendEvent(EAppEventType::EnterBackground);
			}
			else if(wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
			{
				window->isActive = true;
				Modules::Application->SendEvent(EAppEventType::EnterForeground);
			}
			break;

		case WM_ENTERSIZEMOVE:		// Sent one time to a window after it enters the moving or sizing modal loop
			window->isInSizeMove = true;
			return 0;

		case WM_EXITSIZEMOVE:		// Sent one time to a window, after it has exited the moving or sizing modal loop
		{
			window->isInSizeMove = false;

			WINDOWINFO pwi;
			GetWindowInfo(hWnd, &pwi);
			window->SetSize((uint16_t)(pwi.rcClient.right - pwi.rcClient.left), (uint16_t)(pwi.rcClient.bottom - pwi.rcClient.top));
			return 0;
		}

		case WM_SIZE:				// Sent to a window after its size has changed
			if(!window->isInSizeMove && wParam != SIZE_MINIMIZED)
			{
				if(wParam == SIZE_MAXIMIZED)
				{
//					window->SetPosition(0, 0);
				}
				window->SetSize((uint16_t)lParam, (uint16_t)(lParam >> 16));
			}
			return 0;

		case WM_DISPLAYCHANGE:		// Sent to all windows when the display resolution has changed
			break;

		case WM_GETMINMAXINFO:		// When the size or position of the window is about to change
			// Prevent the window from going smaller than some minimum size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = WINDOW_MIN_SIZE;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = WINDOW_MIN_SIZE;
			break;

		case WM_SYSCOMMAND:
			switch(wParam)
			{
				case SC_MOVE:
				case SC_SIZE:
				case SC_KEYMENU:
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;
			}
			break;

		case WM_QUIT:				// Generated when the application calls the PostQuitMessage() function
		case WM_CLOSE:				// Sent as a signal that a window or an application should terminate
			Application::Stop();
			return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// --------------------------------------------------------------------------------------------------------------------
//	IWindow
// --------------------------------------------------------------------------------------------------------------------

std::unique_ptr<IWindow> IWindow::Create(const IWindow::CreationParams& creationParams)
{
	return std::make_unique<WINDOWSWindow>(creationParams);
}
