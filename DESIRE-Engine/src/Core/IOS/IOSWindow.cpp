#include "stdafx.h"
#include "Core/IOS/IOSWindow.h"

IOSWindow::IOSWindow(const IWindow::SCreationParams& creationParams)
	: IWindow(creationParams)
{

}

IOSWindow::~IOSWindow()
{

}

void IOSWindow::HandleWindowMessages()
{

}

void* IOSWindow::GetHandle() const
{
	return nullptr;
}

void IOSWindow::SetCursor(ECursor cursor)
{

}

bool IOSWindow::SetClipboardString(const char *str)
{
	ASSERT(str != nullptr);
	return false;
}

String IOSWindow::GetClipboardString()
{
	String str;
	return str;
}

// Create implementation
IWindow* IWindow::Create(const IWindow::SCreationParams& creationParams)
{
	return new IOSWindow(creationParams);
}
