#pragma once

#include "Engine/Render/RenderEnums.h"
#include "Engine/Render/Texture.h"
#include "Engine/Render/VertexBuffer.h"

inline D3D12_INPUT_ELEMENT_DESC ToD3D12(VertexBuffer::EAttrib attrib)
{
	switch(attrib)
	{
		case VertexBuffer::EAttrib::Position:	return { "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Normal:		return { "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Tangent:	return { "TANGENT",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Color:		return { "COLOR",		0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord0:	return { "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord1:	return { "TEXCOORD",	1,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord2:	return { "TEXCOORD",	2,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord3:	return { "TEXCOORD",	3,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord4:	return { "TEXCOORD",	4,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord5:	return { "TEXCOORD",	5,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord6:	return { "TEXCOORD",	6,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Texcoord7:	return { "TEXCOORD",	7,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		case VertexBuffer::EAttrib::Num:		break;
	}

	ASSERT(false);
	return {};
}

inline DXGI_FORMAT ToD3D12(VertexBuffer::EAttribType attribType, uint8_t count)
{
	ASSERT(0 < count && count <= 4);

	switch(attribType)
	{
		case VertexBuffer::EAttribType::Float:
		{
			switch(count)
			{
				case 1: return DXGI_FORMAT_R32_FLOAT;
				case 2: return DXGI_FORMAT_R32G32_FLOAT;
				case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
				case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}

		case VertexBuffer::EAttribType::Uint8:
		{
			switch(count)
			{
				case 1: return DXGI_FORMAT_R8_UNORM;
				case 2: return DXGI_FORMAT_R8G8_UNORM;
				case 3: break;
				case 4: return DXGI_FORMAT_R8G8B8A8_UNORM;
			}
		}
	}

	ASSERT(false);
	return DXGI_FORMAT_UNKNOWN;
}

inline D3D12_BLEND ToD3D12(EBlend blend)
{
	switch(blend)
	{
		case EBlend::Zero:				return D3D12_BLEND_ZERO;
		case EBlend::One:				return D3D12_BLEND_ONE;
		case EBlend::SrcColor:			return D3D12_BLEND_SRC_COLOR;
		case EBlend::InvSrcColor:		return D3D12_BLEND_INV_SRC_COLOR;
		case EBlend::SrcAlpha:			return D3D12_BLEND_SRC_ALPHA;
		case EBlend::InvSrcAlpha:		return D3D12_BLEND_INV_SRC_ALPHA;
		case EBlend::DestAlpha:			return D3D12_BLEND_DEST_ALPHA;
		case EBlend::InvDestAlpha:		return D3D12_BLEND_INV_DEST_ALPHA;
		case EBlend::DestColor:			return D3D12_BLEND_DEST_COLOR;
		case EBlend::InvDestColor:		return D3D12_BLEND_INV_DEST_COLOR;
		case EBlend::SrcAlphaSat:		return D3D12_BLEND_SRC_ALPHA_SAT;
		case EBlend::BlendFactor:		return D3D12_BLEND_BLEND_FACTOR;
		case EBlend::InvBlendFactor:	return D3D12_BLEND_INV_BLEND_FACTOR;
	}

	ASSERT(false);
	return static_cast<D3D12_BLEND>(0);
}

inline D3D12_BLEND_OP ToD3D12(EBlendOp blendOp)
{
	switch(blendOp)
	{
		case EBlendOp::Add:			return D3D12_BLEND_OP_ADD;
		case EBlendOp::Subtract:	return D3D12_BLEND_OP_SUBTRACT;
		case EBlendOp::RevSubtract:	return D3D12_BLEND_OP_REV_SUBTRACT;
		case EBlendOp::Min:			return D3D12_BLEND_OP_MIN;
		case EBlendOp::Max:			return D3D12_BLEND_OP_MAX;
	}

	ASSERT(false);
	return static_cast<D3D12_BLEND_OP>(0);
}

inline DXGI_FORMAT ToD3D12(Texture::EFormat format)
{
	switch(format)
	{
		case Texture::EFormat::R8:		return DXGI_FORMAT_R8_UNORM;
		case Texture::EFormat::RG8:		return DXGI_FORMAT_R8G8_UNORM;
		case Texture::EFormat::RGB8:	return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Texture::EFormat::RGBA8:	return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Texture::EFormat::RGB32F:	return DXGI_FORMAT_R32G32B32_FLOAT;
		case Texture::EFormat::RGBA32F:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Texture::EFormat::D16:		return DXGI_FORMAT_D16_UNORM;
		case Texture::EFormat::D24_S8:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case Texture::EFormat::D32:		return DXGI_FORMAT_D32_FLOAT;
	}

	ASSERT(false);
	return DXGI_FORMAT_UNKNOWN;
}
