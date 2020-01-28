#pragma once

struct OSWindowCreationParams
{
	static constexpr int32_t kPosCenteredOnScreen = INT32_MAX;

	int32_t posX = kPosCenteredOnScreen;
	int32_t posY = kPosCenteredOnScreen;
	uint16_t width = 1366;
	uint16_t height = 768;
	bool isFullscreen = false;
};
