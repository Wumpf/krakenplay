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

	template<bool hostToNetwork, typename DataType>
	void ConvertEndianess(DataType& data);

	template<> void ConvertEndianess<true, uint32_t>(uint32_t& data)
	{
		data = htonl(data);
	}
	template<> void ConvertEndianess<false, uint32_t>(uint32_t& data)
	{
		data = ntohl(data);
	}
	template<> void ConvertEndianess<true, int32_t>(int32_t& data)
	{
		*reinterpret_cast<uint32_t*>(&data) = htonl(*reinterpret_cast<uint32_t*>(&data));
	}
	template<> void ConvertEndianess<false, int32_t>(int32_t& data)
	{
		*reinterpret_cast<uint32_t*>(&data) = ntohl(*reinterpret_cast<uint32_t*>(&data));
	}

	template<> void ConvertEndianess<true, uint16_t>(uint16_t& data)
	{
		data = htons(data);
	}
	template<> void ConvertEndianess<false, uint16_t>(uint16_t& data)
	{
		data = ntohs(data);
	}
	template<> void ConvertEndianess<true, int16_t>(int16_t& data)
	{
		*reinterpret_cast<uint32_t*>(&data) = htons(*reinterpret_cast<uint32_t*>(&data));
	}
	template<> void ConvertEndianess<false, int16_t>(int16_t& data)
	{
		*reinterpret_cast<uint16_t*>(&data) = ntohs(*reinterpret_cast<uint16_t*>(&data));
	}

	template<> void ConvertEndianess<true, float>(float& data)
	{
		*reinterpret_cast<unsigned int*>(&data) = htonf(data);
	}
	template<> void ConvertEndianess<false, float>(float& data)
	{
		data = ntohf(*reinterpret_cast<unsigned int*>(&data));
	}

	template<bool hostToNetwork>
	void ConvertEndianessPackageData(char* messageBlock, unsigned int blockSize)
	{
		unsigned int readPos = 0;
		while(readPos < blockSize)
		{
			// Read header - consists only of 8bit blocks that need no byte reordering!
			MessageChunkHeader* header = reinterpret_cast<MessageChunkHeader*>(&messageBlock[readPos]);
			readPos += sizeof(MessageChunkHeader);
			unsigned int messageBodySize = GetMessageBodySize(header->messageType);

			// Pass to input manager.
			switch(header->messageType)
			{
			case MessageChunkType::MOUSE_DISCONNECT:
				break;
			case MessageChunkType::MOUSE_STATUS:
			{
				InternalMouseState* internalMouseState = reinterpret_cast<InternalMouseState*>(&messageBlock[readPos]);
				ConvertEndianess<hostToNetwork>(internalMouseState->mouseWheel);
				ConvertEndianess<hostToNetwork>(internalMouseState->positionX);
				ConvertEndianess<hostToNetwork>(internalMouseState->positionY);
				break;
			}

			default:
				assert(false && "Unknown network message type for endianess conversion!");
			}
			readPos += messageBodySize;
		}
	}

	void ConvertEndiannessHostToNetwork(char* messageBlock, unsigned int blockSize)
	{
		ConvertEndianessPackageData<true>(messageBlock, blockSize);
	}
	void ConvertEndiannessNetworkToHost(char* messageBlock, unsigned int blockSize)
	{
		ConvertEndianessPackageData<false>(messageBlock, blockSize);
	}
}