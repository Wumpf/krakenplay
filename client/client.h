#pragma once

#include <cinttypes>
#include <WinSock2.h>
#include <thread>

namespace Krakenplay
{
	/// Krakenplay client. Transmits packets to a Krakenplay-server via UDP.
	class NetworkClient
	{
	public:
		NetworkClient();
		~NetworkClient();

		/// Inits client on a given port.
		/// If client was already initialized, existing client will be closed.
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		bool InitClient();

		/// Setups server address which should be used in SendToServer.
		void SetServerAddress(const std::string& ipAdress, uint16_t port = 12445);

		/// Sends the given datablock to the (earlier configurated) server.
		void SendToServer(const void* data, unsigned int dataSize);

		/// Closes all sockets and de-initilizes the client.
		void DeInitClient();

	private:
		void Send();

		SOCKET clientSocket;
		sockaddr_in serverAddr;
		uint16_t port;
	};
}