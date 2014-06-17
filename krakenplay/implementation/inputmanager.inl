namespace Krakenplay
{
	template<typename ChildClass>
	bool InputManager::DeviceState<ChildClass>::WasDisconnected() const
	{
		return !connected && GetOldState().connected;
	}

	template<typename ChildClass>
	bool InputManager::DeviceState<ChildClass>::WasConnected() const
	{
		return connected && !GetOldState().connected;
	}

	template<typename ChildClass>
	inline const ChildClass& InputManager::DeviceState<ChildClass>::GetOldState() const
	{
		assert(InputManager::Instance().GetState<ChildClass>(clientID, clientDeviceID) == this && "GetOldState can only be called on the current readState");
		auto& ownState = InputManager::Instance().readState.GetDeviceStates<ChildClass>();

		size_t ownIndex = static_cast<size_t>(&ownState.front() - reinterpret_cast<const ChildClass*>(this));
		assert(InputManager::Instance().oldReadState.GetDeviceStates<ChildClass>().size() > ownIndex && "Old read state has not as many elements as new read state");

		return InputManager::Instance().oldReadState.GetDeviceStates<ChildClass>()[ownIndex];
	}

	inline bool InputManager::MouseState::IsButtonDown(MouseButton button) const
	{
		return (static_cast<uint8_t>(state.buttonState) & static_cast<uint8_t>(button)) > 0;
	}

	inline bool InputManager::MouseState::WasButtonPressed(MouseButton button) const
	{
		return IsButtonDown(button) && !GetOldState().IsButtonDown(button);
	}
	inline bool InputManager::MouseState::WasButtonReleased(MouseButton button) const
	{
		return !IsButtonDown(button) && GetOldState().IsButtonDown(button);
	}

	template<typename StateType>
	const StateType* InputManager::GetState(unsigned int globalDeviceID) const
	{
		if(globalDeviceID >= readState.GetDeviceStates<StateType>().size())
			return nullptr;
		return &readState.GetDeviceStates<StateType>()[globalDeviceID];
	}

	template<typename StateType>
	const StateType* InputManager::GetState(unsigned int clientID, uint8_t clientDeviceID) const
	{
		for(const StateType& state : readState.GetDeviceStates<StateType>())
		{
			if(state.clientID == clientID && state.clientDeviceID == clientID)
				return &state;
		}
		return nullptr;
	}

	inline unsigned int InputManager::GetNumMouses() const
	{
		return readState.mouseStates.size();
	}

	inline unsigned int InputManager::GetNumConnectedMouses() const
	{
		return readState.numConnectedMouses;
	}

	inline void InputManager::SetConnectionTimeout(Time deviceConnectionTimeout)
	{
		this->deviceConnectionTimeout = deviceConnectionTimeout;
	}

	template<>
	inline const std::vector<InputManager::MouseState>& InputManager::InputState::GetDeviceStates<InputManager::MouseState>() const
	{
		return mouseStates;
	}
}