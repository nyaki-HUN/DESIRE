#include "stdafx.h"
#include "Core/IWindow.h"
#include "Render/Render.h"

IWindow::IWindow(const SCreationParams& creationParams)
	: isFullscreen(creationParams.isFullscreen)
{
	width = std::max(WINDOW_MIN_SIZE, creationParams.width);
	height = std::max(WINDOW_MIN_SIZE, creationParams.height);
}

IWindow::~IWindow()
{

}

uint16_t IWindow::GetWidth() const
{
	return width;
}

uint16_t IWindow::GetHeight() const
{
	return height;
}

bool IWindow::IsFullscreen() const
{
	return isFullscreen;
}

void IWindow::SetSize(uint16_t i_width, uint16_t i_height)
{
	if(width == i_width && height == i_height)
	{
		return;
	}

	width = i_width;
	height = i_height;
	Render::Get()->UpdateRenderWindow(this);
}
