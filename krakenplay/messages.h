#pragma  once

#include <cinttypes>

#define SET_BIT(x) 1 << (x)

namespace Krakenplay
{
	/// Specifies the different types of available network messages.
	enum class MessageType : uint8_t
	{
		MOUSE_DISCONNECT,
		MOUSE_STATUS
	};

	/// All regular Krakenplay network messages begin with this header.
	struct MessageHeader
	{
		uint8_t messageType;
	};

	/// Structures describing the state of an input device.
	namespace StateObjects
	{
		/// Mouse button bit mask definition.
		enum class MouseButton : uint8_t
		{
			
		};

		/// Mouse state object. Part of MessageType::MOUSE_STATUS
		struct MouseState
		{
			MouseButton buttonState;
		};
	}
}

#undef SET_BIT