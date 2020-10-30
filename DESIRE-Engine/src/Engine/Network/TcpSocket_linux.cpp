#include "Engine/stdafx.h"
#include "Engine/Network/TcpSocket.h"

#if DESIRE_PLATFORM_LINUX

#include "Engine/Core/String/String.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

TcpSocket::TcpSocket()
{
	m_socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(m_socketId != kInvalidSocketId)
	{
		int optval = 1;
		setsockopt(m_socketId, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
	}
}

TcpSocket::~TcpSocket()
{
	if(m_socketId != kInvalidSocketId)
	{
		close(m_socketId);
		m_socketId = kInvalidSocketId;
	}
}

bool TcpSocket::Connect(const String& address, uint16_t port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, address.Str(), &addr.sin_addr);

	int result = connect(m_socketId, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in));
	if(result < 0)
	{
		LOG_MESSAGE("Socket connection to %s:%u failed with status %d", address.Str(), port, errno);
		return false;
	}

	return true;
}

int TcpSocket::Send(const void* pBuffer, size_t size)
{
	return send(m_socketId, pBuffer, size, 0);
}

void TcpSocket::SetNoDelay(bool value)
{
	if(m_socketId == kInvalidSocketId)
	{
		return;
	}

	int optval = value ? 1 : 0;
	setsockopt(m_socketId, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void TcpSocket::SetNonBlocking()
{
	if(m_socketId == kInvalidSocketId)
	{
		return;
	}

	int flags = fcntl(m_socketId, F_GETFL, 0);
	fcntl(m_socketId, F_SETFL, flags | O_NONBLOCK);
}

void TcpSocket::Initialize()
{
}

void TcpSocket::Shutdown()
{
}

#endif	// #if DESIRE_PLATFORM_LINUX
