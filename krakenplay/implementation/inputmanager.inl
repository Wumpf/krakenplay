namespace Krakenplay
{
	template<typename ChildClass>
	bool InputManager::DeviceState<ChildClass>::WasDisconnected() const
	{
		return !connected && GetCorrespondingOldState().connected;
	}

	template<typename ChildClass>
	bool InputManager::DeviceState<ChildClass>::WasConnected() const
	{
		return connected && !GetCorrespondingOldState().connected;
	}

	template<typename ChildClass>
	inline const ChildClass& InputManager::DeviceState<ChildClass>::GetCorrespondingOldState() const
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
		return IsButtonDown(button) && !GetCorrespondingOldState().IsButtonDown(button);
	}
	inline bool InputManager::MouseState::WasButtonReleased(MouseButton button) const
	{
		return !IsButtonDown(button) && GetCorrespondingOldState().IsButtonDown(button);
	}

	template<typename StateType>
	const StateType* InputManager::GetState(unsigned int globalDeviceID) const
	{
		if(globalDeviceID >= readState.GetDeviceStates<StateType>().size())
			return nullptr;
		return &readState.GetDeviceStates<StateType>()[globalDeviceID];
	}

	template<typename StateType>
	const StateType* InputManager::GetState(uint16_t clientID, uint8_t clientDeviceID) const
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

	template<typename DeviceType>
	DeviceType& InputManager::InputState::GetInfoSlot(uint16_t clientID, uint8_t clientDeviceIndex)
	{
		auto disconnectedSlot = GetDeviceStates<DeviceType>().end();
		for(auto it = GetDeviceStates<DeviceType>().begin(); it != GetDeviceStates<DeviceType>().end(); ++it)
		{
			if(it->clientID == clientID && it->clientDeviceID == clientID)
				return *it;
			if(it->connected == false)
				disconnectedSlot = it;
		}

		if(disconnectedSlot == GetDeviceStates<DeviceType>().end())
		{
			mouseStates.emplace_back();
			disconnectedSlot = std::prev(GetDeviceStates<DeviceType>().end());
		}

		disconnectedSlot->clientID = clientID;
		disconnectedSlot->clientDeviceID = clientDeviceIndex;

		return *disconnectedSlot;
	}

	template<>
	inline const std::vector<InputManager::MouseState>& InputManager::InputState::GetDeviceStates<InputManager::MouseState>() const
	{
		return mouseStates;
	}

	template<>
	inline std::vector<InputManager::MouseState>& InputManager::InputState::GetDeviceStates<InputManager::MouseState>()
	{
		return mouseStates;
	}
}