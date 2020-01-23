#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_PS4

#include "Engine/Network/Socket.h"
#include "Engine/Core/Log/Log.h"
#include "Engine/Core/String/String.h"

#include <sys/socket.h>
#include <net.h>

Socket::Socket()
{
}

Socket::~Socket()
{
	Close();
}

void Socket::Initialize()
{
	// Note: No need to call the sceNetInit()/sceNetTerm() functions anymore.
}

void Socket::Shutdown()
{
}

bool Socket::Open(EProtocol protocol)
{
	if(socketId != kInvalidSocketId)
	{
		return false;
	}

	switch(protocol)
	{
		case EProtocol::TCP:	socketId = sceNetSocket("TcpSocket", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, SCE_NET_IPPROTO_TCP); break;
		case EProtocol::UDP:	socketId = sceNetSocket("UdpSocket", SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, SCE_NET_IPPROTO_UDP); break;
	}

	return (socketId != kInvalidSocketId);
}

void Socket::Close()
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	sceNetSocketClose(socketId);
	socketId = kInvalidSocketId;
}

bool Socket::Connect(const String& address, uint16_t port)
{
	SceNetSockaddrIn addr;
	addr.sin_family = SCE_NET_AF_INET;
	addr.sin_port = sceNetHtons(port);
	sceNetInetPton(SCE_NET_AF_INET, address.Str(), &addr.sin_addr);

	int result = sceNetConnect(socketId, reinterpret_cast<const SceNetSockaddr*>(&addr), sizeof(addr));
	if(result < 0)
	{
		LOG_MESSAGE("Socket connection to %s:%u failed with status %d", address.Str(), port, sce_net_errno);
		return false;
	}

	return true;
}

int Socket::Send(const void* buffer, size_t size)
{
	return sceNetSend(socketId, buffer, size, 0);
}

void Socket::SetNoDelay(bool value)
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	int optval = value ? 1 : 0;
	sceNetSetsockopt(socketId, SCE_NET_SOL_SOCKET, SCE_NET_TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::SetNonBlocking()
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	uint32_t optval = 1;
	sceNetSetsockopt(socketId, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &optval, sizeof(optval));
}

#endif	// #if DESIRE_PLATFORM_PS4
