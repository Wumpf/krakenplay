#pragma once

// todo: reduce headers
#include <cstdint>
#include <thread>
#include <winsock2.h>

namespace Krakenplay
{
	/// Krakenplay server singleton class.
	/// The server manages all connections to clients and gathers all incoming inputs.
	class Server
	{
	public:
		static Server& Instance();


		/// Inits server on a given port.
		/// If server was already initialized, existing server will be closed.
		/// Opens a thread that will continuously try to receive message headers. 
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		bool InitServer(uint16_t port = 12445);


		/// 
		void DeInitServer();

	private:
		Server() {}
		~Server();

		void Receive();

		volatile bool serverRunning;

		std::thread receiveThread;
		SOCKET serverSocket;
	};
}