#pragma once

#include "Engine/Common.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define DX_SAFE_RELEASE(pPtr)	\
	if(pPtr != nullptr)			\
	{							\
		pPtr->Release();		\
		pPtr = nullptr;			\
	}
