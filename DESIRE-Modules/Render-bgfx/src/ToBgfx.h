#pragma once

#include "Engine/Render/Mesh.h"
#include "Engine/Render/RenderEnums.h"
#include "Engine/Render/Texture.h"

inline bgfx::Attrib::Enum ToBgfx(Mesh::EAttrib attrib)
{
	switch(attrib)
	{
		case Mesh::EAttrib::Position:	return bgfx::Attrib::Position;
		case Mesh::EAttrib::Normal:		return bgfx::Attrib::Normal;
		case Mesh::EAttrib::Color:		return bgfx::Attrib::Color0;
		case Mesh::EAttrib::Texcoord0:	return bgfx::Attrib::TexCoord0;
		case Mesh::EAttrib::Texcoord1:	return bgfx::Attrib::TexCoord1;
		case Mesh::EAttrib::Texcoord2:	return bgfx::Attrib::TexCoord2;
		case Mesh::EAttrib::Texcoord3:	return bgfx::Attrib::TexCoord3;
		case Mesh::EAttrib::Texcoord4:	return bgfx::Attrib::TexCoord4;
		case Mesh::EAttrib::Texcoord5:	return bgfx::Attrib::TexCoord5;
		case Mesh::EAttrib::Texcoord6:	return bgfx::Attrib::TexCoord6;
		case Mesh::EAttrib::Texcoord7:	return bgfx::Attrib::TexCoord7;
	}

	ASSERT(false);
	return bgfx::Attrib::Count;
}

inline bgfx::AttribType::Enum ToBgfx(Mesh::EAttribType attribType)
{
	switch(attribType)
	{
		case Mesh::EAttribType::Float:	return bgfx::AttribType::Float;
		case Mesh::EAttribType::Uint8:	return bgfx::AttribType::Uint8;
	}

	ASSERT(false);
	return bgfx::AttribType::Count;
}

inline uint64_t ToBgfx(EBlend blend)
{
	switch(blend)
	{
		case EBlend::Zero:				return BGFX_STATE_BLEND_ZERO;
		case EBlend::One:				return BGFX_STATE_BLEND_ONE;
		case EBlend::SrcColor:			return BGFX_STATE_BLEND_SRC_COLOR;
		case EBlend::InvSrcColor:		return BGFX_STATE_BLEND_INV_SRC_COLOR;
		case EBlend::SrcAlpha:			return BGFX_STATE_BLEND_SRC_ALPHA;
		case EBlend::InvSrcAlpha:		return BGFX_STATE_BLEND_INV_SRC_ALPHA;
		case EBlend::DestAlpha:			return BGFX_STATE_BLEND_DST_ALPHA;
		case EBlend::InvDestAlpha:		return BGFX_STATE_BLEND_INV_DST_ALPHA;
		case EBlend::DestColor:			return BGFX_STATE_BLEND_DST_COLOR;
		case EBlend::InvDestColor:		return BGFX_STATE_BLEND_INV_DST_COLOR;
		case EBlend::SrcAlphaSat:		return BGFX_STATE_BLEND_SRC_ALPHA_SAT;
		case EBlend::BlendFactor:		return BGFX_STATE_BLEND_FACTOR;
		case EBlend::InvBlendFactor:	return BGFX_STATE_BLEND_INV_FACTOR;
	}

	ASSERT(false);
	return 0;
}

inline uint64_t ToBgfx(EBlendOp blendOp)
{
	switch(blendOp)
	{
		case EBlendOp::Add:			return BGFX_STATE_BLEND_EQUATION_ADD;
		case EBlendOp::Subtract:	return BGFX_STATE_BLEND_EQUATION_SUB;
		case EBlendOp::RevSubtract:	return BGFX_STATE_BLEND_EQUATION_REVSUB;
		case EBlendOp::Min:			return BGFX_STATE_BLEND_EQUATION_MIN;
		case EBlendOp::Max:			return BGFX_STATE_BLEND_EQUATION_MAX;
	}

	ASSERT(false);
	return 0;
}

inline bgfx::TextureFormat::Enum ToBgfx(Texture::EFormat format)
{
	switch(format)
	{
		case Texture::EFormat::R8:		return bgfx::TextureFormat::R8;
		case Texture::EFormat::RG8:		return bgfx::TextureFormat::RG8;
		case Texture::EFormat::RGB8:	return bgfx::TextureFormat::RGB8;
		case Texture::EFormat::RGBA8:	return bgfx::TextureFormat::RGBA8;
		case Texture::EFormat::RGB32F:	return bgfx::TextureFormat::Unknown;
		case Texture::EFormat::RGBA32F:	return bgfx::TextureFormat::RGBA32F;
		case Texture::EFormat::D16:		return bgfx::TextureFormat::D16;
		case Texture::EFormat::D24_S8:	return bgfx::TextureFormat::D24S8;
		case Texture::EFormat::D32:		return bgfx::TextureFormat::D32;
	}

	ASSERT(false);
	return bgfx::TextureFormat::Unknown;
}
