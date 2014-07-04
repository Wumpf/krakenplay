#include "../mousefetcher.h"
#include <OISInputManager.h>
#include <cassert>
#include <iostream>

namespace Krakenplay
{
	MouseFetcher::MouseFetcher(OIS::InputManager* inputManager) : inputManager(inputManager)
	{
		inputObject = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
		assert(inputObject && "Input object is nullptr. OIS should report that via an exception.");
		inputObject->setEventCallback(this);

		memset(&currentState, 0, sizeof(currentState));
	}
	
	MouseFetcher::~MouseFetcher()
	{
		inputManager->destroyInputObject(inputObject);
	}

	bool MouseFetcher::FetchState()
	{
		requestImmediateUpdate = false;
		inputObject->capture();

		currentState.buttonState = static_cast<decltype(currentState.buttonState)>(inputObject->getMouseState().buttons);
		// Problem with abs position: It depends on window or a minimum size.
		currentState.positionX += static_cast<decltype(currentState.positionX)>(inputObject->getMouseState().X.rel);
		currentState.positionY += static_cast<decltype(currentState.positionY)>(inputObject->getMouseState().Y.rel);
		currentState.mouseWheel = static_cast<decltype(currentState.mouseWheel)>(inputObject->getMouseState().Z.abs);

		return requestImmediateUpdate;
	}

	MessageChunkHeader MouseFetcher::GetStateMessageHeader() const
	{
		MessageChunkHeader chunkHeader;
		chunkHeader.messageType = MessageChunkType::MOUSE_STATUS;
		chunkHeader.deviceIndex = inputObject->getID();
		return chunkHeader;
	}

	MessageChunkHeader MouseFetcher::GetDisconnectMessageHeader() const
	{
		MessageChunkHeader chunkHeader;
		chunkHeader.messageType = MessageChunkType::MOUSE_DISCONNECT;
		chunkHeader.deviceIndex = inputObject->getID();
		return chunkHeader;
	}

	bool MouseFetcher::mouseMoved(const OIS::MouseEvent &arg)
	{
		requestImmediateUpdate = true;
		return true;
	}

	bool MouseFetcher::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		requestImmediateUpdate = true;
		return true;
	}

	bool MouseFetcher::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		requestImmediateUpdate = true;
		return true;
	}
}