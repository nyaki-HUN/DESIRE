#pragma once

#include "Engine/Common.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define DX_SAFE_RELEASE(pPtr)	\
	if(pPtr)					\
	{							\
		pPtr->Release();		\
		pPtr = nullptr;			\
	}
