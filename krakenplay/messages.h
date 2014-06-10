#pragma  once

#include <cinttypes>

namespace Krakenplay
{
	// TODO: Define message data structures

	const uint8_t currentProtocolVersion = 0;

	enum class MessageType : uint8_t
	{
		GAMEPAD_DISCONNECT,
		GAMEPAD_STATUS
	};

	struct MessageHeader
	{
		uint8_t messageType;
	};
}