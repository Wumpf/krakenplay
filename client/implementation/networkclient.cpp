#include "../networkclient.h"
#include "../../krakenplay/messages.h"
#include <iostream>

namespace Krakenplay
{
	NetworkClient::NetworkClient() :
		initialized(false)
	{
		// Server address to default
		SetServerAddress();
	}

	bool NetworkClient::InitClient()
	{
		DeInitClient();
		this->port = port;

		// Initialize winsock - multiple initializations are not bad, infact there is reference counting mechanism
		// http://stackoverflow.com/questions/1869689/is-it-possible-to-tell-if-wsastartup-has-been-called-in-a-process
		WSADATA wsa;
		if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			std::cerr << "Failed. Error Code: " << WSAGetLastError() << std::endl;
			return false;
		}

		// Create server socket.
		if((clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
		}

		initialized = true;

		return true;
	}

	void NetworkClient::SetServerAddress(const char* ip, uint16_t port)
	{
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	}

	void NetworkClient::Send(char* data, unsigned int size)
	{
		if (sendto(clientSocket, data, size, 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
		{
			std::cerr << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
		}
	}


	void NetworkClient::DeInitClient()
	{
		if (initialized)
		{
			closesocket(clientSocket);
			WSACleanup();
			initialized = false;
		}
	}

	NetworkClient::~NetworkClient()
	{
		DeInitClient();
	}
}