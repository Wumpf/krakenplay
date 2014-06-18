#include "../messages.h"
#include <cassert>
#include <WinSock2.h>

namespace Krakenplay
{

	// USB polling rate is by 125hz by default!
	// http://blog.codinghorror.com/mouse-dpi-and-usb-polling-rate/
	// This means that input is usually updated every 8ms

	const double g_maxDurationBetweenMessage_ms = 24.0;
	const double g_minDurationBetweenMessage_ms =  2.0;

	unsigned int GetMessageBodySize(MessageChunkType bodyType)
	{
		switch (bodyType)
		{
		case Krakenplay::MessageChunkType::MOUSE_DISCONNECT:
			return 0;
			break;

		case Krakenplay::MessageChunkType::MOUSE_STATUS:
			return sizeof(InternalMouseState);
			break;

		default:
			assert(false && "Unknown message type!");
			return 0;
			break;
		}
	}
	/*
	void MessageChunkHeader::ConvertFromNetworkToHostByteOrder()
	{
		// only uint8, nothing to do!
	}

	void MessageChunkHeader::ConvertFromHostToNetworkByteOrder()
	{
		// only uint8, nothing to do!
	}

	void InternalMouseState::ConvertFromNetworkToHostByteOrder()
	{
		*reinterpret_cast<uint32_t*>(positionX) = ntohl(*reinterpret_cast<uint32_t*>(positionX));
		*reinterpret_cast<uint32_t*>(positionY) = ntohl(*reinterpret_cast<uint32_t*>(positionY));
		*reinterpret_cast<uint32_t*>(mouseWheel) = ntohl(*reinterpret_cast<uint32_t*>(mouseWheel));
	}

	void InternalMouseState::ConvertFromHostToNetworkByteOrder()
	{
		*reinterpret_cast<uint32_t*>(positionX) = htonl(*reinterpret_cast<uint32_t*>(positionX));
		*reinterpret_cast<uint32_t*>(positionY) = htonl(*reinterpret_cast<uint32_t*>(positionY));
		*reinterpret_cast<uint32_t*>(mouseWheel) = htonl(*reinterpret_cast<uint32_t*>(mouseWheel));
	}
	*/
}