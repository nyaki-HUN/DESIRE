#pragma once

class MeshRenderDataD3D11
{
public:
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;

	std::unique_ptr<D3D11_INPUT_ELEMENT_DESC[]> vertexElementDesc;
};
