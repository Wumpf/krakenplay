#include "../inputmanager.h"
#include <assert.h>

namespace Krakenplay
{
	InputManager& InputManager::Instance()
	{
		static InputManager server;
		return server;
	}

	void InputManager::Update()
	{
		receiveInputMutex.lock();
		// TODO
		receiveInputMutex.unlock();
	}

	void InputManager::ReceiveInput(const MessageChunkHeader& header, const void* messageBody, unsigned int messageBodySize, unsigned int clientIndex)
	{
		assert(messageBody != nullptr || (messageBody == nullptr && messageBodySize == 0) && "Invalid message body pointer!");
		assert(messageBodySize != 0 && GetMessageBodySize(header.messageType) == messageBodySize && "Invalid message body size!");

		receiveInputMutex.lock();

		switch (header.messageType)
		{
		case MessageChunkType::MOUSE_STATUS:
		{
			auto mouseInfoSlot = GetMouseInfoSlot(clientIndex, header.deviceIndex);
			mouseInfoSlot->connected = true;
			mouseInfoSlot->lastUpdate = Time::Now();
			memcpy(&mouseInfoSlot->state, messageBody, messageBodySize);
			break;
		}

		case MessageChunkType::MOUSE_DISCONNECT:
		{
			auto mouseInfoSlot = GetMouseInfoSlot(clientIndex, header.deviceIndex);
			mouseInfoSlot->connected = false;
			mouseInfoSlot->lastUpdate = Time::Now();
			break;
		}

		default:
			break;
		}

		receiveInputMutex.unlock();
	}

	std::vector<InputManager::MouseInfo>::iterator InputManager::GetMouseInfoSlot(unsigned int clientIndex, uint8_t clientDeviceIndex)
	{
		auto disconnectedSlot = mouseStates.end();
		for (auto it = mouseStates.begin(); it != mouseStates.end(); ++it)
		{
			if (it->clientIndex == clientIndex && it->clientDeviceIndex == clientIndex)
				return it;
			if (it->connected == false)
				disconnectedSlot = it;
		}

		if (disconnectedSlot == mouseStates.end())
		{
			mouseStates.emplace_back();
			disconnectedSlot = mouseStates.end() - 1;
		}

		disconnectedSlot->clientIndex = clientIndex;
		disconnectedSlot->clientDeviceIndex = clientDeviceIndex;

		return disconnectedSlot;
	}
}