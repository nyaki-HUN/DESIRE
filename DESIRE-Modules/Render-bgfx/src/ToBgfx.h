#pragma once

#include "Engine/Render/RenderEnums.h"
#include "Engine/Render/Texture.h"
#include "Engine/Render/VertexBuffer.h"

inline bgfx::Attrib::Enum ToBgfx(VertexBuffer::EAttrib attrib)
{
	switch(attrib)
	{
		case VertexBuffer::EAttrib::Position:	return bgfx::Attrib::Position;
		case VertexBuffer::EAttrib::Normal:		return bgfx::Attrib::Normal;
		case VertexBuffer::EAttrib::Tangent:	return bgfx::Attrib::Tangent;
		case VertexBuffer::EAttrib::Color:		return bgfx::Attrib::Color0;
		case VertexBuffer::EAttrib::Texcoord0:	return bgfx::Attrib::TexCoord0;
		case VertexBuffer::EAttrib::Texcoord1:	return bgfx::Attrib::TexCoord1;
		case VertexBuffer::EAttrib::Texcoord2:	return bgfx::Attrib::TexCoord2;
		case VertexBuffer::EAttrib::Texcoord3:	return bgfx::Attrib::TexCoord3;
		case VertexBuffer::EAttrib::Texcoord4:	return bgfx::Attrib::TexCoord4;
		case VertexBuffer::EAttrib::Texcoord5:	return bgfx::Attrib::TexCoord5;
		case VertexBuffer::EAttrib::Texcoord6:	return bgfx::Attrib::TexCoord6;
		case VertexBuffer::EAttrib::Texcoord7:	return bgfx::Attrib::TexCoord7;
		case VertexBuffer::EAttrib::Num:		break;
	}

	ASSERT(false);
	return bgfx::Attrib::Count;
}

inline bgfx::AttribType::Enum ToBgfx(VertexBuffer::EAttribType attribType)
{
	switch(attribType)
	{
		case VertexBuffer::EAttribType::Float:	return bgfx::AttribType::Float;
		case VertexBuffer::EAttribType::Uint8:	return bgfx::AttribType::Uint8;
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
		case Texture::EFormat::RGB32F:	break;
		case Texture::EFormat::RGBA32F:	return bgfx::TextureFormat::RGBA32F;
		case Texture::EFormat::D16:		return bgfx::TextureFormat::D16;
		case Texture::EFormat::D24_S8:	return bgfx::TextureFormat::D24S8;
		case Texture::EFormat::D32:		return bgfx::TextureFormat::D32;
	}

	ASSERT(false);
	return bgfx::TextureFormat::Unknown;
}
