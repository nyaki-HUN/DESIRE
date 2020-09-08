#pragma once

class RenderTarget;

class View
{
public:
	View(uint16_t renderTextureWidth, uint16_t renderTextureHeight);
	~View();

	void SetSize(uint16_t newWidth, uint16_t newHeight);

	uint16_t GetPosX() const;
	uint16_t GetPosY() const;
	uint16_t GetWidth() const;
	uint16_t GetHeight() const;

	RenderTarget& GetRenderTarget() const;

private:
	uint16_t posX = 0;
	uint16_t posY = 0;
	uint16_t width = 0;
	uint16_t height = 0;

	std::unique_ptr<RenderTarget> renderTarget;
};
