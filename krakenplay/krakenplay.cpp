#include "krakenplay.h"

#include <iostream>
#include <winsock2.h>

namespace Krakenplay
{
	Server& Server::Instance()
	{
		static Server server;
		return server;
	}

	bool Server::InitServer(uint16_t port)
	{
		SOCKET s;
		struct sockaddr_in server, si_other;
		int slen, recv_len;
		char buf[512];
		WSADATA wsa;

		slen = sizeof(si_other);

		// Initialize winsock - multiple initializations are not bad, infact there is reference counting mechanism
		// http://stackoverflow.com/questions/1869689/is-it-possible-to-tell-if-wsastartup-has-been-called-in-a-process
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			std::cerr << "Failed. Error Code: " << WSAGetLastError();
			return false;
		}

	/*	// Create server socket
		if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create socket: " << WSAGetLastError();
		}

		// Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(port);

		// Bind
		if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
		{
			std::cerr << "Bind failed with error code: " << WSAGetLastError();
			return false;
		}
		*/
		return true;
	}

	void Server::DeInitServer()
	{
		//closesocket(s);
		WSACleanup();
	}

	Server::~Server()
	{
		DeInitServer();
	}
}