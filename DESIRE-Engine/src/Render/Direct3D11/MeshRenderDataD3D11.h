#pragma once

#include <D3D11.h>

class MeshRenderDataD3D11
{
public:
	ID3D11Buffer *indexBuffer;
	ID3D11Buffer *vertexBuffer;
};
