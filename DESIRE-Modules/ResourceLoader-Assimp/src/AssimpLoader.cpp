#include "stdafx_Assimp.h"
#include "AssimpLoader.h"

#include "Engine/Core/FS/IReadFile.h"

#include "Engine/Resource/Mesh.h"

class AssimpIOStreamWrapper : public Assimp::IOStream
{
public:
	AssimpIOStreamWrapper(const ReadFilePtr& file)
		: file(file)
	{
	}

	size_t Read(void* pBuffer, size_t size, size_t count) override
	{
		return file->ReadBuffer(pBuffer, size * count);
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

		const bool result = file->Seek(static_cast<int64_t>(offset), mapping[origin]);
		return result ? aiReturn_SUCCESS : aiReturn_FAILURE;
	}

	size_t Tell() const override
	{
		return static_cast<size_t>(file->Tell());
	}

	size_t FileSize() const override
	{
		return static_cast<size_t>(file->GetSize());
	}

	void Flush() override
	{
		ASSERT(false && "Not supported");
	}

private:
	const ReadFilePtr& file;
};

class AssimpIOSystemWrapper : public Assimp::IOSystem
{
public:
	AssimpIOSystemWrapper(const ReadFilePtr& file)
		: file(file)
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

		return new AssimpIOStreamWrapper(file);
	}

	void Close(Assimp::IOStream* pFile) override
	{
		delete pFile;
	}

	static constexpr const char* kDummyFilename = "__DUMMY__";

private:
	const ReadFilePtr& file;
};

std::unique_ptr<Mesh> AssimpLoader::Load(const ReadFilePtr& file)
{
	Assimp::Importer importer;
	importer.SetIOHandler(new AssimpIOSystemWrapper(file));

//	const aiScene* pScene = importer.ReadFile(AssimpIOSystemWrapper::kDummyFilename, aiProcessPreset_TargetRealtime_Quality);
	const aiScene* pScene = importer.ReadFile(AssimpIOSystemWrapper::kDummyFilename, aiProcess_CalcTangentSpace);

	if(pScene != nullptr && pScene->HasMeshes())
	{
		for(uint32_t i = 0; i < pScene->mNumMeshes; ++i)
		{
			const aiMesh* pMesh = pScene->mMeshes[i];
		}
	}

	return nullptr;
}
