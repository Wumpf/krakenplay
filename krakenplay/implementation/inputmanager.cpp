#include "../inputmanager.h"
#include <assert.h>
#include <iostream>
namespace Krakenplay
{
	InputManager& InputManager::Instance()
	{
		static InputManager server;
		return server;
	}

	InputManager::InputManager()
	{
		SetConnectionTimeout();
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::Update()
	{
		oldReadState = readState;

		inputWriteMutex.lock();

		// Check device connection timeouts.
		Time now = Time::Now(); // Time::Now can be costly, so do it only once per update.
		writeState.numConnectedMouses = 0;
		for (MouseInfo& info : writeState.mouseStates)
		{
			info.connected = info.connected && (now - info.lastUpdate < deviceConnectionTimeout);
			if (info.connected)
				++writeState.numConnectedMouses;
		}

		// Copy write state to read state.
		readState = writeState;

		inputWriteMutex.unlock();

		// If oldReadState does not have as many states as writeState, read errors occur.
		for (unsigned int i = oldReadState.mouseStates.size(); i < readState.mouseStates.size(); ++i)
		{
			oldReadState.mouseStates.push_back(readState.mouseStates[i]);
			oldReadState.mouseStates.back().connected = false;
		}
	}

	const InputManager::MouseInfo* InputManager::GetMouseState(unsigned int globalDeviceID) const
	{
		if (globalDeviceID >= readState.mouseStates.size())
			return nullptr;
		return &readState.mouseStates[globalDeviceID];
	}

	const InputManager::MouseInfo* InputManager::GetMouseState(unsigned int clientID, uint8_t clientDeviceID) const
	{
		for (const MouseInfo& mouseInfo : readState.mouseStates)
		{
			if (mouseInfo.clientID == clientID && mouseInfo.clientDeviceID == clientID)
				return &mouseInfo;
		}
		return nullptr;
	}

	const InputManager::MouseInfo& InputManager::MouseInfo::GetOldState() const
	{
		assert(InputManager::Instance().GetMouseState(clientID, clientDeviceID) == this && "GetOldState can only be called on the current readState");
		auto& ownMouseState = InputManager::Instance().readState.mouseStates;
	
		size_t ownIndex = static_cast<size_t>(&ownMouseState.front() - this);
		assert(InputManager::Instance().oldReadState.mouseStates.size() > ownIndex && "Old read state has not as many elements as new read state");

		return InputManager::Instance().oldReadState.mouseStates[ownIndex];
	}

	void InputManager::ReceiveInput(const MessageChunkHeader& header, const void* messageBody, unsigned int messageBodySize, unsigned int clientIndex)
	{
		assert(messageBody != nullptr || (messageBody == nullptr && messageBodySize == 0) && "Invalid message body pointer!");
		assert(messageBodySize != 0 && GetMessageBodySize(header.messageType) == messageBodySize && "Invalid message body size!");

		inputWriteMutex.lock();

		switch (header.messageType)
		{
		case MessageChunkType::MOUSE_STATUS:
		{
			auto mouseInfoSlot = writeState.GetMouseInfoSlot(clientIndex, header.deviceIndex);
			mouseInfoSlot->connected = true;
			mouseInfoSlot->lastUpdate = Time::Now();
			memcpy(&mouseInfoSlot->state, messageBody, messageBodySize);
			break;
		}

		case MessageChunkType::MOUSE_DISCONNECT:
		{
			auto mouseInfoSlot = writeState.GetMouseInfoSlot(clientIndex, header.deviceIndex);
			mouseInfoSlot->connected = false;
			mouseInfoSlot->lastUpdate = Time::Now();
			break;
		}

		default:
			break;
		}

		inputWriteMutex.unlock();
	}

	std::vector<InputManager::MouseInfo>::iterator InputManager::InputState::GetMouseInfoSlot(unsigned int clientIndex, uint8_t clientDeviceIndex)
	{
		auto disconnectedSlot = mouseStates.end();
		for (auto it = mouseStates.begin(); it != mouseStates.end(); ++it)
		{
			if (it->clientID == clientIndex && it->clientDeviceID == clientIndex)
				return it;
			if (it->connected == false)
				disconnectedSlot = it;
		}

		if (disconnectedSlot == mouseStates.end())
		{
			mouseStates.emplace_back();
			disconnectedSlot = mouseStates.end() - 1;
		}

		disconnectedSlot->clientID = clientIndex;
		disconnectedSlot->clientDeviceID = clientDeviceIndex;

		return disconnectedSlot;
	}
}