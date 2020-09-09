#include "Engine/stdafx.h"
#include "Engine/Render/Render.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/RenderTarget.h"
#include "Engine/Render/View.h"

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

Render::Render()
{
	screenSpaceQuadVertexShader = std::make_unique<Shader>("vs_screenSpaceQuad");
}

Render::~Render()
{
}

void Render::BeginFrame(OSWindow& window)
{
	SetRenderTarget(nullptr);
	pActiveRenderTarget = nullptr;

	ClearActiveRenderTarget();
	SetViewport(0, 0, window.GetWidth(), window.GetHeight());

	pActiveWindow = &window;
}

void Render::RenderMesh(Mesh* pMesh, Material* pMaterial, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices)
{
	if(pMesh == nullptr)
	{
		ASSERT(false && "Invalid mesh");
		return;
	}

	if(pMaterial == nullptr || pMaterial->vertexShader == nullptr || pMaterial->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(numIndices == UINT32_MAX)
	{
		// Use all the indices
		numIndices = pMesh->GetNumIndices() - indexOffset;
	}

	if(numVertices == UINT32_MAX)
	{
		// Use all the vertices
		numVertices = pMesh->GetNumVertices() - vertexOffset;
	}

	if(indexOffset + numIndices > pMesh->GetNumIndices() ||
		vertexOffset + numVertices > pMesh->GetNumVertices())
	{
		return;
	}

	if(pMesh->pRenderData == nullptr)
	{
		Bind(pMesh);
	}
	else if(pMesh->GetType() == Mesh::EType::Dynamic)
	{
		DynamicMesh* pDynamicMesh = static_cast<DynamicMesh*>(pMesh);
		UpdateDynamicMesh(*pDynamicMesh);
	}

	if(pActiveMesh != pMesh)
	{
		SetMesh(pMesh);
		pActiveMesh = pMesh;
	}

	SetMaterial(pMaterial);
	UpdateShaderParams(*pMaterial);

	DoRender(indexOffset, vertexOffset, numIndices, numVertices);
}

void Render::RenderScreenSpaceQuad(Material* pMaterial)
{
	ASSERT(screenSpaceQuadVertexShader->pRenderData != nullptr && "Shader needs to be bound by the render module");

	if(pMaterial == nullptr || pMaterial->fragmentShader == nullptr)
	{
		ASSERT(false && "Invalid material");
		return;
	}

	if(pActiveMesh != nullptr)
	{
		SetMesh(nullptr);
		pActiveMesh = nullptr;
	}

	SetMaterial(pMaterial);
	SetVertexShader(screenSpaceQuadVertexShader.get());
	UpdateShaderParams(*pMaterial);

	DoRender(0, 0, 0, 4);
}

void Render::SetView(View* pView)
{
	if(pView != nullptr)
	{
		SetActiveRenderTarget(pView->GetRenderTarget());
		ClearActiveRenderTarget();
		SetViewport(pView->GetPosX(), pView->GetPosY(), pView->GetWidth(), pView->GetHeight());
	}
	else
	{
		SetRenderTarget(nullptr);
		pActiveRenderTarget = nullptr;

		if(pActiveWindow != nullptr)
		{
			SetViewport(0, 0, pActiveWindow->GetWidth(), pActiveWindow->GetHeight());
		}
	}
}

void Render::SetActiveRenderTarget(RenderTarget& renderTarget)
{
	if(renderTarget.pRenderData == nullptr)
	{
		Bind(renderTarget);
	}

	if(pActiveRenderTarget != &renderTarget)
	{
		SetRenderTarget(&renderTarget);
		pActiveRenderTarget = &renderTarget;
	}
}

void Render::SetBlendMode(EBlend srcBlend, EBlend destBlend, EBlendOp blendOp)
{
	SetBlendModeSeparated(srcBlend, destBlend, blendOp, srcBlend, destBlend, blendOp);
}

void Render::Bind(Mesh* pMesh)
{
	if(pMesh == nullptr || pMesh->pRenderData != nullptr)
	{
		return;
	}

	pMesh->pRenderData = CreateMeshRenderData(pMesh);
}

void Render::Bind(Shader* pShader)
{
	if(pShader == nullptr || pShader->pRenderData != nullptr)
	{
		return;
	}

	pShader->pRenderData = CreateShaderRenderData(pShader);
}

void Render::Bind(Texture* pTexture)
{
	if(pTexture == nullptr || pTexture->pRenderData != nullptr)
	{
		return;
	}

	pTexture->pRenderData = CreateTextureRenderData(pTexture);
}

void Render::Bind(RenderTarget& renderTarget)
{
	if(renderTarget.pRenderData != nullptr)
	{
		return;
	}

	const uint8_t textureCount = renderTarget.GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		const std::shared_ptr<Texture>& texture = renderTarget.GetTexture(i);
		Bind(texture.get());
	}

	renderTarget.pRenderData = CreateRenderTargetRenderData(renderTarget);
}

void Render::Unbind(Mesh& mesh)
{
	if(mesh.pRenderData == nullptr)
	{
		return;
	}

	DestroyMeshRenderData(mesh.pRenderData);
	mesh.pRenderData = nullptr;

	if(pActiveMesh == &mesh)
	{
		pActiveMesh = nullptr;
	}
}

void Render::Unbind(Shader& shader)
{
	if(shader.pRenderData == nullptr)
	{
		return;
	}

	DestroyShaderRenderData(shader.pRenderData);
	shader.pRenderData = nullptr;

	if(pActiveVertexShader == &shader)
	{
		pActiveVertexShader = nullptr;
	}
	if(pActiveFragmentShader == &shader)
	{
		pActiveFragmentShader = nullptr;
	}
}

void Render::Unbind(Texture& texture)
{
	if(texture.pRenderData == nullptr)
	{
		return;
	}

	DestroyTextureRenderData(texture.pRenderData);
	texture.pRenderData = nullptr;
}

void Render::Unbind(RenderTarget& renderTarget)
{
	if(renderTarget.pRenderData == nullptr)
	{
		return;
	}

	DestroyRenderTargetRenderData(renderTarget.pRenderData);
	renderTarget.pRenderData = nullptr;

	const uint8_t textureCount = renderTarget.GetTextureCount();
	for(uint8_t i = 0; i < textureCount; ++i)
	{
		Unbind(*renderTarget.GetTexture(i));
	}

	if(pActiveRenderTarget == &renderTarget)
	{
		pActiveRenderTarget = nullptr;
	}
}

void Render::SetDefaultRenderStates()
{
	SetColorWriteEnabled(true, true, true, true);
	SetDepthWriteEnabled(true);
	SetDepthTest(EDepthTest::Less);
	SetCullMode(ECullMode::CCW);
}

void Render::SetMaterial(Material* pMaterial)
{
	// Vertex shader
	if(pMaterial->vertexShader != nullptr)
	{
		if(pMaterial->vertexShader->pRenderData == nullptr)
		{
			Bind(pMaterial->vertexShader.get());
		}

		SetVertexShader(pMaterial->vertexShader.get());
	}

	// Fragment shader
	if(pMaterial->fragmentShader != nullptr)
	{
		if(pMaterial->fragmentShader->pRenderData == nullptr)
		{
			Bind(pMaterial->fragmentShader.get());
		}

		SetFragmentShader(pMaterial->fragmentShader.get());
	}

	// Textures
	uint8_t samplerIdx = 0;
	for(const Material::TextureInfo& textureInfo : pMaterial->GetTextures())
	{
		if(textureInfo.texture->pRenderData == nullptr)
		{
			Bind(textureInfo.texture.get());
		}

		SetTexture(samplerIdx, *textureInfo.texture, textureInfo.filterMode, textureInfo.addressMode);
		samplerIdx++;
	}
}
