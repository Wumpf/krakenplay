#pragma once

namespace OIS
{
	class InputManager;
}

namespace Krakenplay
{
	class NetworkClient;

	/// An OIS based input system.
	class InputFetcher
	{
	public:
		/// Initializes OIS and logs status into console.
		InputFetcher();
		~InputFetcher();

		void Update(NetworkClient& client);

	private:
		OIS::InputManager* inputManager;

		static const char* inputDeviceNames[6];
	};

}
