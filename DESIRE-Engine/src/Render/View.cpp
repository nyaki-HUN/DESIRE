#include "stdafx.h"
#include "Render/View.h"
#include "Render/IRender.h"
#include "Resource/Texture.h"

View::View(uint16_t renderTextureWidth, uint16_t renderTextureHeight)
	: posX(0)
	, posY(0)
	, width(renderTextureWidth)
	, height(renderTextureHeight)
	, id(1)
{
	renderTargetTexture = std::make_shared<Texture>(renderTextureWidth, renderTextureHeight, Texture::EFormat::RGBA8);
}

View::~View()
{

}

void View::SetSize(uint16_t newWidth, uint16_t newHeight)
{
	newWidth = std::min(newWidth, renderTargetTexture->width);
	newHeight = std::min(newHeight, renderTargetTexture->height);

	width = newWidth;
	height = newHeight;
}

void View::ResizeRenderTexture(uint16_t newRenderTextureWidth, uint16_t newRenderTextureHeight)
{
	if(renderTargetTexture->width == newRenderTextureWidth && renderTargetTexture->height == newRenderTextureHeight)
	{
		return;
	}

	renderTargetTexture = std::make_shared<Texture>(newRenderTextureWidth, newRenderTextureHeight, Texture::EFormat::RGBA8);
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

uint8_t View::GetID() const
{
	return id;
}

const std::shared_ptr<Texture>& View::GetRenderTargetTexture() const
{
	return renderTargetTexture;
}
