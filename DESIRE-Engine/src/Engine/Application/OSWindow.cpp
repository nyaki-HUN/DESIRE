#include "Engine/stdafx.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Render/Render.h"

uint16_t OSWindow::GetWidth() const
{
	return width;
}

uint16_t OSWindow::GetHeight() const
{
	return height;
}

bool OSWindow::IsFullscreen() const
{
	return isFullscreen;
}

bool OSWindow::IsActive() const
{
	return isActive;
}

void OSWindow::RegisterMessageHandler(int msgType, MessageHandler_t messageHandler)
{
	ASSERT(messageHandler != nullptr);
	additionalMessageHandlers[msgType] = messageHandler;
}

void OSWindow::SetSize(uint16_t i_width, uint16_t i_height)
{
	if(width == i_width && height == i_height)
	{
		return;
	}

	width = i_width;
	height = i_height;
	Modules::Render->UpdateRenderWindow(this);
}
