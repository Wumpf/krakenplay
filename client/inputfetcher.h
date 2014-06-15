#pragma once

#include <vector>
#include "../krakenplay/Time/Stopwatch.h"
#include "../krakenplay/messages.h"

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

		/// Sends update messages of all input devices in a rate between Krakenplay::maxDurationBetweenMessage_seconds and Krakenplay::minDurationBetweenMessage_seconds.
		void Update(NetworkClient& client);

	private:

		/// Checks for free devices and adds them.
		void AddFreeDevices();

		ezStopwatch timeSinceLastUpdatePackage;

		OIS::InputManager* inputManager;
		std::vector<InputFetchDevice*> devices;

		char messageBuffer[g_maxMessageSize];
	};
}
