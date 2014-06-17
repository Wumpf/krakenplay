#pragma once

#include "messages.h"
#include "Time/time.h"

#include <mutex>
#include <vector>
#include <cassert>

namespace Krakenplay
{
	/// The Krakenplay input manager singleton class.
	/// All states are double buffered, so that the Krakenplay::NetworkServer can update the data in parallel.
	class InputManager
	{
	public:
		static InputManager& Instance();

		/// Updates connection timeouts and copies current data from the read state to the write state.
		void Update();
		
		/// Device info, contained in all concrete device infos.
		template<typename ChildClass>
		class DeviceState
		{
		public:
			/// Checks if the given device changed from connected to disconnected in the last update.
			bool WasDisconnected() const;

			/// Checks if the given device disconnected to connected in the last update.
			bool WasConnected() const;
		
			/// Index of the client computer this device belongs to.
			unsigned int clientID;
			
			/// Local index of this device on the client machine.
			uint8_t clientDeviceID;

			/// True if there were no disconnect message or the timeout was not exceeded.
			/// Disconnected devices will be replaced if a new device comes in.
			bool connected;

			/// Timestamp of the last received update
			Time lastUpdate;

		protected:
			/// Returns the old read state of this object.
			/// \attention Works only if this is the current read state!
			const ChildClass& GetOldState() const;
		};

		/// Device info for mouses.
		class MouseState : public DeviceState<MouseState>
		{
		public:
			/// Checks if the given mouse button is currently down.
			bool IsButtonDown(MouseButton button) const;

			/// Checks if the given mouse button changed from not-down to down in the last update.
			bool WasButtonPressed(MouseButton button) const;

			/// Checks if the given mouse button changed from down to not-down in the last update.
			bool WasButtonReleased(MouseButton button) const;


			/// Current mouse state data.
			InternalMouseState state;
		};


		/// Returns the current device state of a given type by providing a global device ID.
		///
		/// If there is no mouse state with the given ID, nullptr will be returned. 
		/// If the queried device is disconnected, its last state will be returned (see DeviceInfo::connected).
		/// \attention Be careful if you store a pointer/references to this state, since after an InputManager::Update call it may be relocated!
		template<typename StateType>
		const StateType* GetState(unsigned int globalDeviceID) const;

		/// Returns the current device state of a given type by providing a client ID and a client device ID.
		///
		/// If there is no mouse state with the given IDs, nullptr will be returned. 
		/// If the queried device is disconnected, its last state will be returned (see DeviceInfo::connected).
		/// \attention Be careful if you store a pointer/references to this state, since after an InputManager::Update call it may be relocated!
		template<typename StateType>
		const StateType* GetState(unsigned int clientID, uint8_t clientDeviceID) const;

		/// Returns the number of known mouse devices.
		/// May contain also disconnected devices.
		unsigned int GetNumMouses() const;

		/// Returns the number of connected mouse devices.
		/// There may be disconnected devices residing in the list as well.
		unsigned int GetNumConnectedMouses() const;



		/// Sets after which time without update a device is considered disconnected.
		/// Note that if a device is orderly shut down, the InputManager/NetworkServer should receive a disconnect message
		/// that will cause the device to be marked disconnected immediately.
		void SetConnectionTimeout(Time deviceConnectionTimeout = Time::Seconds(1.0f));

		/// Receives an input message. This method is threadsafe.
		/// Krakenplay::NetworkServer uses this method in its receive thread.
		void ReceiveInput(const MessageChunkHeader& header, const void* messageBody, unsigned int messageBodySize, unsigned int clientIndex);

	private:
		InputManager();
		~InputManager();

		friend class MouseState;

		/// assembly of all input states that need to be double buffered.
		struct InputState
		{
			InputState() : numConnectedMouses(0) {}

			std::vector<MouseState> mouseStates;
			unsigned int numConnectedMouses;

			/// Retrieves either the mouse info with the given indices or returns a new/reused slot.
			/// For the latter the indices will registered onto the given device slot. Connection state and last update will *not* be changed.
			std::vector<MouseState>::iterator GetMouseInfoSlot(unsigned int clientIndex, uint8_t clientDeviceIndex);


			/// Gets device state list of a given type.
			template<typename DeviceType>
			const std::vector<DeviceType>& GetDeviceStates() const;

			/// Gets mouse device states.
			template<> const std::vector<MouseState>& GetDeviceStates<MouseState>() const;
		};

		InputState readState;		///< State from which is currently read.
		InputState oldReadState;	///< Read state before, used for axis deltas and button events (was pressed/was released)
		InputState writeState;		///< State to which is written in parallel.

		std::mutex inputWriteMutex;

		Time deviceConnectionTimeout;
	};
}

#include "implementation/inputmanager.inl"