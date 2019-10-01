#include "AssimpLoader.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/FS/IReadFile.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/IOStream.hpp"
#include "assimp/IOSystem.hpp"

class AssimpIOStreamWrapper : public Assimp::IOStream
{
public:
	AssimpIOStreamWrapper(const ReadFilePtr& file)
		: file(file)
	{
	}

	size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override
	{
		return file->ReadBuffer(pvBuffer, pSize * pCount);
	}

	size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override
	{
		(void)pvBuffer;
		(void)pSize;
		(void)pCount;

		ASSERT(false && "Not supported");
		return 0;
	}

	aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
	{
		const IReadFile::ESeekOrigin mapping[] =
		{
			IReadFile::ESeekOrigin::Begin,		// aiOrigin::aiOrigin_SET
			IReadFile::ESeekOrigin::Current,	// aiOrigin::aiOrigin_CUR
			IReadFile::ESeekOrigin::End,		// aiOrigin::aiOrigin_END
		};

		const bool result = file->Seek(static_cast<int64_t>(pOffset), mapping[pOrigin]);
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
		return (strcmp(pFile, DUMMY_FILENAME) == 0);
	}

	char getOsSeparator() const override
	{
		return '/';
	}

	Assimp::IOStream* Open(const char* pFile, const char* pMode) override
	{
		(void)pMode;

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

	static const char* DUMMY_FILENAME;

private:
	const ReadFilePtr& file;
};

const char* AssimpIOSystemWrapper::DUMMY_FILENAME = "__DUMMY__";

Mesh* AssimpLoader::Load(const ReadFilePtr& file)
{
	Assimp::Importer importer;
	importer.SetIOHandler(new AssimpIOSystemWrapper(file));

//	const aiScene* scene = importer.ReadFile(AssimpIOSystemWrapper::DUMMY_FILENAME, aiProcessPreset_TargetRealtime_Quality);
	const aiScene* scene = importer.ReadFile(AssimpIOSystemWrapper::DUMMY_FILENAME, aiProcess_CalcTangentSpace);

	if(scene != nullptr && scene->HasMeshes())
	{
		for(uint32_t i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
		}
	}

	return nullptr;
}
