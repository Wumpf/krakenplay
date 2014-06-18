#include "../inputmanager.h"
#include <assert.h>
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

	void InputManager::Update()
	{
		oldReadState = readState;

		inputWriteMutex.lock();

		// Check device connection timeouts.
		Time now = Time::Now(); // Time::Now can be costly, so do it only once per update.
		writeState.numConnectedMouses = 0;
		for (MouseState& info : writeState.mouseStates)
		{
			info.connected = info.connected && (now - info.lastUpdate < deviceConnectionTimeout);
			if (info.connected)
				++writeState.numConnectedMouses;
		}

		// Copy write state to read state.
		readState = writeState;

		inputWriteMutex.unlock();

		// For state that switched from disconnected to connected, reset state to current state to avoid wrong state changes.
		for(unsigned int i = 0; i < readState.mouseStates.size(); ++i)
		{
			if(oldReadState.mouseStates.size() <= i)
				oldReadState.mouseStates.push_back(readState.mouseStates[i]);

			if(readState.mouseStates[i].WasConnected())
			{
				// Not as many old-read states? Create one.
				assert(oldReadState.mouseStates[i].clientDeviceID == readState.mouseStates[i].clientDeviceID &&
						oldReadState.mouseStates[i].clientID == readState.mouseStates[i].clientID && "Inconsistent IDs across new and old read state!");
				oldReadState.mouseStates[i] = readState.mouseStates[i];
				oldReadState.mouseStates[i].connected = false;
			}
		}
	}

	void InputManager::ReceiveInput(const MessageChunkHeader& header, const void* messageBody, unsigned int messageBodySize, unsigned int clientID)
	{
		assert(messageBody != nullptr || (messageBody == nullptr && messageBodySize == 0) && "Invalid message body pointer!");
		assert(messageBodySize != 0 && GetMessageBodySize(header.messageType) == messageBodySize && "Invalid message body size!");

		inputWriteMutex.lock();

		switch (header.messageType)
		{
		case MessageChunkType::MOUSE_STATUS:
		{
			auto mouseInfoSlot = writeState.GetInfoSlot<MouseState>(clientID, header.deviceIndex);
			mouseInfoSlot.connected = true;
			mouseInfoSlot.lastUpdate = Time::Now();
			memcpy(&mouseInfoSlot.state, messageBody, messageBodySize);
			break;
		}

		case MessageChunkType::MOUSE_DISCONNECT:
		{
			auto mouseInfoSlot = writeState.GetInfoSlot<MouseState>(clientID, header.deviceIndex);
			mouseInfoSlot.connected = false;
			mouseInfoSlot.lastUpdate = Time::Now();
			break;
		}

		default:
			assert(false && "Unknown MessageChunkType!");
			break;
		}

		inputWriteMutex.unlock();
	}
}