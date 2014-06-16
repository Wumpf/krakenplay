namespace Krakenplay
{
	inline bool InputManager::MouseInfo::IsButtonDown(StateObjects::MouseButton button) const
	{
		return (static_cast<uint8_t>(state.buttonState) & static_cast<uint8_t>(button)) > 0;
	}

	inline bool InputManager::MouseInfo::WasButtonPressed(StateObjects::MouseButton button) const
	{
		return IsButtonDown(button) && !GetOldState().IsButtonDown(button);
	}
	inline bool InputManager::MouseInfo::WasButtonReleased(StateObjects::MouseButton button) const
	{
		return !IsButtonDown(button) && GetOldState().IsButtonDown(button);
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
}