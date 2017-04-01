#pragma once

#include "Core/Singleton.h"

class ISound
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(ISound)

public:
	virtual void PlaySound(const char *soundName) = 0;
};
