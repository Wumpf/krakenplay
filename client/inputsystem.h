#pragma once

namespace OIS
{
	class InputManager;
}

namespace Krakenplay
{
	class NetworkClient;

	/// An OIS based input system.
	class InputSystem
	{
	public:
		/// Initializes OIS and logs status into console.
		InputSystem();
		~InputSystem();

		void Update(NetworkClient& client);

	private:
		OIS::InputManager* inputManager;

		static const char* inputDeviceNames[6];
	};

}
