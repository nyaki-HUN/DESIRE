#include "Engine/stdafx.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Application/Application.h"

#include "Engine/Core/String/WritableString.h"
#include "Engine/Core/WINDOWS/os.h"

// --------------------------------------------------------------------------------------------------------------------
//	OSWindowImpl
// --------------------------------------------------------------------------------------------------------------------

struct OSWindowImpl
{
	HWND hWnd = 0;
	HCURSOR cursors[OSWindow::NUM_CURSORS] = {};
	OSWindow::ECursor currCursor = OSWindow::CURSOR_ARROW;
	bool isInSizeMove = false;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if(msg == WM_CREATE)
		{
			// Store pointer to the WINDOWSWindow in user data area
			CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
			return 0;
		}

		// Get the window instance
		OSWindow* pWindow = reinterpret_cast<OSWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if(pWindow == nullptr)
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		if(pWindow->m_isActive)
		{
			auto iter = pWindow->m_additionalMessageHandlers.find(msg);
			if(iter != pWindow->m_additionalMessageHandlers.end())
			{
				const std::pair<WPARAM, LPARAM> paramPair(wParam, lParam);
				iter->second(&paramPair);
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

			case WM_SETCURSOR:			// Sent to a window if the mouse causes the cursor to move within a window and mouse input is not captured
				if(LOWORD(lParam) == HTCLIENT)
				{
					pWindow->m_spImpl->UpdateCursor();
					return TRUE;
				}
				break;

			case WM_ACTIVATE:			// Sent to both the window being activated and the window being deactivated
				if(wParam == WA_INACTIVE)
				{
					pWindow->m_isActive = false;
					Modules::Application->SendEvent(EAppEventType::Deactivate);
				}
				else if(wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
				{
					pWindow->m_isActive = true;
					Modules::Application->SendEvent(EAppEventType::Activate);
				}
				break;

			case WM_ENTERSIZEMOVE:		// Sent one time to a window after it enters the moving or sizing modal loop
				pWindow->m_spImpl->isInSizeMove = true;
				return 0;

			case WM_EXITSIZEMOVE:		// Sent one time to a window, after it has exited the moving or sizing modal loop
			{
				pWindow->m_spImpl->isInSizeMove = false;

				WINDOWINFO pwi;
				GetWindowInfo(hWnd, &pwi);
				pWindow->SetSize(static_cast<uint16_t>(pwi.rcClient.right - pwi.rcClient.left), static_cast<uint16_t>(pwi.rcClient.bottom - pwi.rcClient.top));
				return 0;
			}

			case WM_SIZE:				// Sent to a window after its size has changed
				if(!pWindow->m_spImpl->isInSizeMove && wParam != SIZE_MINIMIZED)
				{
					if(wParam == SIZE_MAXIMIZED)
					{
//						pWindow->SetPosition(0, 0);
					}
					pWindow->SetSize(static_cast<uint16_t>(lParam), static_cast<uint16_t>(lParam >> 16));
				}
				return 0;

			case WM_DISPLAYCHANGE:		// Sent to all windows when the display resolution has changed
				break;

			case WM_GETMINMAXINFO:		// When the size or position of the window is about to change
			{
				// Prevent the window from going smaller than some minimum size
				MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
				info->ptMinTrackSize.x = OSWindow::kWindowMinSize;
				info->ptMinTrackSize.y = OSWindow::kWindowMinSize;
				break;
			}

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

	void UpdateCursor()
	{
		CURSORINFO ci = {};
		ci.cbSize = sizeof(ci);
		if(GetCursorInfo(&ci) == FALSE || (ci.flags & CURSOR_SHOWING) == 0)
		{
			return;
		}

		if(cursors[currCursor] == nullptr)
		{
			switch(currCursor)
			{
				case OSWindow::CURSOR_ARROW:			cursors[currCursor] = LoadCursor(NULL, IDC_ARROW); break;
				case OSWindow::CURSOR_MOVE:				cursors[currCursor] = LoadCursor(NULL, IDC_SIZEALL); break;
				case OSWindow::CURSOR_SIZE_BOTTOMLEFT:	cursors[currCursor] = LoadCursor(NULL, IDC_SIZENESW); break;
				case OSWindow::CURSOR_SIZE_BOTTOMRIGHT:	cursors[currCursor] = LoadCursor(NULL, IDC_SIZENWSE); break;
				case OSWindow::CURSOR_SIZE_NS:			cursors[currCursor] = LoadCursor(NULL, IDC_SIZENS); break;
				case OSWindow::CURSOR_SIZE_WE:			cursors[currCursor] = LoadCursor(NULL, IDC_SIZEWE); break;
				case OSWindow::CURSOR_HAND:				cursors[currCursor] = LoadCursor(NULL, IDC_HAND); break;
				case OSWindow::CURSOR_IBEAM:			cursors[currCursor] = LoadCursor(NULL, IDC_IBEAM); break;
				case OSWindow::CURSOR_UP:				cursors[currCursor] = LoadCursor(NULL, IDC_UPARROW); break;
				case OSWindow::CURSOR_NOT_ALLOWED:		cursors[currCursor] = LoadCursor(NULL, IDC_NO); break;
				case OSWindow::NUM_CURSORS:				ASSERT(false); return;
			}
		}

		SetCursor(cursors[currCursor]);
	}

	bool IsInsideClientArea()
	{
		POINT screenPos;
		if(GetCursorPos(&screenPos) == FALSE || WindowFromPoint(screenPos) != hWnd)
		{
			return false;
		}

		RECT area;
		GetClientRect(hWnd, &area);
		ScreenToClient(hWnd, &screenPos);

		const BOOL isInside = PtInRect(&area, screenPos);
		return (isInside == TRUE);
	}
};

// --------------------------------------------------------------------------------------------------------------------
//	OSWindow
// --------------------------------------------------------------------------------------------------------------------

OSWindow::OSWindow(const OSWindowCreationParams& creationParams)
	: m_width(std::max(kWindowMinSize, creationParams.width))
	, m_height(std::max(kWindowMinSize, creationParams.height))
	, m_isFullscreen(creationParams.isFullscreen)
	, m_spImpl(std::make_unique<OSWindowImpl>())
{
	int32_t posX = 0;
	int32_t posY = 0;
	DWORD windowStyleFlags = 0;
	RECT rect = { 0, 0, m_width, m_height };
	if(m_isFullscreen)
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

		int32_t screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int32_t screenHeight = GetSystemMetrics(SM_CYSCREEN);
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

		posX = (creationParams.posX != OSWindowCreationParams::kPosCenteredOnScreen) ? creationParams.posX : (screenWidth - w) / 2;
		posY = (creationParams.posY != OSWindowCreationParams::kPosCenteredOnScreen) ? creationParams.posY : (screenHeight - h) / 2;
	}

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = OSWindowImpl::WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = "DESIRE_Wnd";
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(101));
	RegisterClassEx(&wc);

	m_spImpl->hWnd = CreateWindowExA(0, "DESIRE_Wnd", "DESIRE", windowStyleFlags, posX, posY, rect.right - rect.left, rect.bottom - rect.top, GetDesktopWindow(), nullptr, hInstance, this);

	ShowWindow(m_spImpl->hWnd, SW_SHOW);
}

OSWindow::~OSWindow()
{
	SetWindowLongPtr(m_spImpl->hWnd, GWLP_USERDATA, NULL);
}

void OSWindow::HandleWindowMessages()
{
	MSG msg;
	while(PeekMessage(&msg, HWND_TOP, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void* OSWindow::GetHandle() const
{
	return m_spImpl->hWnd;
}

void OSWindow::SetWindowTitle(const String& newTitle)
{
	SetWindowText(m_spImpl->hWnd, newTitle.Str());
}

void OSWindow::SetCursor(ECursor cursor)
{
	if(m_spImpl->currCursor == cursor)
	{
		return;
	}

	m_spImpl->currCursor = cursor;

	if(m_spImpl->IsInsideClientArea())
	{
		m_spImpl->UpdateCursor();
	}
}

bool OSWindow::SetClipboardString(const String& string)
{
	bool success = false;

	if(OpenClipboard(m_spImpl->hWnd))
	{
		EmptyClipboard();

		if(!string.IsEmpty())
		{
			const int32_t wsize = MultiByteToWideChar(CP_UTF8, 0, string.Str(), static_cast<int>(string.Length()), nullptr, 0);
			HGLOBAL stringHandle = GlobalAlloc(GMEM_MOVEABLE, static_cast<SIZE_T>(wsize + 1) * sizeof(wchar_t));
			if(stringHandle != nullptr)
			{
				wchar_t* pWideStr = static_cast<wchar_t*>(GlobalLock(stringHandle));
				if(pWideStr)
				{
					MultiByteToWideChar(CP_UTF8, 0, string.Str(), static_cast<int>(string.Length()), pWideStr, wsize);
					pWideStr[wsize] = '\0';
					GlobalUnlock(stringHandle);

					if(SetClipboardData(CF_UNICODETEXT, stringHandle))
					{
						success = true;
					}
					else
					{
						GlobalFree(stringHandle);
					}
				}
			}
		}

		CloseClipboard();
	}

	return success;
}

void OSWindow::GetClipboardString(WritableString& outString) const
{
	outString.Clear();

	if(OpenClipboard(m_spImpl->hWnd))
	{
		if(IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			HGLOBAL stringHandle = GetClipboardData(CF_UNICODETEXT);
			if(stringHandle != nullptr)
			{
				const wchar_t* pWideStr = static_cast<const wchar_t*>(GlobalLock(stringHandle));
				if(pWideStr)
				{
					int32_t size = WideCharToMultiByte(CP_UTF8, 0, pWideStr, -1, nullptr, 0, nullptr, nullptr);
					char* pStr = outString.AsCharBufferWithSize(size);
					if(pStr)
					{
						WideCharToMultiByte(CP_UTF8, 0, pWideStr, -1, pStr, size, nullptr, nullptr);
					}

					GlobalUnlock(stringHandle);
				}
			}
		}
		else if(IsClipboardFormatAvailable(CF_TEXT))
		{
			HGLOBAL stringHandle = GetClipboardData(CF_TEXT);
			if(stringHandle != nullptr)
			{
				const char* pStr = static_cast<const char*>(GlobalLock(stringHandle));
				if(pStr)
				{
					outString.Set(pStr, strlen(pStr));
					GlobalUnlock(stringHandle);
				}
			}
		}

		CloseClipboard();
	}
}
