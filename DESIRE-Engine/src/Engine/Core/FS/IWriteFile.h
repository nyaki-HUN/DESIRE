#pragma once

class IWriteFile
{
public:
	IWriteFile();
	virtual ~IWriteFile();

	virtual size_t WriteBuffer(const void* buffer, size_t size) = 0;
	bool WriteString(const char* str, size_t len = SIZE_MAX);

	template<typename T>
	bool Write(const T& val)
	{
		static_assert(std::is_pod<T>::value);

		size_t numBytesWritten = WriteBuffer(&val, sizeof(T));
		return numBytesWritten == sizeof(T);
	}

protected:
};
