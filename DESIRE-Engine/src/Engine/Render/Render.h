#pragma once

class DynamicMesh;
class Material;
class Matrix4;
class Mesh;
class OSWindow;
class Renderable;
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

	virtual bool Init(OSWindow& mainWindow) = 0;
	virtual void UpdateRenderWindow(OSWindow& window) = 0;
	virtual void Kill() = 0;

	virtual void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const = 0;

	void BeginFrame(OSWindow& window);
	virtual void EndFrame() = 0;

	void RenderRenderable(Renderable& renderable, uint32_t indexOffset = 0, uint32_t vertexOffset = 0, uint32_t numIndices = UINT32_MAX, uint32_t numVertices = UINT32_MAX);

	void SetActiveRenderTarget(RenderTarget* pRenderTarget);
	virtual void Clear(uint32_t clearColorRGBA = 0x000000FF, float depth = 1.0f, uint8_t stencil = 0) = 0;

	virtual void SetWorldMatrix(const Matrix4& worldMatrix) = 0;
	virtual void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) = 0;

	// Render state setup
	virtual void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) = 0;
	virtual void SetColorWriteEnabled(bool r, bool g, bool b, bool a) = 0;
	virtual void SetDepthWriteEnabled(bool enabled) = 0;
	virtual void SetDepthTest(EDepthTest deptTest) = 0;
	virtual void SetCullMode(ECullMode cullMode) = 0;
	void SetBlendMode(EBlend srcBlend, EBlend destBlend, EBlendOp blendOp = EBlendOp::Add);
	virtual void SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha) = 0;
	virtual void SetBlendModeDisabled() = 0;

	// Resource unbind
	void Unbind(Renderable& renderable);
	void Unbind(Mesh& mesh);
	void Unbind(Shader& shader);
	void Unbind(Texture& texture);
	void Unbind(RenderTarget& renderTarget);

protected:
	void SetDefaultRenderStates();

	const OSWindow* m_pActiveWindow = nullptr;
	const Mesh* m_pActiveMesh = nullptr;
	const RenderTarget* m_pActiveRenderTarget = nullptr;

private:
	virtual void* CreateRenderableRenderData(const Renderable& renderable) = 0;
	virtual void* CreateMeshRenderData(const Mesh& mesh) = 0;
	virtual void* CreateShaderRenderData(const Shader& shader) = 0;
	virtual void* CreateTextureRenderData(const Texture& texture) = 0;
	virtual void* CreateRenderTargetRenderData(const RenderTarget& renderTarget) = 0;

	virtual void DestroyRenderableRenderData(void* pRenderData) = 0;
	virtual void DestroyMeshRenderData(void* pRenderData) = 0;
	virtual void DestroyShaderRenderData(void* pRenderData) = 0;
	virtual void DestroyTextureRenderData(void* pRenderData) = 0;
	virtual void DestroyRenderTargetRenderData(void* pRenderData) = 0;

	virtual void SetMesh(Mesh& mesh) = 0;
	virtual void UpdateDynamicMesh(DynamicMesh& dynamicMesh) = 0;
	virtual void SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode = EAddressMode::Repeat) = 0;
	virtual void SetRenderTarget(RenderTarget* pRenderTarget) = 0;
	virtual void UpdateShaderParams(const Material& material) = 0;

	virtual void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) = 0;
};
