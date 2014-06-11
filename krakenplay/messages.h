#pragma  once

#include <cinttypes>

namespace Krakenplay
{
	// TODO: Define message data structures

	enum class MessageType : uint8_t
	{
		KEYBOARD_DISCONNECT,
		KEYBOARD_STATUS
	};

	struct MessageHeader
	{
		uint8_t messageType;
	};

	
}