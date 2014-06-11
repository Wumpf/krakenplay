#include "client.h"
#include "../krakenplay/messages.h"
#include <iostream>

namespace Krakenplay
{
	NetworkClient::NetworkClient()
	{
		memset(&serverAddr, 0, sizeof(serverAddr));
	}

	bool NetworkClient::InitClient()
	{
		DeInitClient();

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

		return true;
	}

	void NetworkClient::SetServerAddress(const std::string& ipAdress, uint16_t port = 12445)
	{
		// Prepare the sockaddr_in structure.
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.S_un.S_addr = inet_addr(ipAdress.c_str());
	}

	void NetworkClient::SendToServer(const void* data, unsigned int dataSize)
	{
		// Send message
		if(sendto(clientSocket, reinterpret_cast<const char*>(data), dataSize, 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
		{
			std::cerr << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
		}
	}

	void NetworkClient::DeInitClient()
	{
		if(sendRunning)
		{
			sendRunning = false;
			closesocket(clientSocket);
			WSACleanup();
			sendThread.join();
		}
	}

	NetworkClient::~NetworkClient()
	{
		DeInitClient();
	}
}