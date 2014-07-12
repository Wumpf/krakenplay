#pragma once

#include <cinttypes>
#include <thread>

#include "../krakenplay/socketabstraction.h"
#include "../krakenplay/messages.h"

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

		/// \brief Waits for an identify message from a krakenplay server.
		///
		/// \attention This method blocks until a server is found!
		/// Calls SetServerAddress when a server is found.
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		bool WaitForServerIdentifyMessage(uint16_t messagePort = g_defaultMessagePort, uint16_t identifyPort = g_defaultIdentifyPort);

		/// Sets the server address used in Send.
		void SetServerAddress(const char* ip = "127.0.0.1", uint16_t messagePort = g_defaultMessagePort);

		/// Sends a message to the currently configured server.
		/// \attention Will convert the incoming data to network endianess.
		/// \see SetServerAddress
		void Send(char* data, unsigned int size);


		/// Returns used server ip adress.
		std::string GetServerAdress();

	private:
		SOCKET clientSocket;
		sockaddr_in serverAddr;

		uint16_t messagePort;
		bool initialized;
	};
}
