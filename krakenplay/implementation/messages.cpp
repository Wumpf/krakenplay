#include "../messages.h"
#include <cassert>
#include <WinSock2.h>
#include <type_traits>

namespace Krakenplay
{

	// USB polling rate is by 125hz by default!
	// http://blog.codinghorror.com/mouse-dpi-and-usb-polling-rate/
	// This means that input is usually updated every 8ms

	const double g_maxDurationBetweenMessage_ms = 24.0;
	const double g_minDurationBetweenMessage_ms =  2.0;

	const char g_serverIdentifyMessage[16] = "kraken_01_serve"; // exactly 16 chars!

	const char* const keyboardKeyToStringMap[static_cast<unsigned int>(KeyboardKey::NUM_KEYS)] =
	{
		"unassigned",
		"escape",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",
		"minus",
		"equals",
		"back",
		"tab",
		"q",
		"w",
		"e",
		"r",
		"t",
		"y",
		"u",
		"i",
		"o",
		"p",
		"lbracket",
		"rbracket",
		"return",
		"lcontrol",
		"a",
		"s",
		"d",
		"f",
		"g",
		"h",
		"j",
		"k",
		"l",
		"semicolon",
		"apostrophe",
		"grave",
		"lshift",
		"backslash",
		"z",
		"x",
		"c",
		"v",
		"b",
		"n",
		"m",
		"comma",
		"period",
		"slash",
		"rshift",
		"multiply",
		"lmenu",
		"space",
		"capital",
		"f1",
		"f2",
		"f3",
		"f4",
		"f5",
		"f6",
		"f7",
		"f8",
		"f9",
		"f10",
		"numlock",
		"scroll",
		"numpad7",
		"numpad8",
		"numpad9",
		"subtract",
		"numpad4",
		"numpad5",
		"numpad6",
		"add",
		"numpad1",
		"numpad2",
		"numpad3",
		"numpad0",
		"decimal",
		"oem_102",
		"f11",
		"f12",
		"f13",
		"f14",
		"f15",
		"kana",
		"abnt_c1",
		"convert",
		"noconvert",
		"yen",
		"abnt_c2",
		"numpadequals",
		"prevtrack",
		"at",
		"colon",
		"underline",
		"kanji",
		"stop",
		"ax",
		"unlabeled",
		"nexttrack",
		"numpadenter",
		"rcontrol",
		"mute",
		"calculator",
		"playpause",
		"mediastop",
		"volumedown",
		"volumeup",
		"webhome",
		"numpadcomma",
		"divide",
		"sysrq",
		"rmenu",
		"pause",
		"home",
		"up",
		"pgup",
		"left",
		"right",
		"end",
		"down",
		"pgdown",
		"insert",
		"delete",
		"lwin",
		"rwin",
		"apps",
		"power",
		"sleep",
		"wake",
		"websearch",
		"webfavorites",
		"webrefresh",
		"webstop",
		"webforward",
		"webback",
		"mycomputer",
		"mail",
		"mediaselect"
	};

	unsigned int GetMessageBodySize(MessageChunkType bodyType)
	{
		switch (bodyType)
		{
		case Krakenplay::MessageChunkType::MOUSE_DISCONNECT:
		case Krakenplay::MessageChunkType::GAMEPAD_DISCONNECT:
		case Krakenplay::MessageChunkType::KEYBOARD_DISCONNECT:
		case Krakenplay::MessageChunkType::SERVER_IDENTIFY:
			return 0;
			break;

		case Krakenplay::MessageChunkType::MOUSE_STATUS:
			return sizeof(InternalMouseState);
			break;

		case Krakenplay::MessageChunkType::KEYBOARD_STATUS:
			return sizeof(InternalKeyboardState);
			break;

		case Krakenplay::MessageChunkType::GAMEPAD_STATUS:
			return sizeof(InternalGamepadState);
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
		*reinterpret_cast<uint16_t*>(&data) = htons(*reinterpret_cast<uint16_t*>(&data));
	}
	template<> void ConvertEndianess<false, int16_t>(int16_t& data)
	{
		*reinterpret_cast<uint16_t*>(&data) = ntohs(*reinterpret_cast<uint16_t*>(&data));
	}

	template<> void ConvertEndianess<true, float>(float& data)
	{
        *reinterpret_cast<unsigned int*>(&data) = htonl(*reinterpret_cast<unsigned int*>(&data)); // Should work on all known platforms. htohf exists only in Windows 8.
	}
	template<> void ConvertEndianess<false, float>(float& data)
	{
        unsigned int convertedData = ntohl(*reinterpret_cast<unsigned int*>(&data)); // Should work on all known platforms. ntohf exists only in Windows 8.
        data = *reinterpret_cast<float*>(&convertedData);
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
			case Krakenplay::MessageChunkType::MOUSE_DISCONNECT:
			case Krakenplay::MessageChunkType::GAMEPAD_DISCONNECT:
			case Krakenplay::MessageChunkType::KEYBOARD_DISCONNECT:
			case Krakenplay::MessageChunkType::SERVER_IDENTIFY:
				break;

			case MessageChunkType::MOUSE_STATUS:
			{
				InternalMouseState* internalState = reinterpret_cast<InternalMouseState*>(&messageBlock[readPos]);
				ConvertEndianess<hostToNetwork>(internalState->mouseWheel);
				ConvertEndianess<hostToNetwork>(internalState->positionX);
				ConvertEndianess<hostToNetwork>(internalState->positionY);
				break;
			}

			case Krakenplay::MessageChunkType::KEYBOARD_STATUS:
			{
				break;
			}

			case MessageChunkType::GAMEPAD_STATUS:
			{
				InternalGamepadState* internalState = reinterpret_cast<InternalGamepadState*>(&messageBlock[readPos]);

				ConvertEndianess<hostToNetwork>(*reinterpret_cast<int16_t*>(&internalState->buttons));
				ConvertEndianess<hostToNetwork>(internalState->thumbLX);
				ConvertEndianess<hostToNetwork>(internalState->thumbLY);
				ConvertEndianess<hostToNetwork>(internalState->thumbRX);
				ConvertEndianess<hostToNetwork>(internalState->thumbRY);
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
