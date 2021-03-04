#pragma once

class String;

class TcpSocket
{
public:
	TcpSocket();
	~TcpSocket();

	bool Connect(const String& address, uint16_t port);
	int Send(const void* pBuffer, size_t size);

	void SetNoDelay(bool value);
	void SetNonBlocking();

	static void Initialize();
	static void Shutdown();

private:
#if DESIRE_PLATFORM_WINDOWS || DESIRE_PLATFORM_XBOXONE
	typedef uintptr_t NativeSocket;
	static constexpr NativeSocket kInvalidSocketId = UINTPTR_MAX;
#else
	typedef int32_t NativeSocket;
	static constexpr NativeSocket kInvalidSocketId = -1;
#endif

	NativeSocket m_socketId = kInvalidSocketId;
};
