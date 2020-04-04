#pragma once

#include "Engine/Core/MemoryBuffer.h"
#include "Engine/Core/String/DynamicString.h"

class IReadFile
{
public:
	enum class ESeekOrigin
	{
		Begin,
		Current,
		End
	};

	IReadFile(int64_t fileSize, const String& filename = String::kEmptyString);
	virtual ~IReadFile();

	const String& GetFilename() const;

	virtual bool Seek(int64_t offset, ESeekOrigin origin = ESeekOrigin::Current) = 0;

	inline int64_t GetSize() const		{ return fileSize; }
	inline int64_t Tell() const			{ return position; }
	inline bool IsEof() const			{ return position >= fileSize; }

	virtual void ReadBufferAsync(void* buffer, size_t size, std::function<void()> callback);
	virtual size_t ReadBuffer(void* buffer, size_t size) = 0;
	size_t ReadString(WritableString& out);
	MemoryBuffer ReadAllContents();

	template<typename T>
	bool Read(T& val)
	{
		static_assert(std::is_pod<T>::value);

		size_t numBytesRead = ReadBuffer(&val, sizeof(T));
		return numBytesRead == sizeof(T);
	}

protected:
	int64_t fileSize;
	int64_t position;
	DynamicString filename;
};
