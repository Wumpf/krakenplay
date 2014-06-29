#pragma once

#include <OISJoyStick.h>
#include "inputfetchdevice.h"
#include "../krakenplay/messages.h"

namespace Krakenplay
{
	/// \brief Gamepad input fetcher for grabbing input data from a gamepad.
	///
	/// On windows OIS should be compiled with OIS_WIN32_XINPUT_SUPPORT to handle xbox controller properly!
	class GamepadFetcher : public OIS::JoyStickListener, public InputFetchDevice
	{
	public:
		/// Creates and initializes the mouse fetcher.
		/// \param inputManager		Used ois input manager.
		GamepadFetcher(OIS::InputManager* inputManager);
		~GamepadFetcher();

		/// \copydoc InputFetchDevice::FetchState
		virtual bool FetchState() override;

		/// \copydoc InputFetchDevice::GetState
		virtual const void* GetState(unsigned int& size) const override
		{ size = sizeof(currentState); return &currentState; }

		/// \copydoc InputFetchDevice::GetStateMessageHeader
		virtual const MessageChunkHeader* GetStateMessageHeader() override
		{ return &stateMessageHeader; }

		// OIS Callback functions:

		/// OIS callback for mouse movement.
		bool buttonPressed(const OIS::JoyStickEvent &arg, int button) override;
		/// OIS callback for mouse press events.
		bool buttonReleased(const OIS::JoyStickEvent &arg, int button) override;
		/// OIS callback for mouse released events.
		bool axisMoved(const OIS::JoyStickEvent &arg, int axis) override;

	private:
		bool requestImmediateUpdate;
		bool xboxController;

		OIS::InputManager* inputManager;
		OIS::JoyStick* inputObject;

		MessageChunkHeader stateMessageHeader;
		InternalGamepadState currentState;
	};
}