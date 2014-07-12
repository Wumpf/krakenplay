#include "../networkclient.h"
#include <iostream>

namespace Krakenplay
{
	NetworkClient::NetworkClient() :
        messagePort(0),
        initialized(false)
	{
		// Server address to default
		SetServerAddress();
	}

	bool NetworkClient::InitClient()
	{
		DeInitClient();

#ifdef _WIN32
		// Initialize winsock - multiple initializations are not bad, infact there is reference counting mechanism
		// http://stackoverflow.com/questions/1869689/is-it-possible-to-tell-if-wsastartup-has-been-called-in-a-process
		WSADATA wsa;
		if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			std::cerr << "Failed to start winsock: " << GetLastSocketErrorDescription() << std::endl;
			return false;
		}
#endif

		// Create server socket.
		if((clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create socket: " << GetLastSocketErrorDescription() << std::endl;
		}

		initialized = true;

		return true;
	}

	void NetworkClient::SetServerAddress(const char* ip, uint16_t messagePort)
	{
		this->messagePort = messagePort;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(messagePort);
		serverAddr.sin_addr.s_addr = htonl(inet_addr(ip));
	}

	bool NetworkClient::WaitForServerIdentifyMessage(uint16_t messagePort, uint16_t identifyPort)
	{
		this->messagePort = messagePort;
		MessageChunkHeader messageBuffer;

		// Create socket for server identify message.
		SOCKET identifySocket;
		if ((identifySocket = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create socket: " << GetLastSocketErrorDescription() << std::endl;
			return false;
		}

		sockaddr_in anyAddr;
		anyAddr.sin_family = AF_INET;
		anyAddr.sin_port = htons(identifyPort);
		anyAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(identifySocket, (struct sockaddr *)&anyAddr, sizeof(anyAddr)) == SOCKET_ERROR)
		{
			std::cerr << "Bind failed: " << GetLastSocketErrorDescription() << std::endl;
			closesocket(identifySocket);
			return false;
		}

		for (;;)
		{
			// Receive data
			socklen_t socketadressHeaderLen = sizeof(serverAddr);
			ssize_t recvLen = 0;
			if ((recvLen = recvfrom(identifySocket, reinterpret_cast<char*>(&messageBuffer), sizeof(messageBuffer), 0, reinterpret_cast<sockaddr*>(&serverAddr), &socketadressHeaderLen)) == SOCKET_ERROR)
			{
				// Wrong message
#ifdef _WIN32
				int error = WSAGetLastError();
				if (error == WSAEMSGSIZE)
					continue;
#else
				if (errno == EMSGSIZE)
					continue;
#endif
				std::cerr << "recvfrom() failed: " << GetLastSocketErrorDescription() << std::endl;
				continue;
			}

			if (recvLen == sizeof(messageBuffer) && messageBuffer.messageType == MessageChunkType::SERVER_IDENTIFY)
			{
				if (messageBuffer.deviceIndex != g_protocolVersion)
				{
					std::cerr << "Found server on adress \"" << inet_ntoa(serverAddr.sin_addr) << "\" with protocol version \"" << 
									g_protocolVersion << "\". Supported version: " << g_protocolVersion << std::endl;
				}
				else
				{
					serverAddr.sin_port = htons(messagePort);
					break;
				}
			}
		}

		closesocket(identifySocket);
		return true;
	}

	void NetworkClient::Send(char* data, unsigned int size)
	{
		ConvertEndiannessHostToNetwork(data, size);
		if (sendto(clientSocket, data, size, 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
		{
			std::cerr << "sendto() failed with error code: " << GetLastSocketErrorDescription() << std::endl;
		}
	}

	void NetworkClient::DeInitClient()
	{
		if (initialized)
		{
			closesocket(clientSocket);
#ifdef _WIN32
			WSACleanup();
#endif
			initialized = false;
		}
	}

	std::string NetworkClient::GetServerAdress()
	{
		return inet_ntoa(serverAddr.sin_addr);
	}

	NetworkClient::~NetworkClient()
	{
		DeInitClient();
	}
}
