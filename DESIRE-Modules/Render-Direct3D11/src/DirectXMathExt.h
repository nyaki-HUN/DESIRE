#pragma once

#include "Engine/Core/math/vectormath.h"

#include <DirectXMath.h>

#if (defined(DESIRE_USE_SSE) && defined(_XM_SSE_INTRINSICS_)) || (defined(__ARM_NEON__) && defined(_XM_ARM_NEON_INTRINSICS_))
	#define USE_NATIVE_CONVERSION
#endif

inline Vector4 GetVector4(DirectX::FXMVECTOR xmVec)
{
#if defined(USE_NATIVE_CONVERSION)
	return Vector4(xmVec);
#else
	DirectX::XMFLOAT4A data;
	DirectX::XMStoreFloat4A(&data, xmVec);

	Vector4 vec;
	vec.LoadXYZW(&data.x);
	return vec;
#endif
}

inline DirectX::XMVECTOR GetXMVECTOR(const Vector4& vec)
{
#if defined(USE_NATIVE_CONVERSION)
	return DirectX::XMVECTOR(vec);
#else
	DirectX::XMFLOAT4A data;
	vec.StoreXYZW(&data.x);
	return DirectX::XMLoadFloat4A(&data);
#endif
}

#undef USE_NATIVE_CONVERSION
