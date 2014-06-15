#include "../messages.h"
#include <cassert>

namespace Krakenplay
{

	// USB polling rate is by 125hz by default!
	//http://blog.codinghorror.com/mouse-dpi-and-usb-polling-rate/
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
			return sizeof(StateObjects::MouseState);
			break;

		default:
			assert(false && "Unknown message type!");
			return 0;
			break;
		}
	}
}