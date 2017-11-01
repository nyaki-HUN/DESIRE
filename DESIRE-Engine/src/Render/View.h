#pragma once

class Texture;

class View
{
public:
	View(uint16_t width, uint16_t height);
	~View();

	uint16_t GetPosX() const;
	uint16_t GetPosY() const;
	uint16_t GetWidth() const;
	uint16_t GetHeight() const;
	uint8_t GetID() const;

	const std::shared_ptr<Texture>& GetRenderTargetTexture() const;

private:
	uint16_t posX;
	uint16_t posY;
	uint16_t width;
	uint16_t height;
	uint8_t id;

	std::shared_ptr<Texture> renderTargetTexture;
};
