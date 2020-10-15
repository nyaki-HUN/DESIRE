#pragma once

enum class ECullMode : uint8_t
{
	None,
	CCW,
	CW
};

enum class EBlend : uint8_t
{
	Zero,				// 0, 0, 0, 0
	One,				// 1, 1, 1, 1
	SrcColor,			// Rs, Gs, Bs, As
	InvSrcColor,		// 1-Rs, 1-Gs, 1-Bs, 1-As
	SrcAlpha,			// As, As, As, As
	InvSrcAlpha,		// 1-As, 1-As, 1-As, 1-As
	DestAlpha,			// Ad, Ad, Ad, Ad
	InvDestAlpha,		// 1-Ad, 1-Ad, 1-Ad ,1-Ad
	DestColor,			// Rd, Gd, Bd, Ad
	InvDestColor,		// 1-Rd, 1-Gd, 1-Bd, 1-Ad
	SrcAlphaSat,		// f, f, f, 1; where f = min(As, 1-Ad)
	BlendFactor,		// blendFactor
	InvBlendFactor		// 1-blendFactor
};

enum class EBlendOp : uint8_t
{
	Add,
	Subtract,
	RevSubtract,
	Min,
	Max
};

enum class EColorWrite : uint8_t
{
	Red		= 1 << 0,
	Green	= 1 << 1,
	Blue	= 1 << 2,
	Alpha	= 1 << 3,

	RGB = Red | Green | Blue,
	All = Red | Green | Blue | Alpha
};

enum class EDepthTest : uint8_t
{
	Disabled,
	Less,
	LessEqual,
	Greater,
	GreaterEqual,
	Equal,
	NotEqual
};

enum class EAddressMode : uint8_t
{
	Repeat,
	Clamp,
	MirroredRepeat,
	MirrorOnce,
	Border
};

enum class EFilterMode : uint8_t
{
	Point,			// No filtering, the texel with coordinates nearest to the desired pixel value is used (at most 1 texel being sampled)
	Bilinear,		// Texture samples are averaged (at most 4 samples)
	Trilinear,		// Texture samples are averaged and also blended between mipmap levels (at most 8 samples)
	Anisotropic,	// Use anisotropic interpolation
};
