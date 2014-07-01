#pragma once

#include <OISKeyboard.h>
#include "inputfetchdevice.h"
#include "../krakenplay/messages.h"

namespace Krakenplay
{
	/// Keyboard input fetcher for grabbing input data from a Keyboard.
	class KeyboardFetcher : public OIS::KeyListener, public InputFetchDevice
	{
	public:
		/// Creates and initializes the Keyboard fetcher.
		/// \param inputManager		Used ois input manager.
		KeyboardFetcher(OIS::InputManager* inputManager);
		~KeyboardFetcher();

		/// \copydoc InputFetchDevice::FetchState
		virtual bool FetchState() override;

		/// \copydoc InputFetchDevice::GetState
		virtual const void* GetState(unsigned int& size) const override
		{ size = sizeof(currentState); return &currentState; }

		/// \copydoc InputFetchDevice::GetStateMessageHeader
		virtual const MessageChunkHeader* GetStateMessageHeader() override
		{ return &stateMessageHeader; }

		// OIS Callback functions:

		/// OIS callback for Keyboard press events.
		bool keyPressed(const OIS::KeyEvent& arg) override;
		/// OIS callback for Keyboard released events.
		bool keyReleased(const OIS::KeyEvent& arg) override;

	private:
		bool requestImmediateUpdate;

		OIS::InputManager* inputManager;
		OIS::Keyboard* inputObject;

		MessageChunkHeader stateMessageHeader;
		InternalKeyboardState currentState;
	};
}