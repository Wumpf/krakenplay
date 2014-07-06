namespace Krakenplay
{
	template<typename ChildClass, typename ButtonType>
	inline bool InputManager::DeviceState<ChildClass, ButtonType>::WasDisconnected() const
	{
		return !connected && GetCorrespondingOldState().connected;
	}

	template<typename ChildClass, typename ButtonType>
	inline bool InputManager::DeviceState<ChildClass, ButtonType>::WasConnected() const
	{
		return connected && !GetCorrespondingOldState().connected;
	}

	template<typename ChildClass, typename ButtonType>
	inline bool InputManager::DeviceState<ChildClass, ButtonType>::WasButtonPressed(ButtonType button) const
	{
		return static_cast<const ChildClass*>(this)->IsButtonDown(button) && !GetCorrespondingOldState().IsButtonDown(button);
	}
	template<typename ChildClass, typename ButtonType>
	inline bool InputManager::DeviceState<ChildClass, ButtonType>::WasButtonReleased(ButtonType button) const
	{
		return !static_cast<const ChildClass*>(this)->IsButtonDown(button) && GetCorrespondingOldState().IsButtonDown(button);
	}


	template<typename ChildClass, typename ButtonType>
	inline const ChildClass& InputManager::DeviceState<ChildClass, ButtonType>::GetCorrespondingOldState() const
	{
		assert(InputManager::Instance().GetState<ChildClass>(clientID, clientDeviceID) == this && "GetOldState can only be called on the current readState");
		const auto& ownState = InputManager::Instance().readState.GetStateCollection<ChildClass>().states;

		size_t ownIndex = static_cast<size_t>(reinterpret_cast<const ChildClass*>(this) - &ownState.front());
		assert(InputManager::Instance().oldReadState.GetStateCollection<ChildClass>().states.size() > ownIndex && "Old read state has not as many elements as new read state");

		return InputManager::Instance().oldReadState.GetStateCollection<ChildClass>().states[ownIndex];
	}

	inline bool InputManager::MouseState::IsButtonDown(MouseButton button) const
	{
		return (static_cast<uint8_t>(state.buttonState) & static_cast<uint8_t>(button)) > 0;
	}

	inline bool InputManager::KeyboardState::IsButtonDown(KeyboardKey button) const
	{
		return state.pressedKeys[0] == button ||
			state.pressedKeys[1] == button ||
			state.pressedKeys[2] == button ||
			state.pressedKeys[3] == button ||
			state.pressedKeys[4] == button ||
			state.pressedKeys[5] == button ||
			state.pressedKeys[6] == button ||
			state.pressedKeys[7] == button;
	}

	inline bool InputManager::GamepadState::IsButtonDown(GamepadButton button) const
	{
		return (static_cast<uint16_t>(state.buttons) & static_cast<uint16_t>(button)) > 0;
	}

	template<typename StateType>
	inline const StateType* InputManager::GetState(unsigned int globalDeviceID) const
	{
		if (globalDeviceID >= readState.GetStateCollection<StateType>().states.size())
			return nullptr;
		return &readState.GetStateCollection<StateType>().states[globalDeviceID];
	}

	template<typename StateType>
	inline const StateType* InputManager::GetState(uint16_t clientID, uint8_t clientDeviceID) const
	{
		for (const StateType& state : readState.GetStateCollection<StateType>().states)
		{
			if (state.clientID == clientID && state.clientDeviceID == clientDeviceID)
				return &state;
		}
		return nullptr;
	}

	template<typename StateType>
	inline size_t InputManager::GetNumDevices() const
	{
		return static_cast<const DeviceStateCollection<StateType>*>(&readState)->states.size();
	}

	template<typename StateType>
	inline size_t InputManager::GetNumConnectedDevices() const
	{
		return static_cast<const DeviceStateCollection<StateType>*>(&readState)->numConnected;
	}



	inline const InputManager::MouseState* InputManager::GetStateMouse(unsigned int globalDeviceID) const
	{
		return GetState<MouseState>(globalDeviceID);
	}
	inline const InputManager::MouseState* InputManager::GetStateMouse(uint16_t clientID, uint8_t clientDeviceID) const
	{
		return GetState<MouseState>(clientID, clientDeviceID);
	}
	inline size_t InputManager::GetNumMouses() const
	{
		return GetNumDevices<MouseState>();
	}
	inline size_t InputManager::GetNumConnectedMouses() const
	{
		return GetNumConnectedDevices<MouseState>();
	}


	inline const InputManager::KeyboardState* InputManager::GetStateKeyboard(unsigned int globalDeviceID) const
	{
		return GetState<KeyboardState>(globalDeviceID);
	}
	inline const InputManager::KeyboardState* InputManager::GetStateKeyboard(uint16_t clientID, uint8_t clientDeviceID) const
	{
		return GetState<KeyboardState>(clientID, clientDeviceID);
	}
	inline size_t InputManager::GetNumKeyboards() const
	{
		return GetNumDevices<KeyboardState>();
	}
	inline size_t InputManager::GetNumConnectedKeyboards() const
	{
		return GetNumConnectedDevices<KeyboardState>();
	}


	inline const InputManager::GamepadState* InputManager::GetStateGamepad(unsigned int globalDeviceID) const
	{
		return GetState<GamepadState>(globalDeviceID);
	}
	inline const InputManager::GamepadState* InputManager::GetStateGamepad(uint16_t clientID, uint8_t clientDeviceID) const
	{
		return GetState<GamepadState>(clientID, clientDeviceID);
	}
	inline size_t InputManager::GetNumGamepads() const
	{
		return GetNumDevices<GamepadState>();
	}
	inline size_t InputManager::GetNumConnectedGamepads() const
	{
		return GetNumConnectedDevices<GamepadState>();
	}

	inline void InputManager::SetConnectionTimeout(Time deviceConnectionTimeout)
	{
		this->deviceConnectionTimeout = deviceConnectionTimeout;
	}

	template<typename StateType>
	inline StateType& InputManager::InputState::GetInfoSlot(uint16_t clientID, uint8_t clientDeviceIndex)
	{
		auto& states = GetStateCollection<StateType>().states;

		auto disconnectedSlot = states.end();
		for (auto it = GetStateCollection<StateType>().states.begin(); it != states.end(); ++it)
		{
			if (it->clientID == clientID && it->clientDeviceID == clientDeviceIndex)
				return *it;
			if (it->connected == false)
				disconnectedSlot = it;
		}

		if (disconnectedSlot == states.end())
		{
			states.emplace_back();
			disconnectedSlot = std::prev(states.end());
		}

		disconnectedSlot->clientID = clientID;
		disconnectedSlot->clientDeviceID = clientDeviceIndex;

		return *disconnectedSlot;
	}

	template<typename StateType>
	inline InputManager::DeviceStateCollection<StateType>& InputManager::InputState::GetStateCollection()
	{
		return *static_cast<DeviceStateCollection<StateType>*>(this);
	}

	template<typename StateType>
	inline const InputManager::DeviceStateCollection<StateType>& InputManager::InputState::GetStateCollection() const
	{
		return *static_cast<const DeviceStateCollection<StateType>*>(this);
	}
}