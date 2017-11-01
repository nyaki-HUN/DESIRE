#include "stdafx.h"
#include "Render/View.h"
#include "Render/IRender.h"
#include "Resource/Texture.h"

View::View(uint16_t width, uint16_t height)
	: posX(0)
	, posY(0)
	, width(width)
	, height(height)
	, id(1)
{
	renderTargetTexture = std::make_shared<Texture>(width, height, Texture::EFormat::RGBA8);
}

View::~View()
{
	IRender::Get()->Unbind(renderTargetTexture.get());
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
