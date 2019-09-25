#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_LINUX || DESIRE_PLATFORM_OSX

#include "Engine/Network/Socket.h"
#include "Engine/Core/Log/Log.h"
#include "Engine/Core/String/String.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

Socket::Socket()
{
}

Socket::~Socket()
{
	Close();
}

void Socket::Initialize()
{
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
		case EProtocol::TCP:	socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); break;
		case EProtocol::UDP:	socketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); break;
	}

	return (socketId != kInvalidSocketId);
}

void Socket::Close()
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	close(socketId);
	socketId = kInvalidSocketId;
}

bool Socket::Connect(const String& address, uint16_t port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, address.Str(), &addr.sin_addr);

	int result = connect(socketId, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in));
	if(result < 0)
	{
		LOG_MESSAGE("Socket connection to %s:%u failed with status %d", address.Str(), port, errno);
		return false;
	}

	return true;
}

int Socket::Send(const void* buffer, size_t size)
{
	return send(socketId, buffer, size, 0);
}

void Socket::SetNoDelay(bool value)
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	int optval = value ? 1 : 0;
	setsockopt(socketId, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::SetNonBlocking()
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	int flags = fcntl(socketId, F_GETFL, 0);
	fcntl(socketId, F_SETFL, flags | O_NONBLOCK);
}

#endif	// #if DESIRE_PLATFORM_LINUX
