#pragma once

#include <cinttypes>
#include <WinSock2.h>
#include <thread>

namespace Krakenplay
{
	/// Krakenplay client. Transmitts input to a Krakenplay-server via UDP.
	class Client
	{
	public:
		Client() {}
		~Client();

		/// Inits client on a given port.
		/// If client was already initialized, existing client will be closed.
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		bool InitClient(uint16_t port = 12445);

		/// 
		void DeInitClient();

	private:
		void Send();


		std::thread sendThread;
		SOCKET clientSocket;

		uint16_t port;

		volatile bool sendRunning;
	};
}