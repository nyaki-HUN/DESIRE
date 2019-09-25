#pragma once

#include <stdint.h>

class String;

class Socket
{
public:
	enum class EProtocol
	{
		TCP,
		UDP
	};

	Socket();
	~Socket();

	bool Open(EProtocol protocol = EProtocol::TCP);
	void Close();

	bool Connect(const String& address, uint16_t port);
	int Send(const void* buffer, size_t size);

	void SetNoDelay(bool value);
	void SetNonBlocking();

	static void Initialize();
	static void Shutdown();

private:
#if DESIRE_PLATFORM_WINDOWS || DESIRE_PLATFORM_XBOXONE
	static constexpr uintptr_t kInvalidSocketId = UINTPTR_MAX;

	uintptr_t socketId = kInvalidSocketId;
#else
	static constexpr int kInvalidSocketId = -1;

	int socketId = kInvalidSocketId;
#endif
};
