#include "krakenplay.h"
#include "messages.h"


#include <iostream>


namespace Krakenplay
{
	NetworkServer& NetworkServer::Instance()
	{
		static NetworkServer server;
		return server;
	}

	bool NetworkServer::InitServer(uint16_t port)
	{	
		DeInitServer();

		// Initialize winsock - multiple initializations are not bad, infact there is reference counting mechanism
		// http://stackoverflow.com/questions/1869689/is-it-possible-to-tell-if-wsastartup-has-been-called-in-a-process
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			std::cerr << "Failed. Error Code: " << WSAGetLastError() << std::endl;
			return false;
		}

		// Create server socket.
		if((serverSocket = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
		}

		// Prepare the sockaddr_in structure.
		sockaddr_in clientAddr;
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_addr.s_addr = INADDR_ANY;
		clientAddr.sin_port = htons(port);

		// Bind
		if(bind(serverSocket, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
		{
			std::cerr << "Bind failed with error code: " << WSAGetLastError() << std::endl;
			return false;
		}

		// Open thread for receiving
		serverRunning = true;
		receiveThread = std::thread(std::bind(&Krakenplay::NetworkServer::Receive, this));
	
		return true;
	}

	void NetworkServer::Receive()
	{
		while(serverRunning)
		{
			// Receive header
			MessageHeader messageHeader;

			sockaddr_in socketadressHeader;
			int socketadressHeaderLen = sizeof(socketadressHeader);
			int recv_len = 0;
			if((recv_len = recvfrom(serverSocket, reinterpret_cast<char*>(&messageHeader), sizeof(MessageHeader), 0,
				reinterpret_cast<sockaddr*>(&socketadressHeader), &socketadressHeaderLen)) == SOCKET_ERROR)
			{
				if(serverRunning)
					std::cerr << "recvfrom() failed with error code: " << WSAGetLastError() << std::endl;
				continue;
			}

			// (DEBUG) print details of the client/peer and the data received
			std::cout << "Received packet from " << inet_ntoa(socketadressHeader.sin_addr) << ":" << ntohs(socketadressHeader.sin_port);
		}
	}

	void NetworkServer::DeInitServer()
	{
		if(serverRunning)
		{
			serverRunning = false; 
			closesocket(serverSocket);
			WSACleanup();
			receiveThread.join();
		}
	}

	NetworkServer::~NetworkServer()
	{
		DeInitServer();
	}
}