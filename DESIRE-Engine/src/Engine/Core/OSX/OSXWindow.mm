#include "stdafx.h"
#include "Core/OSX/OSXWindow.h"

OSXWindow::OSXWindow(const IWindow::CreationParams& creationParams)
	: IWindow(creationParams)
{
	for(int i = 0; i < NUM_CURSORS; ++i)
	{
		cursors[i] = nullptr;
	}
}

OSXWindow::~OSXWindow()
{
	for(int i = 0; i < NUM_CURSORS; ++i)
	{
		if(cursors[i] != nullptr)
		{
			[cursors[i] release];
		}
	}
}

void OSXWindow::HandleWindowMessages()
{
	EventTargetRef targetWindow = GetEventDispatcherTarget();
	if(targetWindow == nullptr)
	{
		return;
	}

	EventRef event = nullptr;
	if(ReceiveNextEvent(0, nullptr, kEventDurationNoWait, true, &event) == noErr)
	{
		// Dispatch the event
		SendEventToEventTarget(event, targetWindow);
		ReleaseEvent(event);
	}
}

void* OSXWindow::GetHandle() const
{
	return nullptr;
}

void OSXWindow::SetWindowTitle(const char *newTitle)
{
	WindowRef windowRef = ActiveNonFloatingWindow();
	CFStringRef titleRef = CFStringCreateWithCString(kCFAllocatorDefault, newTitle, kCFStringEncodingASCII);
	SetWindowTitleWithCFString(windowRef, titleRef);
	CFRelease(titleRef);
}

void OSXWindow::SetCursor(ECursor cursor)
{
	if(cursors[cursor] == nullptr)
	{
		switch(cursor)
		{
			case CURSOR_ARROW:				cursors[cursor] = [[NSCursor arrowCursor] retain]; break;
			case CURSOR_MOVE:				cursors[cursor] = [[NSCursor crosshairCursor] retain]; break;
			case CURSOR_SIZE_BOTTOMLEFT:	cursors[cursor] = [[NSCursor arrowCursor] retain]; break;			// OSX doesn't have it
			case CURSOR_SIZE_BOTTOMRIGHT:	cursors[cursor] = [[NSCursor arrowCursor] retain]; break;			// OSX doesn't have it
			case CURSOR_SIZE_NS:			cursors[cursor] = [[NSCursor resizeUpDownCursor]; break;
			case CURSOR_SIZE_WE:			cursors[cursor] = [[NSCursor resizeLeftRightCursor] retain]; break;
			case CURSOR_HAND:				cursors[cursor] = [[NSCursor pointingHandCursor] retain]; break;
			case CURSOR_IBEAM:				cursors[cursor] = [[NSCursor IBeamCursor] retain]; break;
			case CURSOR_UP:					cursors[cursor] = [[NSCursor resizeUpCursor] retain]; break;
			case NUM_CURSORS:				ASSERT(false); return;
		}
	}

	[cursors[cursor] set];
}

bool OSXWindow::SetClipboardString(const String& string)
{
	NSArray *types = [NSArray arrayWithObjects:NSStringPboardType, nil];

	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	[pasteboard declareTypes:types owner:nil];
	[pasteboard setString:[NSString stringWithUTF8String:string.c_str()] forType:NSStringPboardType];
	return true;
}

String OSXWindow::GetClipboardString()
{
	String str;

	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	if([[pasteboard types] containsObject:NSStringPboardType])
	{
		NSString *object = [pasteboard stringForType:NSStringPboardType];
		if(object != nullptr)
		{
			str = String([object UTF8String]);
		}
	}

	return str;
}

// --------------------------------------------------------------------------------------------------------------------
//	IWindow
// --------------------------------------------------------------------------------------------------------------------

std::unique_ptr<IWindow> IWindow::Create(const IWindow::CreationParams& creationParams)
{
	return std::make_unique<OSXWindow>(creationParams);
}
