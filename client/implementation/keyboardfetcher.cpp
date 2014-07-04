#include "../Keyboardfetcher.h"
#include <OISInputManager.h>
#include <cassert>
#include <iostream>

namespace Krakenplay
{
	KeyboardFetcher::KeyboardFetcher(OIS::InputManager* inputManager) : inputManager(inputManager)
	{
		inputObject = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
		assert(inputObject && "Input object is nullptr. OIS should report that via an exception.");
		inputObject->setEventCallback(this);

		memset(&currentState, 0, sizeof(currentState));
	}

	KeyboardFetcher::~KeyboardFetcher()
	{
		inputManager->destroyInputObject(inputObject);
	}

	bool KeyboardFetcher::FetchState()
	{
		requestImmediateUpdate = false;
		inputObject->capture();

		unsigned int numPressedKeys = 0;
		for(unsigned int i = 0; i < static_cast<unsigned int>(KeyboardKey::NUM_KEYS) && numPressedKeys < sizeof(currentState.pressedKeys); ++i)
		{
			if(inputObject->isKeyDown(static_cast<OIS::KeyCode>(i)))
			{
				currentState.pressedKeys[numPressedKeys] = static_cast<KeyboardKey>(i);
				++numPressedKeys;
			}
		}
		for(unsigned int i = numPressedKeys; i < sizeof(currentState.pressedKeys); ++i)
		{
			currentState.pressedKeys[i] = KeyboardKey::NONE;
		}

		return requestImmediateUpdate;
	}

	MessageChunkHeader KeyboardFetcher::GetStateMessageHeader() const
	{
		MessageChunkHeader chunkHeader;
		chunkHeader.messageType = MessageChunkType::KEYBOARD_STATUS;
		chunkHeader.deviceIndex = inputObject->getID();
		return chunkHeader;
	}

	MessageChunkHeader KeyboardFetcher::GetDisconnectMessageHeader() const
	{
		MessageChunkHeader chunkHeader;
		chunkHeader.messageType = MessageChunkType::KEYBOARD_DISCONNECT;
		chunkHeader.deviceIndex = inputObject->getID();
		return chunkHeader;
	}

	bool KeyboardFetcher::keyPressed(const OIS::KeyEvent& arg)
	{
		requestImmediateUpdate = true;
		return true;
	}

	bool KeyboardFetcher::keyReleased(const OIS::KeyEvent& arg)
	{
		requestImmediateUpdate = true;
		return true;
	}
}