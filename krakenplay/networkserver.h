#pragma once

#include <vector>
#include <cstdint>
#include <thread>
#include <winsock2.h>

#include "messages.h"

namespace Krakenplay
{
	/// \brief Krakenplay server singleton class.
	///
	/// The server manages all connections to clients and gathers all incoming inputs.
	class NetworkServer
	{
	public:
		static NetworkServer& Instance();

		/// \brief Inits server on a given port.
		///
		/// If server was already initialized, existing server will be closed.
		/// Opens a thread that will continuously try to receive message headers and 
		/// another one that sends identify messages in the rate given by SetIdentifyMessageRate.
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		bool InitServer(uint16_t messagePort = g_defaultMessagePort, uint16_t identifyPort = g_defaultIdentifyPort);

		/// \brief Closes the server.
		void DeInitServer();

		/// \brief Sets the rate of identify message broadcasts in milliseconds.
		///
		/// These packages are used to enable automatic client connection.
		/// \param identifyMessageRateMS Rate in milliseconds. Zero will result in NO packages at all.
		void SetIdentifyMessageRate(unsigned int identifyMessageRateMS = 400);

	private:
		NetworkServer();
		~NetworkServer();

		void Receive();
		void Identify();
	
		volatile unsigned int identifyMessageRateMS;
		volatile bool serverRunning;

		std::thread receiveThread;
		SOCKET serverSocket;
		uint16_t messagePort;

		std::thread identifyThread;
		SOCKET broadcastSocket;
		uint16_t identifyPort;

		

		/// List of all known clients. Position within list gives each an unique id.
		/// \attention Used by the receive thread.
		std::vector<std::string> knownClients;
	};
}