#pragma once

#include "messages.h"
#include <mutex>
#include <vector>

#include "Time/time.h"

namespace Krakenplay
{
	/// The Krakenplay input manager singleton class.
	/// Updates its internal state automatically 
	class InputManager
	{
	public:
		static InputManager& Instance();

		/// Updates internal timeouts.
		void Update();
		
		/// Receives an input message. This method is threadsafe.
		/// Krakenplay::NetworkServer uses this method in its receive thread.
		void ReceiveInput(const MessageChunkHeader& header, const void* messageBody, unsigned int messageBodySize, unsigned int clientIndex);

		/// Device info, contained in all concrete device infos.
		struct DeviceInfo
		{
			/// Index of the client computer this device belongs to.
			unsigned int clientIndex;
			
			/// Local index of this device on the client machine.
			uint8_t clientDeviceIndex;

			/// True if there were no disconnect message or the timeout was not exceeded.
			/// Disconnected devices will be replaced if a new device comes in.
			bool connected;

			/// Timestamp of the last received update
			Time lastUpdate;
		};

		struct MouseInfo : public DeviceInfo
		{
			Krakenplay::StateObjects::MouseState state;
		};

	private:
		InputManager() {}
		~InputManager() {}

		/// Retrieves either the mouse info with the given indices or returns a new/reused slot.
		/// For the latter the indices will registered onto the given device slot. Connection state and last update will *not* be changed.
		std::vector<MouseInfo>::iterator GetMouseInfoSlot(unsigned int clientIndex, uint8_t clientDeviceIndex);


		std::vector<MouseInfo> mouseStates;

		std::mutex receiveInputMutex;
	};
}