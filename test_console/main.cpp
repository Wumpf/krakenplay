#include "../krakenplay/networkserver.h"
#include "../krakenplay/inputmanager.h"
#include <iostream>
#include <string>

using namespace Krakenplay;

// Names for all possible mouse buttons.
static const char* mouseButtonNames[static_cast<unsigned>(MouseButton::NUM_BUTTONS)] =
	{ "left", "right", "middle", "button3", "button4", "button5", "button6", "button7" };

// Returns a string for displaying deviceID and clientDeviceID
template<typename DeviceType>
std::string GetDeviceIdentifyString(const DeviceType& deviceInfo)
{
	return "(clientID " + std::to_string(deviceInfo.clientID) + " | devID " + std::to_string(deviceInfo.clientDeviceID) + ") ";
}

int main()
{
	std::cout << "Init server ... ";
	if(NetworkServer::Instance().InitServer())
		std::cout << "done\n";
	else
	{
		std::cout << "failed\n";
		return 1;
	}


	for(;;)
	{
		InputManager::Instance().Update();

		
		// Mouse
		for (unsigned int mouseIdx = 0; mouseIdx < InputManager::Instance().GetNumMouses(); ++mouseIdx)
		{
			auto mouseState = InputManager::Instance().GetState<InputManager::MouseState>(mouseIdx);
			if(mouseState->WasConnected())
				std::cout << GetDeviceIdentifyString(*mouseState) << "Mouse connected!\n";
			if(mouseState->WasDisconnected())
				std::cout << GetDeviceIdentifyString(*mouseState) << "Mouse disconnected!\n";

			// All buttons.
			for (unsigned int mouseButtonIdx = 0; mouseButtonIdx < static_cast<unsigned>(MouseButton::NUM_BUTTONS); ++mouseButtonIdx)
			{
				MouseButton mouseButton = static_cast<MouseButton>(1 << mouseButtonIdx);

				if (mouseState->WasButtonPressed(mouseButton))
					std::cout << GetDeviceIdentifyString(*mouseState) << "Mouse button " << mouseButtonNames[mouseButtonIdx] << " was pressed!\n";
				if (mouseState->WasButtonReleased(mouseButton))
					std::cout << GetDeviceIdentifyString(*mouseState) << "Mouse button " << mouseButtonNames[mouseButtonIdx] << " was released!\n";
			}
		}

		Sleep(10);
	}


	return 0;
}
