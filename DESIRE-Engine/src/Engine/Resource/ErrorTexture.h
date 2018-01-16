#pragma once

#include "Engine/Core/Singleton.h"
#include "Engine/Resource/Texture.h"

class ErrorTexture : public Texture
{
	DESIRE_SINGLETON(ErrorTexture)

public:

private:
	void FillData();
};
