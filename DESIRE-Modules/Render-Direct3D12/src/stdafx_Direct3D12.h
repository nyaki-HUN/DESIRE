#pragma once

#include "Engine/Common.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#define DX_SAFE_RELEASE(pPtr)	\
	if(pPtr != nullptr)			\
	{							\
		pPtr->Release();		\
		pPtr = nullptr;			\
	}
