#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_LINUX

#include "Engine/Application/OSWindow.h"
#include "Engine/Application/Application.h"
#include "Engine/Application/EAppEventType.h"
#include "Engine/Core/String/WritableString.h"

#include <X11/Xlib.h>
#include <X11/cursorfont.h>

// --------------------------------------------------------------------------------------------------------------------
//	OSWindowImpl
// --------------------------------------------------------------------------------------------------------------------

class OSWindowImpl
{
public:
	Display *display;
	Window windowHandle;
	Cursor cursors[OSWindow::NUM_CURSORS];
};

// --------------------------------------------------------------------------------------------------------------------
//	OSWindow
// --------------------------------------------------------------------------------------------------------------------

OSWindow::OSWindow(const OSWindowCreationParams& creationParams)
	: width(std::max(kWindowMinSize, creationParams.width))
	, height(std::max(kWindowMinSize, creationParams.height))
	, isFullscreen(creationParams.isFullscreen)
	, impl(std::make_unique<OSWindowImpl>())
{
	for(int i = 0; i < NUM_CURSORS; ++i)
	{
		impl->cursors[i] = None;
	}

	// Create local X Display connection
	impl->display = XOpenDisplay(nullptr);
	if(impl->display == nullptr)
	{
		LOG_ERROR("Unable to open a connection to the X server");
	}
}

OSWindow::~OSWindow()
{
	if(impl->display != nullptr)
	{
		for(int i = 0; i < NUM_CURSORS; ++i)
		{
			if(impl->cursors[i] != None)
			{
				XFreeCursor(impl->display, impl->cursors[i]);
			}
		}

		XCloseDisplay(impl->display);
		impl->display = nullptr;
	}
}

void OSWindow::HandleWindowMessages()
{
	if(impl->display == nullptr)
	{
		return;
	}

	XEvent event;
	while(XPending(impl->display) > 0)
	{
		XNextEvent(impl->display, &event);

		if(isActive)
		{
			auto it = impl->additionalMessageHandlers.find(event.type);
			if(it != impl->additionalMessageHandlers.end())
			{
				it->second(&event);
			}
		}

		switch(event.type)
		{
			case FocusIn:
				isActive = true;
				Modules::Application->SendEvent(EAppEventType::EnterForeground);
				break;

			case FocusOut:
				isActive = false;
				Modules::Application->SendEvent(EAppEventType::EnterBackground);
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
	return impl->windowHandle;
}

void OSWindow::SetWindowTitle(const char *newTitle)
{
	if(impl->display == nullptr)
	{
		return;
	}

	XTextProperty titleTextProp;
	XStringListToTextProperty(const_cast<char**>(&newTitle), 1, &titleTextProp);

	XSetWMName(impl->display, impl->windowHandle, &titleTextProp);
	XSetWMIconName(impl->display, impl->windowHandle, &titleTextProp);

	XFree(titleTextProp.value);
}

void OSWindow::SetCursor(ECursor cursor)
{
	if(impl->display == nullptr)
	{
		return;
	}

	if(impl->cursors[cursor] == None)
	{
		// X Font Cursors: http://tronche.com/gui/x/xlib/appendix/b/
		switch(cursor)
		{
			case CURSOR_ARROW:				impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_left_ptr); break;
			case CURSOR_MOVE:				impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_fleur); break;
			case CURSOR_SIZE_BOTTOMLEFT:	impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_bottom_left_corner); break;
			case CURSOR_SIZE_BOTTOMRIGHT:	impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_bottom_right_corner); break;
			case CURSOR_SIZE_NS:			impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_double_arrow); break;
			case CURSOR_SIZE_WE:			impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_sb_h_double_arrow); break;
			case CURSOR_HAND:				impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_hand2); break;
			case CURSOR_IBEAM:				impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_xterm); break;
			case CURSOR_UP:					impl->cursors[cursor] = XCreateFontCursor(impl->display, XC_center_ptr); break;
			case NUM_CURSORS:				ASSERT(false); return;
		}
	}

	XDefineCursor(impl->display, impl->windowHandle, impl->cursors[cursor]);
	XFlush(impl->display);
}

bool OSWindow::SetClipboardString(const String& string)
{
	if(impl->display != nullptr)
	{
		XSetSelectionOwner(impl->display, XA_PRIMARY, None, CurrentTime);
		XStoreBytes(impl->display, string.Str(), string.Length());
		return true;
	}

	return false;
}

void OSWindow::GetClipboardString(WritableString& outString)
{
	outString.Clear();

	if(impl->display == nullptr)
	{
		return;
	}

	int size = 0;
	char *ptr = XFetchBytes(impl->display, &size);
	if(ptr != nullptr)
	{
		outString.Assign(ptr, size);
		XFree(ptr);
	}
}

#endif	// #if DESIRE_PLATFORM_LINUX
