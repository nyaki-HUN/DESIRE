#include "Engine/stdafx.h"
#include "Engine/Network/TcpSocket.h"

#if DESIRE_PLATFORM_WINDOWS || DESIRE_PLATFORM_XBOXONE

#include "Engine/Core/String/String.h"

#include <WS2tcpip.h>

TcpSocket::TcpSocket()
{
	m_socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TcpSocket::~TcpSocket()
{
	if(m_socketId != kInvalidSocketId)
	{
		closesocket(m_socketId);
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
		LOG_MESSAGE("Socket connection to %s:%u failed with status %d", address.Str(), port, WSAGetLastError());
		return false;
	}

	return true;
}

int TcpSocket::Send(const void* pBuffer, size_t size)
{
	ASSERT(size <= INT_MAX);
	return send(m_socketId, static_cast<const char*>(pBuffer), static_cast<int>(size), 0);
}

void TcpSocket::SetNoDelay(bool value)
{
	if(m_socketId == kInvalidSocketId)
	{
		return;
	}

	int optval = value ? 1 : 0;
	setsockopt(m_socketId, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&optval), sizeof(optval));
}

void TcpSocket::SetNonBlocking()
{
	if(m_socketId == kInvalidSocketId)
	{
		return;
	}

	u_long mode = 1;
	ioctlsocket(m_socketId, FIONBIO, &mode);
}

void TcpSocket::Initialize()
{
	// Initialize Winsock2
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void TcpSocket::Shutdown()
{
	WSACleanup();
}

#endif	// #if DESIRE_PLATFORM_WINDOWS || DESIRE_PLATFORM_XBOXONE
