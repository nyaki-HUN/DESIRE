#pragma once

class Texture;

class View
{
public:
	View(uint16_t renderTextureWidth, uint16_t renderTextureHeight);
	~View();

	void SetSize(uint16_t newWidth, uint16_t newHeight);
	void ResizeRenderTexture(uint16_t newRenderTextureWidth, uint16_t newRenderTextureHeight);

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
