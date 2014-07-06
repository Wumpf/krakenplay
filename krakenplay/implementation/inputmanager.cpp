#include "../inputmanager.h"
#include <cassert>
#include <cstring>
#include <iostream>

namespace Krakenplay
{
	InputManager& InputManager::Instance()
	{
		static InputManager oneAndOnly;
		return oneAndOnly;
	}

	InputManager::InputManager()
	{
		SetConnectionTimeout();
	}

	InputManager::~InputManager()
	{
	}

	template<typename StateList>
	void HandleNewConnects(StateList& newStateList, StateList& oldStateList)
	{
		for (unsigned int i = 0; i < newStateList.size(); ++i)
		{
			if (oldStateList.size() <= i)
			{
				oldStateList.push_back(newStateList[i]);
				oldStateList.back().connected = false;
			}

			else if (newStateList[i].WasConnected())
			{
				oldStateList[i] = newStateList[i];
				oldStateList[i].connected = false;
			}
		}
	}

	template<typename StateList>
	void HandleConnectionTimeouts(StateList& writeState, size_t& connectionCounter, Time deviceConnectionTimeout)
	{
		Time now = Time::Now();
		connectionCounter = 0;
		for (auto& info : writeState)
		{
			info.connected = info.connected && (now - info.lastUpdate < deviceConnectionTimeout);
			if (info.connected)
				++connectionCounter;
		}
	}

	void InputManager::Update()
	{
		oldReadState = readState;

		inputWriteMutex.lock();

		// Check device connection timeouts.
		HandleConnectionTimeouts(writeState.GetStateCollection<MouseState>().states, writeState.GetStateCollection<MouseState>().numConnected, deviceConnectionTimeout);
		HandleConnectionTimeouts(writeState.GetStateCollection<KeyboardState>().states, writeState.GetStateCollection<KeyboardState>().numConnected, deviceConnectionTimeout);
		HandleConnectionTimeouts(writeState.GetStateCollection<GamepadState>().states, writeState.GetStateCollection<GamepadState>().numConnected, deviceConnectionTimeout);

		// Copy write state to read state.
		readState = writeState;

		inputWriteMutex.unlock();

		// For state that switched from disconnected to connected, reset state to current state to avoid wrong state changes.
		HandleNewConnects(readState.GetStateCollection<MouseState>().states, oldReadState.GetStateCollection<MouseState>().states);
		HandleNewConnects(readState.GetStateCollection<GamepadState>().states, oldReadState.GetStateCollection<GamepadState>().states);
		HandleNewConnects(readState.GetStateCollection<KeyboardState>().states, oldReadState.GetStateCollection<KeyboardState>().states);
	}

	void InputManager::ReceiveInput(const MessageChunkHeader& header, const void* messageBody, unsigned int messageBodySize, uint16_t clientID)
	{
		assert(messageBody != nullptr || (messageBody == nullptr && messageBodySize == 0) && "Invalid message body pointer!");
		assert(GetMessageBodySize(header.messageType) == messageBodySize && "Invalid message body size!");

		inputWriteMutex.lock();

		switch (header.messageType)
		{
		case MessageChunkType::MOUSE_STATUS:
		{
			MouseState& infoSlot = writeState.GetInfoSlot<MouseState>(clientID, header.deviceIndex);
			infoSlot.connected = true;
			infoSlot.lastUpdate = Time::Now();
			memcpy(&infoSlot.state, messageBody, messageBodySize);
			break;
		}

		case MessageChunkType::MOUSE_DISCONNECT:
		{
			MouseState& infoSlot = writeState.GetInfoSlot<MouseState>(clientID, header.deviceIndex);
			infoSlot.connected = false;
			infoSlot.lastUpdate = Time::Now();
			break;
		}

		case MessageChunkType::KEYBOARD_STATUS:
		{
			KeyboardState& infoSlot = writeState.GetInfoSlot<KeyboardState>(clientID, header.deviceIndex);
			infoSlot.connected = true;
			infoSlot.lastUpdate = Time::Now();
			memcpy(&infoSlot.state, messageBody, messageBodySize);
			break;
		}

		case MessageChunkType::KEYBOARD_DISCONNECT:
		{
			KeyboardState& infoSlot = writeState.GetInfoSlot<KeyboardState>(clientID, header.deviceIndex);
			infoSlot.connected = false;
			infoSlot.lastUpdate = Time::Now();
			break;
		}

		case MessageChunkType::GAMEPAD_STATUS:
		{
			GamepadState& infoSlot = writeState.GetInfoSlot<GamepadState>(clientID, header.deviceIndex);
			infoSlot.connected = true;
			infoSlot.lastUpdate = Time::Now();
			memcpy(&infoSlot.state, messageBody, messageBodySize);
			break;
		}

		case MessageChunkType::GAMEPAD_DISCONNECT:
		{
			GamepadState& infoSlot = writeState.GetInfoSlot<GamepadState>(clientID, header.deviceIndex);
			infoSlot.connected = false;
			infoSlot.lastUpdate = Time::Now();
			break;
		}

		default:
			assert(false && "Unknown MessageChunkType!");
			break;
		}

		inputWriteMutex.unlock();
	}
}
