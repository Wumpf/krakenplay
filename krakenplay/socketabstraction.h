#pragma once

#ifdef _WIN32
	#include <WinSock2.h>

	typedef int ssize_t;
	typedef int socklen_t;
#else
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	typedef int SOCKET;
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
#endif

#include <string>

namespace Krakenplay
{
	const std::string GetLastSocketErrorDescription();
#ifndef _WIN32
	inline int closesocket(SOCKET s) { return close(s); }
#endif
}
