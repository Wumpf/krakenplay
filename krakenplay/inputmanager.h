#pragma once

#include "messages.h"
#include "time/time.h"

#include <mutex>
#include <vector>
#include <cassert>

namespace Krakenplay
{
	/// \brief The Krakenplay input manager singleton class.
	///
	/// Internally all states are hold three times buffered. Once for writing (possibly in parallel) and two times for reading (old and current).
	class InputManager
	{
	public:
		/// Returns singleton instance.
		static InputManager& Instance();

		/// \brief Updates connection timeouts and copies current data from the read state to the write state.
		///
		/// All WasPressed/Released methods rely on this call. This method locks the internal write state that can be accessed in parallel.
		/// You should call this method frequently.
		void Update();

		/// \brief Device info, contained in all concrete device infos.
		template<typename ChildClass, typename ButtonType>
		class DeviceState
		{
		public:
			/// \brief Checks if the given device changed from connected to disconnected in the last update.
			bool WasDisconnected() const;

			/// \brief Checks if the given device disconnected to connected in the last update.
			bool WasConnected() const;

			/// Checks if the given button/key button changed from not-down to down in the last update.
			bool WasButtonPressed(ButtonType button) const;

			/// Checks if the given button/key changed from down to not-down in the last update.
			bool WasButtonReleased(ButtonType button) const;

			/// Timestamp of the last received update.
			Time lastUpdate;

			/// Index of the client computer this device belongs to.
			uint16_t clientID;

			/// Local index of this device on the client machine.
			uint8_t clientDeviceID;

			/// True if there were no disconnect message or the timeout was not exceeded.
			/// Disconnected devices will be replaced if a new device comes in.
			bool connected;

		protected:
			/// Returns the old read state of this object.
			/// \attention Works only if this is the current read state!
			const ChildClass& GetCorrespondingOldState() const;
		};

		/// Device info for mouses.
		class MouseState : public DeviceState<MouseState, MouseButton>
		{
		public:
			/// Checks if the given mouse button is currently down.
			bool IsButtonDown(MouseButton button) const;

			/// Current mouse state data.
			InternalMouseState state;
		};

		/// Device info for keyboards.
		class KeyboardState : public DeviceState<KeyboardState, KeyboardKey>
		{
		public:
			/// Checks if the given keyboard key button is currently down.
			bool IsButtonDown(KeyboardKey key) const;

			/// Current keyboard state data.
			InternalKeyboardState state;
		};

		/// Device info for gamepads.
		class GamepadState : public DeviceState<GamepadState, GamepadButton>
		{
		public:
			/// Checks if the given gamepad button is currently down.
			bool IsButtonDown(GamepadButton button) const;

			/// Current gamepad state data.
			InternalGamepadState state;
		};


		/// \name Template based state access.
		/// State access using child classes of DeviceState as parameter.
		/// \{

		/// \brief Returns the current device state of a given type by providing a global device ID.
		///
		/// If there is no state with the given ID, nullptr will be returned. 
		/// If the queried device is disconnected, its last state will be returned (see DeviceInfo::connected).
		/// \attention Be careful if you store a pointer/references to this state, since after an InputManager::Update call it may be relocated!
		template<typename StateType>
		const StateType* GetState(unsigned int globalDeviceID) const;

		/// \brief Returns the current device state of a given type by providing a client ID and a client device ID.
		///
		/// If there is no state with the given IDs, nullptr will be returned. 
		/// If the queried device is disconnected, its last state will be returned (see DeviceInfo::connected).
		/// \attention Be careful if you store a pointer/references to this state, since after an InputManager::Update call it may be relocated!
		template<typename StateType>
		const StateType* GetState(uint16_t clientID, uint8_t clientDeviceID) const;

		/// \brief Returns the number of known devices of a given state type.
		///
		/// May contain also disconnected devices.
		/// \see GetNumConnectedDevices
		template<typename StateType>
		size_t GetNumDevices() const;

		/// \brief Returns the number of connected devices for a given type.
		///
		/// There may be disconnected devices residing in the list as well.
		/// \see GetNumDevices
		template<typename StateType>
		size_t GetNumConnectedDevices() const;

		/// \}


		/// \name Explicit state access.
		/// Wrapper of the template access functions.
		/// \{

		/// \see GetState
		const MouseState* GetStateMouse(unsigned int globalDeviceID) const;
		/// \see GetState
		const MouseState* GetStateMouse(uint16_t clientID, uint8_t clientDeviceID) const;
		/// \see GetNumDevices
		size_t GetNumMouses() const;
		/// \see GetNumConnectedDevices
		size_t GetNumConnectedMouses() const;

		/// \see GetState
		const KeyboardState* GetStateKeyboard(unsigned int globalDeviceID) const;
		/// \see GetState
		const KeyboardState* GetStateKeyboard(uint16_t clientID, uint8_t clientDeviceID) const;
		/// \see GetNumDevices
		size_t GetNumKeyboards() const;
		/// \see GetNumConnectedDevices
		size_t GetNumConnectedKeyboards() const;

		/// \see GetState
		const GamepadState* GetStateGamepad(unsigned int globalDeviceID) const;
		/// \see GetState
		const GamepadState* GetStateGamepad(uint16_t clientID, uint8_t clientDeviceID) const;
		/// \see GetNumDevices
		size_t GetNumGamepads() const;
		/// \see GetNumConnectedDevices
		size_t GetNumConnectedGamepads() const;

		/// \}


		/// \brief Sets after which time without update a device is considered disconnected.
		///
		/// Note that if a device is orderly shut down, the InputManager/NetworkServer should receive a disconnect message
		/// that will cause the device to be marked disconnected immediately.
		void SetConnectionTimeout(Time deviceConnectionTimeout = Time::Seconds(1.0f));

		/// \brief Receives an input message. This method is thread-safe.
		///
		/// Krakenplay::NetworkServer uses this method in its receive thread.
		void ReceiveInput(const MessageChunkHeader& header, const void* messageBody, unsigned int messageBodySize, uint16_t clientID);

	private:
		InputManager();
		~InputManager();

		friend class MouseState;

		/// Collection of device states of a certain type.
		template<typename StateType>
		struct DeviceStateCollection
		{
			DeviceStateCollection() : numConnected(0) {}

			std::vector<StateType> states;
			size_t numConnected;
		};

		/// Assembly of all input states that need to be double buffered.
		///
		/// Inheriting from DeviceStateCollection collections allows to access states via template parameters.
		/// For a similar example see http://stackoverflow.com/questions/5904680/can-i-gain-access-to-a-component-by-type
		struct InputState : public DeviceStateCollection<MouseState>, DeviceStateCollection<KeyboardState>, DeviceStateCollection<GamepadState>
		{
			/// Retrieves either the mouse info with the given indices or returns a new/reused slot.
			/// For the latter the indices will registered onto the given device slot. Connection state and last update will *not* be changed.
			template<typename StateType>
			StateType& GetInfoSlot(uint16_t clientID, uint8_t clientDeviceIndex);

			/// \brief Returns state collection reference for given state type.
			///
			/// Performs static_cast on this pointer.
			template<typename StateType>
			DeviceStateCollection<StateType>& GetStateCollection();
			/// \brief Returns state collection reference for given state type.
			///
			/// Performs static_cast on this pointer.
			template<typename StateType>
			const DeviceStateCollection<StateType>& GetStateCollection() const;
		};

		InputState readState;		///< State from which is currently read.
		InputState oldReadState;	///< Read state before, used for axis deltas and button events (was pressed/was released)
		InputState writeState;		///< State to which is written in parallel.

		std::mutex inputWriteMutex;

		Time deviceConnectionTimeout;
	};
}

#include "implementation/inputmanager.inl"
