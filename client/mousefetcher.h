#pragma once

#include <OISMouse.h>
#include "inputfetchdevice.h"
#include "../krakenplay/messages.h"

namespace Krakenplay
{
	/// Mouse input fetcher for grabbing input data from a mouse.
	class MouseFetcher : public OIS::MouseListener, public InputFetchDevice
	{
	public:
		MouseFetcher(OIS::InputManager* inputManager);
		~MouseFetcher();

		/// \copydoc InputFetchDevice::FetchState
		virtual bool FetchState() override;

		/// \copydoc InputFetchDevice::GetState
		virtual const void* GetState(unsigned int& size) const override
		{ size = sizeof(currentState); return &currentState; }

		/// \copydoc InputFetchDevice::GetStateMessageHeader
		virtual const MessageChunkHeader* GetStateMessageHeader() override
		{ return &stateMessageHeader; }

		// OIS Callback functions:
		bool mouseMoved(const OIS::MouseEvent &arg) override;
		bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) override;
		bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) override;

	private:
		bool requestImmediateUpdate;

		OIS::InputManager* inputManager;
		OIS::Mouse* inputObject;

		MessageChunkHeader stateMessageHeader;
		StateObjects::MouseState currentState;
	};
}