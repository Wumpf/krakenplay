#pragma  once

#include <cinttypes>

#define SET_BIT(x) 1 << (x)

namespace Krakenplay
{
// No padding
#pragma pack(push)
#pragma pack(1)

	/// \brief Specifies the different types of available network messages.
	enum class MessageChunkType : uint8_t
	{
		MOUSE_DISCONNECT,
		MOUSE_STATUS,

		KEYBOARD_DISCONNECT,
		KEYBOARD_STATUS,

		GAMEPAD_DISCONNECT,
		GAMEPAD_STATUS,

		SERVER_IDENTIFY, ///< Device index of chunk header is used for protocol version.

		NUM_MESSAGES
	};

	/// \brief Header for a message chunk.
	///
	/// A single network message can consist of multiple message chunks.
	/// After it follows either a message body with GetMessageBodySize bytes length.
	struct MessageChunkHeader
	{
		MessageChunkType messageType;	///< Message type this chunk. Use GetMessageBodySize to get the size of the following body.
		uint8_t deviceIndex;			///< Most messages are about devices - this is the client-local device index.
	};

	/// Returns the size of the message body for the given type.
	unsigned int GetMessageBodySize(MessageChunkType bodyType);

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

	/// Mouse state object. Body of MessageType::MOUSE_STATUS.
	struct InternalMouseState
	{
		InternalMouseState() : positionX(0), positionY(0), mouseWheel(0), buttonState(static_cast<MouseButton>(0)) {}

		/// \brief Current x-position of the mouse.
		/// 
		/// Note that this value is not dependend on the actual screen position of the corresponding mouse cursor of the client machine.
		/// Instead the InputFetcher accumulates mouse movement deltas.
		int32_t positionX;

		/// \brief Current y-position of the mouse.
		/// 
		/// Note that this value is not dependend on the actual screen position of the corresponding mouse cursor of the client machine.
		/// Instead the InputFetcher accumulates mouse movement deltas.
		int32_t positionY;

		/// \brief Current position of the mouse-wheel.
		/// 
		/// Note that this value is not dependend on the actual screen position of the corresponding mouse cursor of the client machine.
		/// Instead the InputFetcher accumulates mouse movement deltas.
		int32_t mouseWheel;

		/// \brief Current mouse state as bitmask.
		///
		/// If buttonState & static_cast<uint8_t>(MouseButton::<i>button_of_choice</i>) > 0 then <i>button_of_choice</i> is pressed.
		MouseButton buttonState;
	};

#pragma pack(pop)

	/// Converts the endianess of a given message data block (multiple messages, each beginning with MessageChunkHeader) from host to network.
	void ConvertEndiannessHostToNetwork(char* messageBlock, unsigned int blockSize);

	/// Converts the endianess of a given message data block (multiple messages, each beginning with MessageChunkHeader) from network to host.
	void ConvertEndiannessNetworkToHost(char* messageBlock, unsigned int blockSize);

	/// Maximum expected number of milliseconds between two status messages.
	extern const double g_maxDurationBetweenMessage_ms;
	/// Minimum expected number of milliseconds between two status messages.
	/// If an input event demands immediate update it will still be delayed by this timestep.
	extern const double g_minDurationBetweenMessage_ms;

	/// A single network message never exceeds this size in bytes.
	/// This gives a limitation to the number of messages that can be chained together.
	const unsigned int g_maxMessageSize = 2048;

	/// Default port for all normal Krakenplay network messages.
	const uint16_t g_defaultMessagePort = 12445;
	/// Default port for all Krakenplay server identify broadcasts.
	const uint16_t g_defaultIdentifyPort = 12446;

	/// Current protocol version, see MessageChunkType::SERVER_IDENTIFY.
	const uint8_t g_protocolVersion = 0;
}

#undef SET_BIT