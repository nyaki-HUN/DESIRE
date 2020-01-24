#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_PS4

#include "Engine/Network/TcpSocket.h"
#include "Engine/Core/Log/Log.h"
#include "Engine/Core/String/String.h"

#include <sys/socket.h>
#include <net.h>

TcpSocket::TcpSocket()
{
	socketId = sceNetSocket("TcpSocket", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, SCE_NET_IPPROTO_TCP);
}

TcpSocket::~TcpSocket()
{
	if(socketId != kInvalidSocketId)
	{
		sceNetSocketClose(socketId);
		socketId = kInvalidSocketId;
	}
}

bool TcpSocket::Connect(const String& address, uint16_t port)
{
	SceNetSockaddrIn addr;
	addr.sin_family = SCE_NET_AF_INET;
	addr.sin_port = sceNetHtons(port);
	sceNetInetPton(SCE_NET_AF_INET, address.Str(), &addr.sin_addr);

	int result = sceNetConnect(socketId, reinterpret_cast<const SceNetSockaddr*>(&addr), sizeof(SceNetSockaddrIn));
	if(result < 0)
	{
		LOG_MESSAGE("Socket connection to %s:%u failed with status %d", address.Str(), port, sce_net_errno);
		return false;
	}

	return true;
}

int TcpSocket::Send(const void* buffer, size_t size)
{
	return sceNetSend(socketId, buffer, size, 0);
}

void TcpSocket::SetNoDelay(bool value)
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	int optval = value ? 1 : 0;
	sceNetSetsockopt(socketId, SCE_NET_SOL_SOCKET, SCE_NET_TCP_NODELAY, &optval, sizeof(optval));
}

void TcpSocket::SetNonBlocking()
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	uint32_t optval = 1;
	sceNetSetsockopt(socketId, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &optval, sizeof(optval));
}

void TcpSocket::Initialize()
{
	// Note: No need to call the sceNetInit()/sceNetTerm() functions anymore.
}

void TcpSocket::Shutdown()
{
}

#endif	// #if DESIRE_PLATFORM_PS4
