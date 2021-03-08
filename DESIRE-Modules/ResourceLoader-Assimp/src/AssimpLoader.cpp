#include "stdafx_Assimp.h"
#include "AssimpLoader.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/ResourceManager.h"

#include "Engine/Core/FS/IReadFile.h"

#include "Engine/Render/Material.h"
#include "Engine/Render/Mesh.h"

class AssimpIOStreamWrapper : public Assimp::IOStream
{
public:
	AssimpIOStreamWrapper(const ReadFilePtr& spFile)
		: m_spFile(spFile)
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
	const ReadFilePtr& m_spFile;
};

class AssimpIOSystemWrapper : public Assimp::IOSystem
{
public:
	AssimpIOSystemWrapper(const ReadFilePtr& spFile)
		: m_spFile(spFile)
	{
	}

	bool Exists(const char* pFile) const override
	{
		return (strcmp(pFile, kDummyFilename) == 0);
	}

	char getOsSeparator() const override
	{
		return '/';
	}

	Assimp::IOStream* Open(const char* pFile, const char* pMode) override
	{
		DESIRE_UNUSED(pMode);

		if(!Exists(pFile))
		{
			return nullptr;
		}

		return new AssimpIOStreamWrapper(m_spFile);
	}

	void Close(Assimp::IOStream* pFile) override
	{
		delete pFile;
	}

	static constexpr char kDummyFilename[] = "__DUMMY__";

private:
	const ReadFilePtr& m_spFile;
};

std::unique_ptr<Mesh> AssimpLoader::Load(const ReadFilePtr& spFile)
{
	Assimp::Importer importer;
	importer.SetIOHandler(new AssimpIOSystemWrapper(spFile));

	importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES, 24);

	constexpr uint32_t loadFlags = aiProcessPreset_TargetRealtime_Quality |
		aiProcess_ConvertToLeftHanded |
		aiProcess_TransformUVCoords |
		aiProcess_SplitByBoneCount;
//		aiProcess_OptimizeGraph;

	const aiScene* pScene = importer.ReadFile(AssimpIOSystemWrapper::kDummyFilename, loadFlags);

	if(pScene && pScene->HasMeshes())
	{
		for(uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; ++meshIdx)
		{
			const aiMesh* pSceneMesh = pScene->mMeshes[meshIdx];
			if(!pSceneMesh->HasPositions())
			{
				continue;
			}

			Array<Mesh::VertexLayout> vertexLayout;
			vertexLayout.Add({ Mesh::EAttrib::Position, 3, Mesh::EAttribType::Float });

			if(pSceneMesh->mNormals != nullptr)
			{
				vertexLayout.Add({ Mesh::EAttrib::Normal, 3, Mesh::EAttribType::Float });
			}

			if(pSceneMesh->mTangents != nullptr)
			{
				vertexLayout.Add({ Mesh::EAttrib::Tangent, 3, Mesh::EAttribType::Float });
			}

			const uint32_t numColorChannels = pSceneMesh->GetNumUVChannels();
			if(numColorChannels > 0)
			{
				vertexLayout.Add({ Mesh::EAttrib::Color, 4, Mesh::EAttribType::Float });
			}

			const uint32_t numUvChannels = pSceneMesh->GetNumUVChannels();
			for(uint32_t i = 0; i < numUvChannels; ++i)
			{
				const Mesh::EAttrib attrib = static_cast<Mesh::EAttrib>(static_cast<uint32_t>(Mesh::EAttrib::Texcoord0) + i);
				ASSERT(attrib < Mesh::EAttrib::Num);
				const uint8_t count = static_cast<uint8_t>(pSceneMesh->mNumUVComponents[i]);
				vertexLayout.Add({ attrib, count, Mesh::EAttribType::Float });
			}

			std::shared_ptr<Mesh> spMesh = std::make_shared<Mesh>(std::move(vertexLayout), pSceneMesh->mNumFaces * 3, pSceneMesh->mNumVertices);

			const uint32_t vertexSize = spMesh->GetVertexSize();
			float* pVertex = spMesh->m_spVertices.get();
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

			for(uint32_t i = 0; i < pSceneMesh->mNumFaces; ++i)
			{
				ASSERT(pSceneMesh->mFaces[i].mNumIndices == 3);
				spMesh->m_spIndices[i * 3 + 0] = pSceneMesh->mFaces[i].mIndices[0];
				spMesh->m_spIndices[i * 3 + 1] = pSceneMesh->mFaces[i].mIndices[1];
				spMesh->m_spIndices[i * 3 + 2] = pSceneMesh->mFaces[i].mIndices[2];
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

	return nullptr;
}
