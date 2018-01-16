#include "Engine/stdafx.h"
#include "Engine/Render/View.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Resource/Texture.h"

View::View(uint16_t renderTextureWidth, uint16_t renderTextureHeight)
	: width(renderTextureWidth)
	, height(renderTextureHeight)
{
	renderTarget = std::make_unique<RenderTarget>(width, height);
}

View::~View()
{

}

void View::SetSize(uint16_t newWidth, uint16_t newHeight)
{
	if(renderTarget->GetWidth() < newWidth || renderTarget->GetHeight() < newHeight)
	{
		renderTarget = nullptr;
		renderTarget = std::make_unique<RenderTarget>(newWidth, newHeight);
	}

	width = newWidth;
	height = newHeight;
}

uint16_t View::GetPosX() const
{
	return posX;
}

uint16_t View::GetPosY() const
{
	return posY;
}

uint16_t View::GetWidth() const
{
	return width;
}

uint16_t View::GetHeight() const
{
	return height;
}

RenderTarget* View::GetRenderTarget() const
{
	return renderTarget.get();
}
