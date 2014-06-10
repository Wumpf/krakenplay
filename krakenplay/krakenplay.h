#pragma once

#include <cstdint>

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
		/// \return true if everything is alright, false otherwise - will write error messages to cerr!
		bool InitServer(uint16_t port = 1244568);

		void DeInitServer();

	private:
		Server() {}
		~Server();

		const size_t maxNumClients;
	};
}