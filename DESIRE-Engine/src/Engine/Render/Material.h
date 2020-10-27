#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedString.h"

#include "Engine/Render/RenderEnums.h"

class Shader;
class Texture;

class Material
{
public:
	struct TextureInfo
	{
		std::shared_ptr<Texture> m_spTexture;
		EFilterMode m_filterMode;
		EAddressMode m_addressMode;
	};

	struct ShaderParam
	{
		const HashedString m_name;

		ShaderParam(HashedString name, const void* pParam);
		ShaderParam(HashedString name, std::function<void(float*)>&& func);
		const void* GetValue() const;

	private:
		const void* m_pParam = nullptr;
		std::function<void(float*)> m_paramFunc = nullptr;
	};

	Material();
	~Material();

	void AddTexture(const std::shared_ptr<Texture>& spTexture, EFilterMode filterMode = EFilterMode::Trilinear, EAddressMode addressMode = EAddressMode::Repeat);
	void ChangeTexture(uint8_t idx, const std::shared_ptr<Texture>& spTexture);
	const Array<TextureInfo>& GetTextures() const;

	void AddShaderParam(HashedString name, const void* pParam);
	void AddShaderParam(HashedString name, std::function<void(float*)>&& func);
	void RemoveAllShaderParams();
	const Array<ShaderParam>& Material::GetShaderParams() const;

	std::shared_ptr<Shader> m_spVertexShader;
	std::shared_ptr<Shader> m_spPixelShader;

	ECullMode m_cullMode = ECullMode::CCW;

	EBlend m_srcBlendRGB = EBlend::SrcAlpha;
	EBlend m_destBlendRGB = EBlend::InvSrcAlpha;
	EBlendOp m_blendOpRGB = EBlendOp::Add;
	EBlend m_srcBlendAlpha = EBlend::SrcAlpha;
	EBlend m_destBlendAlpha = EBlend::InvSrcAlpha;
	EBlendOp m_blendOpAlpha = EBlendOp::Add;

	EColorWrite m_colorWriteMask = EColorWrite::All;

	EDepthTest m_depthTest = EDepthTest::Less;

	bool m_isBlendEnabled = false;
	bool m_isDepthWriteEnabled = true;

private:
	Array<TextureInfo> m_textures;
	Array<ShaderParam> m_shaderParams;
};
