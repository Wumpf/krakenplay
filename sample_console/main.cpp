#include "../krakenplay/networkserver.h"
#include "../krakenplay/inputmanager.h"
#include <iostream>
#include <string>

using namespace Krakenplay;

// Names for all possible mouse buttons.
static const char* mouseButtonNames[static_cast<unsigned>(MouseButton::NUM_BUTTONS)] =
	{ "left", "right", "middle", "button3", "button4", "button5", "button6", "button7" };

// Names for all possible gamepad buttons.
static const char* gamepadButtonNames[static_cast<unsigned>(GamepadButton::NUM_BUTTONS)] =
	{ "dpad_up", "dpad_down", "dpad_left", "dpad_right", "start", "back", "left_thumb", "right_thumb", "left_shoulder", "right_shoulder", "a", "b", "x", "y" };

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

		// Mouse.
		for (unsigned int mouseIdx = 0; mouseIdx < InputManager::Instance().GetNumMouses(); ++mouseIdx)
		{
			auto mouseState = InputManager::Instance().GetStateMouse(mouseIdx);
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

		// Keyboard
		for(unsigned int keyboardIdx = 0; keyboardIdx < InputManager::Instance().GetNumKeyboards(); ++keyboardIdx)
		{
			auto keyboardState = InputManager::Instance().GetStateKeyboard(keyboardIdx);
			if(keyboardState->WasConnected())
				std::cout << GetDeviceIdentifyString(*keyboardState) << "Keyboard connected!\n";
			if(keyboardState->WasDisconnected())
				std::cout << GetDeviceIdentifyString(*keyboardState) << "Keyboard disconnected!\n";

			// All buttons.
			for(unsigned int keyboardKeyIdx = 0; keyboardKeyIdx < static_cast<unsigned>(KeyboardKey::NUM_KEYS); ++keyboardKeyIdx)
			{
				KeyboardKey key = static_cast<KeyboardKey>(keyboardKeyIdx);
				if(keyboardState->WasButtonPressed(key))
					std::cout << GetDeviceIdentifyString(*keyboardState) << "Keyboard key " << keyboardKeyToStringMap[keyboardKeyIdx] << " was pressed!\n";
				if(keyboardState->WasButtonReleased(key))
					std::cout << GetDeviceIdentifyString(*keyboardState) << "Keyboard key " << keyboardKeyToStringMap[keyboardKeyIdx] << " was released!\n";
			}
		}

		// Gamepad
		for (unsigned int gamepadIdx = 0; gamepadIdx < InputManager::Instance().GetNumGamepads(); ++gamepadIdx)
		{
			auto gamepadState = InputManager::Instance().GetStateGamepad(gamepadIdx);
			if (gamepadState->WasConnected())
				std::cout << GetDeviceIdentifyString(*gamepadState) << "Gamepad connected!\n";
			if (gamepadState->WasDisconnected())
				std::cout << GetDeviceIdentifyString(*gamepadState) << "Gamepad disconnected!\n";

			// All buttons.
			for (unsigned int gamepadButtonIdx = 0; gamepadButtonIdx < static_cast<unsigned>(GamepadButton::NUM_BUTTONS); ++gamepadButtonIdx)
			{
				GamepadButton gamepadButton = static_cast<GamepadButton>(1 << gamepadButtonIdx);

				if (gamepadState->WasButtonPressed(gamepadButton))
					std::cout << GetDeviceIdentifyString(*gamepadState) << "Gamepad button " << gamepadButtonNames[gamepadButtonIdx] << " was pressed!\n";
				if (gamepadState->WasButtonReleased(gamepadButton))
					std::cout << GetDeviceIdentifyString(*gamepadState) << "Gamepad button " << gamepadButtonNames[gamepadButtonIdx] << " was released!\n";
			}
		}
	}


	return 0;
}
