#include "../networkclient.h"
#include <iostream>

namespace Krakenplay
{
	NetworkClient::NetworkClient() :
		initialized(false),
		messagePort(0)
	{
		// Server address to default
		SetServerAddress();
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

		initialized = true;

		return true;
	}

	void NetworkClient::SetServerAddress(const char* ip, uint16_t messagePort)
	{
		this->messagePort = messagePort;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(messagePort);
		serverAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	}

	bool NetworkClient::WaitForServerIdentifyMessage(uint16_t messagePort, uint16_t identifyPort)
	{
		this->messagePort = messagePort;
		MessageChunkHeader messageBuffer;

		// Create socket for server identify message.
		SOCKET identifySocket;
		if ((identifySocket = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC)) == INVALID_SOCKET)
		{
			std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
			return false;
		}

		sockaddr_in anyAddr;
		anyAddr.sin_family = AF_INET;
		anyAddr.sin_addr.s_addr = INADDR_ANY;
		anyAddr.sin_port = htons(identifyPort);

		if (bind(identifySocket, (struct sockaddr *)&anyAddr, sizeof(anyAddr)) == SOCKET_ERROR)
		{
			std::cerr << "Bind failed with error code: " << WSAGetLastError() << std::endl;
			closesocket(identifySocket);
			return false;
		}

		for (;;)
		{
			// Receive data
			int socketadressHeaderLen = sizeof(serverAddr);
			int recvLen = 0;
			if ((recvLen = recvfrom(identifySocket, reinterpret_cast<char*>(&messageBuffer), sizeof(messageBuffer), 0, reinterpret_cast<sockaddr*>(&serverAddr), &socketadressHeaderLen)) == SOCKET_ERROR)
			{
				int error = WSAGetLastError();

				// Wrong message
				if (error == WSAEMSGSIZE)
					continue;

				std::cerr << "recvfrom() failed with error code: " << error << std::endl;
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

	std::string NetworkClient::GetServerAdress()
	{
		return inet_ntoa(serverAddr.sin_addr);
	}

	NetworkClient::~NetworkClient()
	{
		DeInitClient();
	}
}