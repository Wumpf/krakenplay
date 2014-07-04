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
		/// Creates and initializes the mouse fetcher.
		/// \param inputManager		Used ois input manager.
		MouseFetcher(OIS::InputManager* inputManager);
		~MouseFetcher();

		/// \copydoc InputFetchDevice::FetchState
		virtual bool FetchState() override;

		/// \copydoc InputFetchDevice::GetState
		virtual const void* GetState(unsigned int& size) const override
		{ size = sizeof(currentState); return &currentState; }

		/// \copydoc InputFetchDevice::GetStateMessageHeader
		virtual MessageChunkHeader GetStateMessageHeader() const override;

		/// \copydoc InputFetchDevice::GetDisconnectMessageHeader
		virtual MessageChunkHeader GetDisconnectMessageHeader() const override;

		// OIS Callback functions:

		/// OIS callback for mouse movement.
		bool mouseMoved(const OIS::MouseEvent &arg) override;
		/// OIS callback for mouse press events.
		bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) override;
		/// OIS callback for mouse released events.
		bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) override;

	private:
		bool requestImmediateUpdate;

		OIS::InputManager* inputManager;
		OIS::Mouse* inputObject;

		InternalMouseState currentState;
	};
}