#include "Engine/stdafx.h"
#include "Engine/Core/LINUX/LINUXWindow.h"
#include "Engine/Application/Application.h"
#include "Engine/Application/EAppEventType.h"
#include "Engine/Core/String/WritableString.h"

#include <X11/cursorfont.h>

LINUXWindow::LINUXWindow(const IWindowCreationParams& creationParams)
	: IWindow(creationParams)
{
	for(int i = 0; i < NUM_CURSORS; ++i)
	{
		cursors[i] = None;
	}

	// Create local X Display connection
	display = XOpenDisplay(nullptr);
	if(display == nullptr)
	{
		LOG_ERROR("Unable to open a connection to the X server");
	}
}

LINUXWindow::~LINUXWindow()
{
	if(display != nullptr)
	{
		for(int i = 0; i < NUM_CURSORS; ++i)
		{
			if(cursors[i] != None)
			{
				XFreeCursor(display, cursors[i]);
			}
		}

		XCloseDisplay(display);
		display = nullptr;
	}
}

void LINUXWindow::HandleWindowMessages()
{
	if(display == nullptr)
	{
		return;
	}

	XEvent event;
	while(XPending(display) > 0)
	{
		XNextEvent(display, &event);

		if(window->isActive)
		{
			auto it = window->additionalMessageHandlers.find(event.type);
			if(it != window->additionalMessageHandlers.end())
			{
				it->second(event);
			}
		}

		switch(event.type)
		{
			case FocusIn:
				window->isActive = true;
				IApp::Get()->SendEvent(EAppEventType::ENTER_FOREGROUND);
				break;

			case FocusOut:
				window->isActive = false;
				IApp::Get()->SendEvent(EAppEventType::ENTER_BACKGROUND);
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
					IApp::Stop();
				}
				break;

			case MappingNotify:
				break;
		}
	}
}

void* LINUXWindow::GetHandle() const
{
	return windowHandle;
}

void LINUXWindow::SetWindowTitle(const char *newTitle)
{
	if(display == nullptr)
	{
		return;
	}

	XTextProperty titleTextProp;
	XStringListToTextProperty((char**)&newTitle, 1, &titleTextProp);

	XSetWMName(display, windowHandle, &titleTextProp);
	XSetWMIconName(display, windowHandle, &titleTextProp);

	XFree(titleTextProp.value);
}

void LINUXWindow::SetCursor(ECursor cursor)
{
	if(display == nullptr)
	{
		return;
	}

	if(cursors[cursor] == None)
	{
		// X Font Cursors: http://tronche.com/gui/x/xlib/appendix/b/
		switch(cursor)
		{
			case CURSOR_ARROW:				cursors[cursor] = XCreateFontCursor(display, XC_left_ptr); break;
			case CURSOR_MOVE:				cursors[cursor] = XCreateFontCursor(display, XC_fleur); break;
			case CURSOR_SIZE_BOTTOMLEFT:	cursors[cursor] = XCreateFontCursor(display, XC_bottom_left_corner); break;
			case CURSOR_SIZE_BOTTOMRIGHT:	cursors[cursor] = XCreateFontCursor(display, XC_bottom_right_corner); break;
			case CURSOR_SIZE_NS:			cursors[cursor] = XCreateFontCursor(display, XC_double_arrow); break;
			case CURSOR_SIZE_WE:			cursors[cursor] = XCreateFontCursor(display, XC_sb_h_double_arrow); break;
			case CURSOR_HAND:				cursors[cursor] = XCreateFontCursor(display, XC_hand2); break;
			case CURSOR_IBEAM:				cursors[cursor] = XCreateFontCursor(display, XC_xterm); break;
			case CURSOR_UP:					cursors[cursor] = XCreateFontCursor(display, XC_center_ptr); break;
			case NUM_CURSORS:				ASSERT(false); return;
		}
	}

	XDefineCursor(display, windowHandle, cursors[cursor]);
	XFlush(display);
}

bool LINUXWindow::SetClipboardString(const String& string)
{
	if(display != nullptr)
	{
		XSetSelectionOwner(display, XA_PRIMARY, None, CurrentTime);
		XStoreBytes(display, string.c_str(), string.Length());
		return true;
	}

	return false;
}

void LINUXWindow::GetClipboardString(WritableString& outString)
{
	outString.Clear();

	if(display == nullptr)
	{
		return;
	}

	int size = 0;
	char *ptr = XFetchBytes(display, &size);
	if(ptr != nullptr)
	{
		outString.Assign(ptr, size);
		XFree(ptr);
	}
}

void LINUXWindow::RegisterMessageHandler(int msgType, MessageHandler_t messageHandler)
{
	ASSERT(messageHandler != nullptr);
	additionalMessageHandlers[msgType] = messageHandler;
}

// --------------------------------------------------------------------------------------------------------------------
//	OSWindow
// --------------------------------------------------------------------------------------------------------------------

std::unique_ptr<OSWindow> OSWindow::Create(const OSWindowCreationParams& creationParams)
{
	return std::make_unique<LINUXWindow>(creationParams);
}
