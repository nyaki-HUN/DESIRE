#include "Engine/stdafx.h"
#include "Engine/Application/OSWindow.h"

#if DESIRE_PLATFORM_OSX

// --------------------------------------------------------------------------------------------------------------------
//	OSWindowImpl
// --------------------------------------------------------------------------------------------------------------------

class OSWindowImpl
{
public:
	NSCursor *cursors[OSWindow::NUM_CURSORS] = {};
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

}

OSXWindow::~OSXWindow()
{
	for(int i = 0; i < NUM_CURSORS; ++i)
	{
		if(impl->cursors[i] != nullptr)
		{
			[impl->cursors[i] release];
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
	if(impl->cursors[cursor] == nullptr)
	{
		switch(cursor)
		{
			case CURSOR_ARROW:				impl->cursors[cursor] = [[NSCursor arrowCursor] retain]; break;
			case CURSOR_MOVE:				impl->cursors[cursor] = [[NSCursor closedHandCursor] retain]; break;
			case CURSOR_SIZE_BOTTOMLEFT:	impl->cursors[cursor] = [[NSCursor closedHandCursor] retain]; break;			// OSX doesn't have it
			case CURSOR_SIZE_BOTTOMRIGHT:	impl->cursors[cursor] = [[NSCursor closedHandCursor] retain]; break;			// OSX doesn't have it
			case CURSOR_SIZE_NS:			impl->cursors[cursor] = [[NSCursor resizeUpDownCursor] retain]; break;
			case CURSOR_SIZE_WE:			impl->cursors[cursor] = [[NSCursor resizeLeftRightCursor] retain]; break;
			case CURSOR_HAND:				impl->cursors[cursor] = [[NSCursor pointingHandCursor] retain]; break;
			case CURSOR_IBEAM:				impl->cursors[cursor] = [[NSCursor IBeamCursor] retain]; break;
			case CURSOR_UP:					impl->cursors[cursor] = [[NSCursor resizeUpCursor] retain]; break;
			case CURSOR_NOT_ALLOWED:		impl->cursors[cursor] = [[NSCursor operationNotAllowedCursor] retain]; break;
			case NUM_CURSORS:				ASSERT(false); return;
		}
	}

	[impl->cursors[cursor] set];
}

bool OSXWindow::SetClipboardString(const String& string)
{
	NSArray *types = [NSArray arrayWithObjects:NSStringPboardType, nil];

	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	[pasteboard declareTypes:types owner:nil];
	[pasteboard setString:[NSString stringWithUTF8String:string.Str()] forType:NSStringPboardType];
	return true;
}

void OSWindow::GetClipboardString(WritableString& outString)
{
	outString.Clear();

	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	if([[pasteboard types] containsObject:NSStringPboardType])
	{
		NSString *object = [pasteboard stringForType:NSStringPboardType];
		if(object != nullptr)
		{
			outString.Assign([object UTF8String], [object length]);
		}
	}
}

#endif	// #if DESIRE_PLATFORM_OSX
