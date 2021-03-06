#include "stdafx_Assimp.h"
#include "AssimpLoader.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/ResourceManager.h"

#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/Object.h"

#include "Engine/Render/IndexBuffer.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/Renderable.h"
#include "Engine/Render/RenderComponent.h"
#include "Engine/Render/VertexBuffer.h"

class AssimpIOStreamWrapper : public Assimp::IOStream
{
public:
	AssimpIOStreamWrapper(ReadFilePtr spFile)
		: m_spFile(std::move(spFile))
	{
	}

	size_t Read(void* pBuffer, size_t size, size_t count) override
	{
		return m_spFile->ReadBuffer(pBuffer, size * count);
	}

	size_t Write(const void* pBuffer, size_t size, size_t count) override
	{
		DESIRE_UNUSED(pBuffer);
		DESIRE_UNUSED(size);
		DESIRE_UNUSED(count);

		ASSERT(false && "Not supported");
		return 0;
	}

	aiReturn Seek(size_t offset, aiOrigin origin) override
	{
		const IReadFile::ESeekOrigin mapping[] =
		{
			IReadFile::ESeekOrigin::Begin,		// aiOrigin::aiOrigin_SET
			IReadFile::ESeekOrigin::Current,	// aiOrigin::aiOrigin_CUR
			IReadFile::ESeekOrigin::End,		// aiOrigin::aiOrigin_END
		};

		const bool result = m_spFile->Seek(static_cast<int64_t>(offset), mapping[origin]);
		return result ? aiReturn_SUCCESS : aiReturn_FAILURE;
	}

	size_t Tell() const override
	{
		return static_cast<size_t>(m_spFile->Tell());
	}

	size_t FileSize() const override
	{
		return static_cast<size_t>(m_spFile->GetSize());
	}

	void Flush() override
	{
		ASSERT(false && "Not supported");
	}

private:
	ReadFilePtr m_spFile;
};

class AssimpIOSystemWrapper : public Assimp::IOSystem
{
public:
	bool Exists(const char* pFile) const override
	{
		// TODO: Proper exists function
		return pFile && FileSystem::Get().Open(String(pFile, strlen(pFile))) != nullptr;
	}

	char getOsSeparator() const override
	{
		return '/';
	}

	Assimp::IOStream* Open(const char* pFile, const char* pMode) override
	{
		DESIRE_UNUSED(pMode);

		ReadFilePtr spFile = FileSystem::Get().Open(String(pFile, strlen(pFile)));
		return spFile ? new AssimpIOStreamWrapper(std::move(spFile)) : nullptr;
	}

	void Close(Assimp::IOStream* pFile) override
	{
		delete pFile;
	}
};

std::unique_ptr<Object> AssimpLoader::Load(const String& filename)
{
	Assimp::Importer importer;
	importer.SetIOHandler(new AssimpIOSystemWrapper());

	importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES, 24);

	constexpr uint32_t loadFlags = aiProcessPreset_TargetRealtime_Quality |
		aiProcess_ConvertToLeftHanded |
		aiProcess_TransformUVCoords |
		aiProcess_SplitByBoneCount;
//		aiProcess_OptimizeGraph;

	std::unique_ptr<Object> spObject = std::make_unique<Object>();
	spObject->SetObjectName(filename);

	const aiScene* pScene = importer.ReadFile(filename.Str(), loadFlags);
	if(!pScene || !pScene->mRootNode || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		return spObject;
	}

	if(pScene->HasMeshes())
	{
		for(uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; ++meshIdx)
		{
			const aiMesh* pSceneMesh = pScene->mMeshes[meshIdx];
			if(!pSceneMesh->HasPositions())
			{
				continue;
			}

			Renderable* pRenderable = new Renderable();

			Array<VertexBuffer::Layout> vertexLayout;
			vertexLayout.Add({ VertexBuffer::EAttrib::Position, 3, VertexBuffer::EAttribType::Float });

			if(pSceneMesh->mNormals != nullptr)
			{
				vertexLayout.Add({ VertexBuffer::EAttrib::Normal, 3, VertexBuffer::EAttribType::Float });
			}

			if(pSceneMesh->mTangents != nullptr)
			{
				vertexLayout.Add({ VertexBuffer::EAttrib::Tangent, 3, VertexBuffer::EAttribType::Float });
			}

			const uint32_t numColorChannels = pSceneMesh->GetNumUVChannels();
			if(numColorChannels > 0)
			{
				vertexLayout.Add({ VertexBuffer::EAttrib::Color, 4, VertexBuffer::EAttribType::Float });
			}

			const uint32_t numUvChannels = pSceneMesh->GetNumUVChannels();
			for(uint32_t i = 0; i < numUvChannels; ++i)
			{
				const VertexBuffer::EAttrib attrib = static_cast<VertexBuffer::EAttrib>(static_cast<uint32_t>(VertexBuffer::EAttrib::Texcoord0) + i);
				ASSERT(attrib < VertexBuffer::EAttrib::Num);
				const uint8_t count = static_cast<uint8_t>(pSceneMesh->mNumUVComponents[i]);
				vertexLayout.Add({ attrib, count, VertexBuffer::EAttribType::Float });
			}

			// Index buffer
			pRenderable->m_spIndexBuffer = std::make_shared<IndexBuffer>(pSceneMesh->mNumFaces * 3);
			for(uint32_t i = 0; i < pSceneMesh->mNumFaces; ++i)
			{
				ASSERT(pSceneMesh->mFaces[i].mNumIndices == 3);
				pRenderable->m_spIndexBuffer->m_spIndices[i * 3 + 0] = pSceneMesh->mFaces[i].mIndices[0];
				pRenderable->m_spIndexBuffer->m_spIndices[i * 3 + 1] = pSceneMesh->mFaces[i].mIndices[1];
				pRenderable->m_spIndexBuffer->m_spIndices[i * 3 + 2] = pSceneMesh->mFaces[i].mIndices[2];
			}

			// Vertex buffer
			pRenderable->m_spVertexBuffer = std::make_shared<VertexBuffer>(pSceneMesh->mNumVertices, std::move(vertexLayout));
			const uint32_t vertexSize = pRenderable->m_spVertexBuffer->GetVertexSize();
			float* pVertex = pRenderable->m_spVertexBuffer->m_spVertices.get();
			for(uint32_t i = 0; i < pSceneMesh->mNumVertices; ++i)
			{
				uint32_t j = 0;

				pVertex[j++] = pSceneMesh->mVertices[i].x;
				pVertex[j++] = pSceneMesh->mVertices[i].y;
				pVertex[j++] = pSceneMesh->mVertices[i].z;

				if(pSceneMesh->mNormals != nullptr)
				{
					pVertex[j++] = pSceneMesh->mNormals[i].x;
					pVertex[j++] = pSceneMesh->mNormals[i].y;
					pVertex[j++] = pSceneMesh->mNormals[i].z;
				}

				if(pSceneMesh->mTangents != nullptr)
				{
					pVertex[j++] = pSceneMesh->mTangents[i].x;
					pVertex[j++] = pSceneMesh->mTangents[i].y;
					pVertex[j++] = pSceneMesh->mTangents[i].z;
				}

				if(numColorChannels > 0)
				{
					pVertex[j++] = pSceneMesh->mColors[0][i].r;
					pVertex[j++] = pSceneMesh->mColors[0][i].g;
					pVertex[j++] = pSceneMesh->mColors[0][i].b;
					pVertex[j++] = pSceneMesh->mColors[0][i].a;
				}

				for(uint32_t channelIdx = 0; channelIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++channelIdx)
				{
					if(pSceneMesh->HasTextureCoords(channelIdx))
					{
						pVertex[j++] = pSceneMesh->mTextureCoords[channelIdx][i].x;

						if(pSceneMesh->mNumUVComponents[i] > 1)
						{
							pVertex[j++] = pSceneMesh->mTextureCoords[channelIdx][i].y;
						}

						if(pSceneMesh->mNumUVComponents[i] > 2)
						{
							pVertex[j++] = pSceneMesh->mTextureCoords[channelIdx][i].z;
						}
					}
				}

				pVertex += vertexSize;
			}
		}

		for(uint32_t materialIdx = 0; materialIdx < pScene->mNumMaterials; ++materialIdx)
		{
			std::shared_ptr<Material> spMaterial = std::make_shared<Material>();

			const aiMaterial* pMaterial = pScene->mMaterials[materialIdx];
			if(pMaterial)
			{
				for(uint32_t propertyIdx = 0; propertyIdx < pScene->mNumMaterials; ++propertyIdx)
				{
					const aiMaterialProperty* pProperty = pMaterial->mProperties[propertyIdx];
					if(pProperty)
					{
						if(pProperty->mSemantic == aiTextureType_NONE)
						{
							// Non-texture
						}
						else
						{
							// Texture
							aiString path;
							aiTextureMapping mapping;
							uint32_t uvindex = 0;
							ai_real blend;
							aiTextureOp op;
							aiTextureMapMode mapmode;
							if(pMaterial->GetTexture(static_cast<aiTextureType>(pProperty->mSemantic), pProperty->mIndex, &path, &mapping, &uvindex, &blend, &op, &mapmode) == AI_SUCCESS)
							{
								const String filename(path.C_Str(), path.length);
								EFilterMode filterMode = EFilterMode::Trilinear;

								EAddressMode addressMode = EAddressMode::Repeat;
								switch(mapmode)
								{
									case aiTextureMapMode_Wrap:		addressMode = EAddressMode::Repeat; break;
									case aiTextureMapMode_Clamp:	addressMode = EAddressMode::Clamp; break;
									case aiTextureMapMode_Decal:	addressMode = EAddressMode::Border; break;
									case aiTextureMapMode_Mirror:	addressMode = EAddressMode::MirroredRepeat; break;
								}

								spMaterial->AddTexture(Modules::Application->GetResourceManager().GetTexture(filename), filterMode, addressMode);
							}
						}
					}
				}
			}
		}

		for(uint32_t animIdx = 0; animIdx < pScene->mNumAnimations; animIdx++)
		{
		}
	}

	importer.FreeScene();

	return spObject;
}
