#include "../gamepadfetcher.h"
#include <OISInputManager.h>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace Krakenplay
{
	GamepadFetcher::GamepadFetcher(OIS::InputManager* inputManager) : inputManager(inputManager)
	{
		inputObject = static_cast<OIS::JoyStick*>(inputManager->createInputObject(OIS::OISJoyStick, true));
		assert(inputObject && "Input object is nullptr. OIS should report that via an exception.");
		inputObject->setEventCallback(this);

		memset(&currentState, 0, sizeof(currentState));
		stateMessageHeader.messageType = MessageChunkType::GAMEPAD_STATUS;
		stateMessageHeader.deviceIndex = inputObject->getID();

		std::string vendor = inputObject->vendor();
		std::transform(vendor.begin(), vendor.end(), vendor.begin(), ::tolower);
		xboxController = vendor.find("xbox") != std::string::npos;
	}
	
	GamepadFetcher::~GamepadFetcher()
	{
		inputManager->destroyInputObject(inputObject);
	}

	bool GamepadFetcher::FetchState()
	{
		requestImmediateUpdate = false;
		inputObject->capture();


		// When it comes to actual XBox360 controllers, we are facing a horrible input mapping mess here.
		// http://wiki.unity3d.com/index.php?title=Xbox360Controller

		// This current version works only correctly on windows for OIS with OIS_WIN32_XINPUT_SUPPORT

		// Try to map to XBox-Gamepad buttons. At least on windows buttons are mapped the following way.
		if (xboxController && inputObject->getNumberOfComponents(OIS::OIS_Button) == 12)
		{
			currentState.buttons = static_cast<GamepadButton>(
				(inputObject->getJoyStickState().mButtons[0] ? static_cast<uint16_t>(GamepadButton::START) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[1] ? static_cast<uint16_t>(GamepadButton::BACK) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[2] ? static_cast<uint16_t>(GamepadButton::LEFT_THUMB) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[3] ? static_cast<uint16_t>(GamepadButton::RIGHT_THUMB) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[4] ? static_cast<uint16_t>(GamepadButton::LEFT_SHOULDER) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[5] ? static_cast<uint16_t>(GamepadButton::RIGHT_SHOULDER) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[8] ? static_cast<uint16_t>(GamepadButton::A) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[9] ? static_cast<uint16_t>(GamepadButton::B) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[10] ? static_cast<uint16_t>(GamepadButton::X) : static_cast<uint16_t>(0)) |
				(inputObject->getJoyStickState().mButtons[11] ? static_cast<uint16_t>(GamepadButton::Y) : static_cast<uint16_t>(0)));
		}
		else
		{
			uint16_t buttonCode = 0;
			
			unsigned int buttonNum = std::min<unsigned int>(inputObject->getNumberOfComponents(OIS::OIS_Button), sizeof(currentState.buttons) * 8 - 4);
			for (unsigned int i = 0; i < buttonNum; ++i)
				buttonCode &= inputObject->getJoyStickState().mButtons[i] ? (1 << i) : 0;
			currentState.buttons = static_cast<GamepadButton>(buttonCode);
		}

		int numAxis = inputObject->getNumberOfComponents(OIS::OIS_Axis);

		// Left analog.
		if (numAxis > 0)
			currentState.thumbLX = inputObject->getJoyStickState().mAxes[0].abs;
		else
			currentState.thumbLX = 0;
		if (numAxis > 1)
			currentState.thumbLY = inputObject->getJoyStickState().mAxes[1].abs;
		else
			currentState.thumbLY = 0;

		// Right analog.
		if (numAxis > 2)
			currentState.thumbRX = inputObject->getJoyStickState().mAxes[2].abs;
		else
			currentState.thumbRX = 0;
		if (numAxis > 3)
			currentState.thumbRY = inputObject->getJoyStickState().mAxes[3].abs;
		else
			currentState.thumbRY = 0;

		// Trigger
		if (numAxis > 4)
			currentState.leftTrigger = inputObject->getJoyStickState().mAxes[4].abs / 129;
		else
			currentState.leftTrigger = 0;
		if (numAxis > 4)
			currentState.rightTrigger = inputObject->getJoyStickState().mAxes[5].abs / 129;
		else
			currentState.rightTrigger = 0;

		// POV
		if (inputObject->getNumberOfComponents(OIS::OIS_POV) > 0)
		{
			currentState.buttons = static_cast<GamepadButton>(static_cast<uint16_t>(currentState.buttons) | 
				((inputObject->getJoyStickState().mPOV[0].direction & OIS::Pov::North) > 0 ? static_cast<uint16_t>(GamepadButton::DPAD_UP) : 0) |
				((inputObject->getJoyStickState().mPOV[0].direction & OIS::Pov::South) > 0 ? static_cast<uint16_t>(GamepadButton::DPAD_DOWN) : 0) |
				((inputObject->getJoyStickState().mPOV[0].direction & OIS::Pov::West) > 0 ? static_cast<uint16_t>(GamepadButton::DPAD_LEFT) : 0) |
				((inputObject->getJoyStickState().mPOV[0].direction & OIS::Pov::East) > 0 ? static_cast<uint16_t>(GamepadButton::DPAD_RIGHT) : 0));
		}
		
		return requestImmediateUpdate;
	}

	bool GamepadFetcher::buttonPressed(const OIS::JoyStickEvent &arg, int button)
	{
		requestImmediateUpdate = true;
		return true;
	}

	bool GamepadFetcher::buttonReleased(const OIS::JoyStickEvent &arg, int button)
	{
		requestImmediateUpdate = true;
		return true;
	}

	bool GamepadFetcher::axisMoved(const OIS::JoyStickEvent &arg, int axis)
	{
		requestImmediateUpdate = true;
		return true;
	}
}