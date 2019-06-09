#pragma once

class SoundSystem
{
public:
	SoundSystem();
	virtual ~SoundSystem();

	virtual void PlaySound(const char* soundName) = 0;
};
