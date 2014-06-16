#pragma  once

#include <cinttypes>

#define SET_BIT(x) 1 << (x)

namespace Krakenplay
{
// No padding
#pragma pack(push)
#pragma pack(1)

	/// Specifies the different types of available network messages.
	enum class MessageChunkType : uint8_t
	{
		MOUSE_DISCONNECT,
		MOUSE_STATUS
	};

	/// Header for a message chunk. A single network message can consist of multiple message chunks.
	/// After it follows either a message body with GetMessageBodySize bytes length.
	struct MessageChunkHeader
	{
		MessageChunkType messageType;
		uint8_t deviceIndex; ///< Most messages are about devices - this is the client-local device index.
	};

	/// Returns the size of the message body for the given type.
	unsigned int GetMessageBodySize(MessageChunkType bodyType);

	/// Structures describing the state of an input device.
	namespace StateObjects
	{
		/// Mouse button bit mask definition. Compatible to OIS::MouseButtonID
		enum class MouseButton : uint8_t
		{
			LEFT = SET_BIT(0),
			RIGHT = SET_BIT(1),
			MIDDLE = SET_BIT(2),
			BUTTON3 = SET_BIT(3),
			BUTTON4 = SET_BIT(4),
			BUTTON5 = SET_BIT(5),
			BUTTON6 = SET_BIT(6),
			BUTTON7 = SET_BIT(7),

			NUM_BUTTONS = 8
		};

		/// Mouse state object. Part of MessageType::MOUSE_STATUS
		struct MouseState
		{
			int32_t positionX;
			int32_t positionY;
			int32_t mouseWheel;
			MouseButton buttonState;
		};
	}

#pragma pack(pop)

	/// Maximum expected number of milliseconds between two status messages.
	extern const double g_maxDurationBetweenMessage_ms;
	/// Minimum expected number of milliseconds between two status messages.
	/// If an input event demands immediate update it will still be delayed by this timestep.
	extern const double g_minDurationBetweenMessage_ms;

	/// A single network message never exceeds this size in bytes.
	/// This gives a limitation to the number of messages that can be chained together.
	const unsigned int g_maxMessageSize = 2048;
}

#undef SET_BIT