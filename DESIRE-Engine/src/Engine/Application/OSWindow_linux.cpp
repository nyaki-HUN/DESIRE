#include "Engine/stdafx.h"
#include "Engine/Application/OSWindow.h"

#if DESIRE_PLATFORM_LINUX

#include "Engine/Application/Application.h"

#include "Engine/Core/String/WritableString.h"

#include <X11/Xlib.h>
#include <X11/cursorfont.h>

// --------------------------------------------------------------------------------------------------------------------
//	OSWindowImpl
// --------------------------------------------------------------------------------------------------------------------

struct OSWindowImpl
{
	Display* pDisplay = nullptr;
	Window windowHandle;
	Cursor cursors[OSWindow::NUM_CURSORS];
};

// --------------------------------------------------------------------------------------------------------------------
//	OSWindow
// --------------------------------------------------------------------------------------------------------------------

OSWindow::OSWindow(const OSWindowCreationParams& creationParams)
	: m_width(std::max(kWindowMinSize, creationParams.width))
	, m_height(std::max(kWindowMinSize, creationParams.height))
	, m_isFullscreen(creationParams.isFullscreen)
	, m_spImpl(std::make_unique<OSWindowm_spImpl>())
{
	for(int i = 0; i < NUM_CURSORS; ++i)
	{
		m_spImpl->cursors[i] = None;
	}

	// Create local X Display connection
	m_spImpl->pDisplay = XOpenDisplay(nullptr);
	if(m_spImpl->pDisplay == nullptr)
	{
		LOG_ERROR("Unable to open a connection to the X server");
	}
}

OSWindow::~OSWindow()
{
	if(m_spImpl->pDisplay != nullptr)
	{
		for(int i = 0; i < NUM_CURSORS; ++i)
		{
			if(m_spImpl->cursors[i] != None)
			{
				XFreeCursor(m_spImpl->pDisplay, m_spImpl->cursors[i]);
			}
		}

		XCloseDisplay(m_spImpl->pDisplay);
		m_spImpl->pDisplay = nullptr;
	}
}

void OSWindow::HandleWindowMessages()
{
	if(m_spImpl->pDisplay == nullptr)
	{
		return;
	}

	XEvent event;
	while(XPending(m_spImpl->pDisplay) > 0)
	{
		XNextEvent(m_spImpl->pDisplay, &event);

		if(isActive)
		{
			auto iter = m_spImpl->additionalMessageHandlers.find(event.type);
			if(iter != m_spImpl->additionalMessageHandlers.end())
			{
				iter->second(&event);
			}
		}

		switch(event.type)
		{
			case FocusIn:
				m_isActive = true;
				Modules::Application->SendEvent(EAppEventType::Activate);
				break;

			case FocusOut:
				m_isActive = false;
				Modules::Application->SendEvent(EAppEventType::Deactivate);
				break;

			case VisibilityNotify:
				switch(event.xvisibility.state)
				{
					case VisibilityUnobscured:
						break;
					case VisibilityPartiallyObscured:
						break;
					case VisibilityFullyObscured:
						break;
				}
				break;

			case DestroyNotify:
				IApp::Stop();
				break;

			case UnmapNotify:
				break;

			case MapNotify:
				break;

			case ConfigureNotify:
				break;

			case GravityNotify:
				LOG_DEBUG("X Event: GravityNotify");
				break;

			case ClientMessage:
				// Check delete window message
				if(event.xclient.data.l[0] == (long)wmDeleteWindow)
				{
					Application::Stop();
				}
				break;

			case MappingNotify:
				break;
		}
	}
}

void* OSWindow::GetHandle() const
{
	return m_spImpl->windowHandle;
}

void OSWindow::SetWindowTitle(const char* pNewTitle)
{
	if(m_spImpl->pDisplay == nullptr)
	{
		return;
	}

	XTextProperty titleTextProp;
	XStringListToTextProperty(const_cast<char**>(&pNewTitle), 1, &titleTextProp);

	XSetWMName(m_spImpl->pDisplay, m_spImpl->windowHandle, &titleTextProp);
	XSetWMIconName(m_spImpl->pDisplay, m_spImpl->windowHandle, &titleTextProp);

	XFree(titleTextProp.value);
}

void OSWindow::SetCursor(ECursor cursor)
{
	if(m_spImpl->pDisplay == nullptr)
	{
		return;
	}

	if(m_spImpl->cursors[cursor] == None)
	{
		// X Font Cursors: http://tronche.com/gui/x/xlib/appendix/b/
		switch(cursor)
		{
			case CURSOR_ARROW:				m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_left_ptr); break;
			case CURSOR_MOVE:				m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_fleur); break;
			case CURSOR_SIZE_BOTTOMLEFT:	m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_bottom_left_corner); break;
			case CURSOR_SIZE_BOTTOMRIGHT:	m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_bottom_right_corner); break;
			case CURSOR_SIZE_NS:			m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_sb_v_double_arrow); break;
			case CURSOR_SIZE_WE:			m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_sb_h_double_arrow); break;
			case CURSOR_HAND:				m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_hand2); break;
			case CURSOR_IBEAM:				m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_xterm); break;
			case CURSOR_UP:					m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_center_ptr); break;
			case CURSOR_NOT_ALLOWED:		m_spImpl->cursors[cursor] = XCreateFontCursor(m_spImpl->pDisplay, XC_circle); break;
			case NUM_CURSORS:				ASSERT(false); return;
		}
	}

	XDefineCursor(m_spImpl->pDisplay, m_spImpl->windowHandle, m_spImpl->cursors[cursor]);
	XFlush(m_spImpl->pDisplay);
}

bool OSWindow::SetClipboardString(const String& string)
{
	if(m_spImpl->pDisplay != nullptr)
	{
		XSetSelectionOwner(m_spImpl->pDisplay, XA_PRIMARY, None, CurrentTime);
		XStoreBytes(m_spImpl->pDisplay, string.Str(), string.Length());
		return true;
	}

	return false;
}

void OSWindow::GetClipboardString(WritableString& outString)
{
	outString.Clear();

	if(m_spImpl->pDisplay == nullptr)
	{
		return;
	}

	int size = 0;
	char* pStr = XFetchBytes(m_spImpl->pDisplay, &size);
	if(ptr != nullptr)
	{
		outString.Assign(pStr, size);
		XFree(pStr);
	}
}

#endif	// #if DESIRE_PLATFORM_LINUX
