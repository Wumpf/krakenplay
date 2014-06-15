#pragma once

#include <vector>
#include <cstdint>
#include <thread>
#include <winsock2.h>

namespace Krakenplay
{
	/// Krakenplay server singleton class.
	/// The server manages all connections to clients and gathers all incoming inputs.
	class NetworkServer
	{
	public:
		static NetworkServer& Instance();

		/// Inits server on a given port.
		/// If server was already initialized, existing server will be closed.
		/// Opens a thread that will continuously try to receive message headers. 
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		bool InitServer(uint16_t port = 12445);

		/// Closes the server.
		void DeInitServer();

	private:
		NetworkServer() {}
		~NetworkServer();

		void Receive();
	
		volatile bool serverRunning;

		std::thread receiveThread;
		SOCKET serverSocket;

		/// List of all known clients. Position within list gives each an unique id.
		/// \attention Used by the receive thread.
		std::vector<std::string> knownClients;
	};
}