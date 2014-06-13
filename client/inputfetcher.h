#pragma once

#include <vector>

namespace OIS
{
	class InputManager;
}

namespace Krakenplay
{
	class NetworkClient;
	class InputFetchDevice;

	/// An OIS based input system.
	class InputFetcher
	{
	public:
		/// Initializes OIS and logs status into console.
		InputFetcher();
		~InputFetcher();

		void Update(NetworkClient& client);

	private:

		/// Checks for free devices and adds them.
		void AddFreeDevices();

		OIS::InputManager* inputManager;
		std::vector<InputFetchDevice*> devices;
	};
}
