#pragma once

#include <cinttypes>
#include <WinSock2.h>
#include <thread>

namespace Krakenplay
{
	/// Krakenplay client. Transmitts input to a Krakenplay-server via UDP.
	class NetworkClient
	{
	public:
		NetworkClient();
		~NetworkClient();

		/// Inits client on a given port.
		/// If client was already initialized, existing client will be closed.
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		/// \see DeInitClient
		bool InitClient();

		/// Destroys the client.
		/// \see InitClient
		void DeInitClient();

		/// Sets the server address used in Send.
		void SetServerAddress(const char* ip = "127.0.0.1", uint16_t port = 12445);

		/// Sends a message to the currently configured server.
		/// \see SetServerAddress
		void Send(char* data, unsigned int size);

	private:
		SOCKET clientSocket;
		sockaddr_in serverAddr;

		uint16_t port;
		bool initialized;
	};
}