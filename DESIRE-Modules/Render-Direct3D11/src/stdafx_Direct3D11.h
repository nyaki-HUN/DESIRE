#pragma once

#include "Engine/Common.h"

#include <d3d11.h>
#include <DirectXMath.h>

#define DX_SAFE_RELEASE(pPtr)	\
	if(pPtr != nullptr)			\
	{							\
		pPtr->Release();		\
		pPtr = nullptr;			\
	}
