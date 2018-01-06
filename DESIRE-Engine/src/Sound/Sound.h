#pragma once

#include "Core/Singleton.h"

class Sound
{
	DESIRE_DECLARE_SINGLETON(Sound)

public:
	virtual void PlaySound(const char *soundName) = 0;
};
