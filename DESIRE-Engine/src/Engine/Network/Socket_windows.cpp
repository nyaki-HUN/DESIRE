#include "Engine/stdafx.h"

#if DESIRE_PLATFORM_WINDOWS || DESIRE_PLATFORM_XBOXONE

#include "Engine/Network/Socket.h"
#include "Engine/Core/Log/Log.h"
#include "Engine/Core/String/String.h"

#include <WS2tcpip.h>

Socket::Socket()
{
}

Socket::~Socket()
{
	Close();
}

void Socket::Initialize()
{
	// Initialize Winsock2
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void Socket::Shutdown()
{
	WSACleanup();
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

	closesocket(socketId);
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
		LOG_MESSAGE("Socket connection to %s:%u failed with status %d", address.Str(), port, WSAGetLastError());
		return false;
	}

	return true;
}

int Socket::Send(const void* buffer, size_t size)
{
	ASSERT(size <= INT_MAX);
	return send(socketId, static_cast<const char*>(buffer), static_cast<int>(size), 0);
}

void Socket::SetNoDelay(bool value)
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	int optval = value ? 1 : 0;
	setsockopt(socketId, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&optval), sizeof(optval));
}

void Socket::SetNonBlocking()
{
	if(socketId == kInvalidSocketId)
	{
		return;
	}

	u_long mode = 1;
	ioctlsocket(socketId, FIONBIO, &mode);
}

#endif	// #if DESIRE_PLATFORM_WINDOWS
