#include "../networkserver.h"
#include "../messages.h"
#include "../inputmanager.h"

#include <iostream>
#include <cassert>

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
		char messageBuffer[g_maxMessageSize];

		sockaddr_in socketadressHeader;

		while(serverRunning)
		{
			// Receive data
			int socketadressHeaderLen = sizeof(sockaddr_in);
			int recvLen = 0;
			if ((recvLen = recvfrom(serverSocket, messageBuffer, g_maxMessageSize, 0, reinterpret_cast<sockaddr*>(&socketadressHeader), &socketadressHeaderLen)) == SOCKET_ERROR)
			{
				if (serverRunning)
					std::cerr << "recvfrom() failed with error code: " << WSAGetLastError() << std::endl;
				continue;
			}

			// Check if client is already known and add to list if not.
			std::string ipString = inet_ntoa(socketadressHeader.sin_addr);
			auto clientIter = std::find(knownClients.begin(), knownClients.end(), ipString);
			if (clientIter == knownClients.end())
			{
				knownClients.push_back(ipString);
				clientIter = knownClients.end() - 1;
			}
			unsigned int clientIndex = clientIter - knownClients.begin();

			// Preparse message into its chunks.
			int readPos = 0;
			while (readPos < recvLen)
			{
				// Read headers
				const MessageChunkHeader* header = reinterpret_cast<MessageChunkHeader*>(&messageBuffer[readPos]);
				readPos += sizeof(MessageChunkHeader);
				unsigned int messageBodySize = GetMessageBodySize(header->messageType);

				// Pass to input manager.
				InputManager::Instance().ReceiveInput(*header, &messageBuffer[readPos], messageBodySize, clientIndex);
				readPos += messageBodySize;
			}
			assert(readPos == recvLen && "Received message was smaller than expected!");
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