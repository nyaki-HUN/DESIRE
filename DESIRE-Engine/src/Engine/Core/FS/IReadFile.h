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

	IReadFile(size_t fileSize, const String& filename = String::kEmptyString);
	virtual ~IReadFile();

	const String& GetFilename() const;

	virtual bool Seek(int64_t offset, ESeekOrigin origin = ESeekOrigin::Current) = 0;

	size_t GetSize() const;
	size_t Tell() const;
	bool IsEof() const;

	virtual void ReadBufferAsync(void* pBuffer, size_t size, std::function<void()> callback);
	virtual size_t ReadBuffer(void* pBuffer, size_t size) = 0;
	bool ReadString(WritableString& string);

	template<typename T>
	bool Read(T& val)
	{
		static_assert(std::is_pod<T>::value);

		const size_t numBytesRead = ReadBuffer(&val, sizeof(T));
		return numBytesRead == sizeof(T);
	}

	DynamicString ReadAllAsText();
	MemoryBuffer ReadAllAsBinary();

protected:
	size_t m_fileSize = 0;
	size_t m_position = 0;
	DynamicString m_filename;
};
