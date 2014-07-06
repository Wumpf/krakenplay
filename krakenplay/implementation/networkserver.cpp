#include "../networkserver.h"
#include "../inputmanager.h"
#include "../Time/Stopwatch.h"

#include <iostream>
#include <cassert>

namespace Krakenplay
{
	NetworkServer& NetworkServer::Instance()
	{
		static NetworkServer server;
		return server;
	}

	bool NetworkServer::InitServer(uint16_t messagePort, uint16_t identifyPort)
	{	
		DeInitServer();
		this->messagePort = messagePort;
		this->identifyPort = identifyPort;

		// Initialize winsock - multiple initializations are not bad, infact there is reference counting mechanism
		// http://stackoverflow.com/questions/1869689/is-it-possible-to-tell-if-wsastartup-has-been-called-in-a-process
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			std::cerr << "Failed. Error Code: " << WSAGetLastError() << std::endl;
			return false;
		}

		// Create broadcast socket
		if ((broadcastSocket = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create server-broadcast socket: " << WSAGetLastError() << std::endl;
		}
		bool broadcastVal = true;
		if (setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, (char *)&broadcastVal, sizeof(broadcastVal)) == SOCKET_ERROR)
		{
			std::cerr << "Could not set server-broadcast to allow broadcasts (setsockopt). Error code: " << WSAGetLastError() << std::endl;
		}


		// Create server socket.
		if((serverSocket = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create server-receive socket: " << WSAGetLastError() << std::endl;
		}

		// Prepare the sockaddr_in structure.
		sockaddr_in clientAddr;
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_addr.s_addr = INADDR_ANY;
		clientAddr.sin_port = htons(messagePort);

		// Bind
		if(bind(serverSocket, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
		{
			std::cerr << "Bind failed with error code: " << WSAGetLastError() << std::endl;
			return false;
		}


		// Open threads for receiving and autoconnect
		serverRunning = true;
		receiveThread = std::thread(std::bind(&Krakenplay::NetworkServer::Receive, this));
		identifyThread = std::thread(std::bind(&Krakenplay::NetworkServer::Identify, this));

		return true;
	}

	void NetworkServer::SetIdentifyMessageRate(unsigned int identifyMessageRateMS)
	{
		this->identifyMessageRateMS = identifyMessageRateMS;
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
			size_t clientIndex = clientIter - knownClients.begin();
			assert(clientIndex < std::numeric_limits<uint16_t>::max() && "Number of clients higher than storable within an uint16_t!");

			// Resolve endianess.
			ConvertEndiannessNetworkToHost(messageBuffer, recvLen);

			// Preparse message into its chunks.
			int readPos = 0;
			while (readPos < recvLen)
			{
				// Read headers
				MessageChunkHeader* header = reinterpret_cast<MessageChunkHeader*>(&messageBuffer[readPos]);
				readPos += sizeof(MessageChunkHeader);
				unsigned int messageBodySize = GetMessageBodySize(header->messageType);

				// Pass to input manager.
				InputManager::Instance().ReceiveInput(*header, &messageBuffer[readPos], messageBodySize, static_cast<uint16_t>(clientIndex));
				readPos += messageBodySize;
			}
			assert(readPos == recvLen && "Received message was smaller than expected!");
		}
	}

	void NetworkServer::Identify()
	{
		sockaddr_in clientAddr;
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(identifyPort);
		clientAddr.sin_addr.s_addr = INADDR_BROADCAST;

		MessageChunkHeader identifyMessage;
		identifyMessage.messageType = MessageChunkType::SERVER_IDENTIFY;
		identifyMessage.deviceIndex = g_protocolVersion;

		Krakenplay::Stopwatch stopwatch;

		while (serverRunning && identifyMessageRateMS > 0)
		{
			stopwatch.StopAndReset();
			stopwatch.Resume();

			if (sendto(broadcastSocket, reinterpret_cast<char*>(&identifyMessage), sizeof(identifyMessage), 0, reinterpret_cast<sockaddr*>(&clientAddr), sizeof(clientAddr)) == SOCKET_ERROR)
			{
				if (serverRunning && identifyMessageRateMS > 0)
					std::cerr << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
				continue;
			}

			// Sleep until message sending needed again.
			int sleepTimeMS = static_cast<int>(identifyMessageRateMS - stopwatch.GetRunningTotal().GetMilliseconds());
			if (sleepTimeMS > 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMS));
		}
	}

	void NetworkServer::DeInitServer()
	{
		if(serverRunning)
		{
			serverRunning = false; 
			closesocket(serverSocket);
			closesocket(broadcastSocket);
			WSACleanup();
			receiveThread.join();
			identifyThread.join();
		}
	}

	NetworkServer::NetworkServer()
	{
		SetIdentifyMessageRate();
	}

	NetworkServer::~NetworkServer()
	{
		DeInitServer();
	}
}