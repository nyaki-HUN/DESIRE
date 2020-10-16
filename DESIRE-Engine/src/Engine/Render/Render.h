#pragma once

class DynamicMesh;
class Material;
class Matrix4;
class Mesh;
class OSWindow;
class Renderable;
class RenderData;
class RenderTarget;
class Shader;
class String;
class Texture;
class View;
class WritableString;

class Render
{
public:
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
	virtual void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = UINT16_MAX, uint16_t height = UINT16_MAX) = 0;
	virtual void SetWorldMatrix(const Matrix4& worldMatrix) = 0;
	virtual void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) = 0;

	// Resource unbind
	void Unbind(Renderable& renderable);
	void Unbind(Mesh& mesh);
	void Unbind(Shader& shader);
	void Unbind(Texture& texture);
	void Unbind(RenderTarget& renderTarget);

protected:
	const OSWindow* m_pActiveWindow = nullptr;
	const Mesh* m_pActiveMesh = nullptr;
	const RenderTarget* m_pActiveRenderTarget = nullptr;

private:
	virtual RenderData* CreateRenderableRenderData(const Renderable& renderable) = 0;
	virtual RenderData* CreateMeshRenderData(const Mesh& mesh) = 0;
	virtual RenderData* CreateShaderRenderData(const Shader& shader) = 0;
	virtual RenderData* CreateTextureRenderData(const Texture& texture) = 0;
	virtual RenderData* CreateRenderTargetRenderData(const RenderTarget& renderTarget) = 0;

	virtual void OnDestroyRenderableRenderData(RenderData* pRenderData)		{ DESIRE_UNUSED(pRenderData); }
	virtual void OnDestroyMeshRenderData(RenderData* pRenderData)			{ DESIRE_UNUSED(pRenderData); }
	virtual void OnDestroyShaderRenderData(RenderData* pRenderData)			{ DESIRE_UNUSED(pRenderData); }
	virtual void OnDestroyTextureRenderData(RenderData* pRenderData)		{ DESIRE_UNUSED(pRenderData); }
	virtual void OnDestroyRenderTargetRenderData(RenderData* pRenderData)	{ DESIRE_UNUSED(pRenderData); }

	virtual void SetMesh(Mesh& mesh) = 0;
	virtual void UpdateDynamicMesh(DynamicMesh& dynamicMesh) = 0;
	virtual void SetRenderTarget(RenderTarget* pRenderTarget) = 0;
	virtual void UpdateShaderParams(const Material& material) = 0;

	virtual void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) = 0;
};
