#pragma once

#include "Core/Singleton.h"
#include "Resource/Texture.h"

class ErrorTexture : public Texture
{
	DESIRE_SINGLETON(ErrorTexture)

public:

private:
	void FillData();
};
