#include "client.h"
#include "../krakenplay/messages.h"
#include <iostream>

namespace Krakenplay
{
	bool NetworkClient::InitClient(uint16_t port)
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

		// Open thread for receiving
		sendRunning = true;
		sendThread = std::thread(std::bind(&Krakenplay::NetworkClient::Send, this));

		return true;
	}

	void NetworkClient::Send()
	{
		// Prepare the sockaddr_in structure.
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // TODO

		while(sendRunning)
		{
			// TEST CODE

			// Send message
			MessageHeader messageHeader;
			messageHeader.messageType = 13;

			if(sendto(clientSocket, reinterpret_cast<char*>(&messageHeader), sizeof(messageHeader), 0, 
					 reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
			{
				if(sendRunning)
					std::cerr << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
				continue;
			}

			Sleep(300);
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