#include "Engine/stdafx.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Render/Render.h"

uint16_t OSWindow::GetWidth() const
{
	return m_width;
}

uint16_t OSWindow::GetHeight() const
{
	return m_height;
}

bool OSWindow::IsFullscreen() const
{
	return m_isFullscreen;
}

bool OSWindow::IsActive() const
{
	return m_isActive;
}

void OSWindow::RegisterMessageHandler(int msgType, MessageHandler_t messageHandler)
{
	ASSERT(messageHandler != nullptr);
	m_additionalMessageHandlers[msgType] = messageHandler;
}

void OSWindow::SetSize(uint16_t width, uint16_t height)
{
	if(m_width == width && m_height == height)
	{
		return;
	}

	m_width = width;
	m_height = height;
	Modules::Render->UpdateRenderWindow(*this);
}
