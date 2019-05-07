#pragma once

#include <stdint.h>

struct OSWindowCreationParams
{
	static constexpr int kPosCenteredOnScreen = INT32_MAX;

	int posX = kPosCenteredOnScreen;
	int posY = kPosCenteredOnScreen;
	uint16_t width = 1366;
	uint16_t height = 768;
	bool isFullscreen = false;
};
