#pragma once

class DynamicMesh;
class Material;
class Matrix4;
class Mesh;
class OSWindow;
class RenderTarget;
class Shader;
class String;
class Texture;
class View;
class WritableString;

class Render
{
public:
	enum class EDepthTest
	{
		Disabled,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual
	};

	enum class ECullMode
	{
		None,
		CCW,
		CW
	};

	enum class EBlend
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

	enum class EBlendOp
	{
		Add,
		Subtract,
		RevSubtract,
		Min,
		Max
	};

	enum class EAddressMode
	{
		Repeat,
		Clamp,
		MirroredRepeat,
		MirrorOnce,
		Border
	};

	enum class EFilterMode
	{
		Point,			// No filtering, the texel with coordinates nearest to the desired pixel value is used (at most 1 texel being sampled)
		Bilinear,		// Texture samples are averaged (at most 4 samples)
		Trilinear,		// Texture samples are averaged and also blended between mipmap levels (at most 8 samples)
		Anisotropic,	// Use anisotropic interpolation
	};

	Render();
	virtual ~Render();

	virtual void Init(OSWindow* pMainWindow) = 0;
	virtual void UpdateRenderWindow(OSWindow* pWindow) = 0;
	virtual void Kill() = 0;

	virtual void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const = 0;

	virtual void BeginFrame(OSWindow* pWindow) = 0;
	virtual void EndFrame() = 0;

	void RenderMesh(Mesh* pMesh, Material* pMaterial);
	void RenderScreenSpaceQuad(Material* pMaterial);

	// Sets the current view that the rendering will happen on. (Use nullptr to set the default view which is using the frame buffer)
	virtual void SetView(View* pView) = 0;

	virtual void SetWorldMatrix(const Matrix4& worldMatrix) = 0;
	virtual void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) = 0;

	// Render state setup
	virtual void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) = 0;
	virtual void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
	virtual void SetColorWriteEnabled(bool r, bool g, bool b, bool a) = 0;
	virtual void SetDepthWriteEnabled(bool enabled) = 0;
	virtual void SetDepthTest(EDepthTest deptTest) = 0;
	virtual void SetCullMode(ECullMode cullMode) = 0;
	void SetBlendMode(EBlend srcBlend, EBlend destBlend, EBlendOp blendOp = EBlendOp::Add);
	virtual void SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha) = 0;
	virtual void SetBlendModeDisabled() = 0;

	// Resource bind
	virtual void Bind(Mesh* pMesh) = 0;
	virtual void Bind(Shader* pShader) = 0;
	virtual void Bind(Texture* pTexture) = 0;
	virtual void Bind(RenderTarget* pRenderTarget) = 0;

	// Resource unbind
	virtual void Unbind(Mesh* pMesh) = 0;
	virtual void Unbind(Shader* pShader) = 0;
	virtual void Unbind(Texture* pTexture) = 0;
	virtual void Unbind(RenderTarget* pRenderTarget) = 0;

protected:
	void SetDefaultRenderStates();

	virtual void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;

	std::unique_ptr<Shader> screenSpaceQuadVertexShader;

private:
	virtual void SetMesh(Mesh* pMesh) = 0;
	virtual void UpdateDynamicMesh(DynamicMesh& dynamicMesh) = 0;
	void SetMaterial(Material* pMaterial);
	virtual void SetVertexShader(Shader* pVertexShader) = 0;
	virtual void SetFragmentShader(Shader* pFragmentShader) = 0;
	virtual void SetTexture(uint8_t samplerIdx, Texture* pTexture, EFilterMode filterMode, EAddressMode addressMode = EAddressMode::Repeat) = 0;
	virtual void UpdateShaderParams(const Material* pMaterial) = 0;

	// Submit draw command
	virtual void DoRender() = 0;
};
