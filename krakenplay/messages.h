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
	/// \see InternalMouseState
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

		/// \brief Current mouse button state as bitmask.
		///
		/// If buttonState & static_cast<uint8_t>(MouseButton::<i>button_of_choice</i>) > 0 then <i>button_of_choice</i> is pressed.
		MouseButton buttonState;
	};

	/// Mouse button bit mask definition.
	///
	/// Fully compatible with XINPUT_GAMEPAD (http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinput_gamepad%28v=vs.85%29.aspx)
	/// \see InternalGamepadState
	enum class GamepadButton : uint16_t
	{
		DPAD_UP = SET_BIT(0),
		DPAD_DOWN = SET_BIT(1),
		DPAD_LEFT = SET_BIT(2),
		DPAD_RIGHT = SET_BIT(3),
		START = SET_BIT(4),
		BACK = SET_BIT(5),
		LEFT_THUMB = SET_BIT(6),
		RIGHT_THUMB = SET_BIT(7),
		LEFT_SHOULDER = SET_BIT(8),
		RIGHT_SHOULDER = SET_BIT(9),
		A = SET_BIT(10),
		B = SET_BIT(11),
		X = SET_BIT(12),
		Y = SET_BIT(13),

		NUM_BUTTONS = 14
	};

	/// \brief Gamepad state object. Body of MessageType::GAMEPAD_STATUS.
	///
	/// Yes, this resembles exactly an XBox360 Gamepad, since this is the defacto standard on computers at the moment.
	/// Krakenplay will try to map any controller on any system correctly to this structure. However the mapping can go wrong in many ways.
	/// If you encounter any miss-mapping or missing buttons please report or take a look into gamepadfetcher.cpp yourself, thanks!
	/// The structure is fully compatible with XINPUT_GAMEPAD (http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinput_gamepad%28v=vs.85%29.aspx)
	struct InternalGamepadState
	{
		/// \brief Current gamepad button state as bitmask.
		///
		/// If buttonState & static_cast<uint8_t>(GamepadButton::<i>button_of_choice</i>) > 0 then <i>button_of_choice</i> is pressed.
		GamepadButton buttons;
		
		/// The current value of the left trigger analog control. The value is between 0 and 255.
		int8_t leftTrigger;
		/// The current value of the right trigger analog control. The value is between 0 and 255.
		int8_t rightTrigger;

		/// \brief Left thumbstick x-axis value. The value is between -32768 and 32767.
		/// 
		/// Usually a value of 0 means centered. However a deadzone may be needed!
		int16_t thumbLX;
		/// \brief Left thumbstick y-axis value. The value is between -32768 and 32767.
		/// 
		/// Usually a value of 0 means centered. However a deadzone may be needed!
		int16_t thumbLY;

		/// \brief Right thumbstick x-axis value. The value is between -32768 and 32767.
		/// 
		/// Usually a value of 0 means centered. However a deadzone may be needed!
		int16_t thumbRX;
		/// \brief Right thumbstick y-axis value. The value is between -32768 and 32767.
		/// 
		/// Usually a value of 0 means centered. However a deadzone may be needed!
		int16_t thumbRY;
	};

	/// Keyboard key code. Compatible to OIS.
	/// \see InternalKeyboardState
	enum class KeyboardKey : uint8_t
	{
		UNASSIGNED  = 0x00,
		ESCAPE      = 0x01,
		NUM1        = 0x02,
		NUM2        = 0x03,
		NUM3        = 0x04,
		NUM4        = 0x05,
		NUM5        = 0x06,
		NUM6        = 0x07,
		NUM7        = 0x08,
		NUM8        = 0x09,
		NUM9        = 0x0A,
		NUM0        = 0x0B,
		MINUS       = 0x0C,    ///< - on main keyboard
		EQUALS      = 0x0D,
		BACK        = 0x0E,    ///< backspace
		TAB         = 0x0F,
		Q           = 0x10,
		W           = 0x11,
		E           = 0x12,
		R           = 0x13,
		T           = 0x14,
		Y           = 0x15,
		U           = 0x16,
		I           = 0x17,
		O           = 0x18,
		P           = 0x19,
		LBRACKET    = 0x1A,
		RBRACKET    = 0x1B,
		RETURN      = 0x1C,    ///< Enter on main keyboard
		LCONTROL    = 0x1D,
		A           = 0x1E,
		S           = 0x1F,
		D           = 0x20,
		F           = 0x21,
		G           = 0x22,
		H           = 0x23,
		J           = 0x24,
		K           = 0x25,
		L           = 0x26,
		SEMICOLON   = 0x27,
		APOSTROPHE  = 0x28,
		GRAVE       = 0x29,    ///< accent
		LSHIFT      = 0x2A,
		BACKSLASH   = 0x2B,
		Z           = 0x2C,
		X           = 0x2D,
		C           = 0x2E,
		V           = 0x2F,
		B           = 0x30,
		N           = 0x31,
		M           = 0x32,
		COMMA       = 0x33,
		PERIOD      = 0x34,    ///< . on main keyboard
		SLASH       = 0x35,    ///< / on main keyboard
		RSHIFT      = 0x36,
		MULTIPLY    = 0x37,    ///< * on numeric keypad
		LMENU       = 0x38,    ///< left Alt
		SPACE       = 0x39,
		CAPITAL     = 0x3A,
		F1          = 0x3B,
		F2          = 0x3C,
		F3          = 0x3D,
		F4          = 0x3E,
		F5          = 0x3F,
		F6          = 0x40,
		F7          = 0x41,
		F8          = 0x42,
		F9          = 0x43,
		F10         = 0x44,
		NUMLOCK     = 0x45,
		SCROLL      = 0x46,    ///< Scroll Lock
		NUMPAD7     = 0x47,
		NUMPAD8     = 0x48,
		NUMPAD9     = 0x49,
		SUBTRACT    = 0x4A,    ///< - on numeric keypad
		NUMPAD4     = 0x4B,
		NUMPAD5     = 0x4C,
		NUMPAD6     = 0x4D,
		ADD         = 0x4E,    ///< + on numeric keypad
		NUMPAD1     = 0x4F,
		NUMPAD2     = 0x50,
		NUMPAD3     = 0x51,
		NUMPAD0     = 0x52,
		DECIMAL     = 0x53,    ///< . on numeric keypad
		OEM_102     = 0x56,    ///< < > | on UK/Germany keyboards
		F11         = 0x57,
		F12         = 0x58,
		F13         = 0x64,    ///<                     (NEC PC98)
		F14         = 0x65,    ///<                     (NEC PC98)
		F15         = 0x66,    ///<                     (NEC PC98)
		KANA        = 0x70,    ///< (Japanese keyboard)
		ABNT_C1     = 0x73,    ///< / ? on Portugese (Brazilian) keyboards
		CONVERT     = 0x79,    ///< (Japanese keyboard)
		NOCONVERT   = 0x7B,    ///< (Japanese keyboard)
		YEN         = 0x7D,    ///< (Japanese keyboard)
		ABNT_C2     = 0x7E,    ///< Numpad . on Portugese (Brazilian) keyboards
		NUMPADEQUALS= 0x8D,    ///< = on numeric keypad (NEC PC98)
		PREVTRACK   = 0x90,    ///< Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
		AT          = 0x91,    ///<                     (NEC PC98)
		COLON       = 0x92,    ///<                     (NEC PC98)
		UNDERLINE   = 0x93,    ///<                     (NEC PC98)
		KANJI       = 0x94,    ///< (Japanese keyboard)
		STOP        = 0x95,    ///<                     (NEC PC98)
		AX          = 0x96,    ///<                     (Japan AX)
		UNLABELED   = 0x97,    ///<                        (J3100)
		NEXTTRACK   = 0x99,    ///< Next Track
		NUMPADENTER = 0x9C,    ///< Enter on numeric keypad
		RCONTROL    = 0x9D,
		MUTE        = 0xA0,    ///< Mute
		CALCULATOR  = 0xA1,    ///< Calculator
		PLAYPAUSE   = 0xA2,    ///< Play / Pause
		MEDIASTOP   = 0xA4,    ///< Media Stop
		VOLUMEDOWN  = 0xAE,    ///< Volume -
		VOLUMEUP    = 0xB0,    ///< Volume +
		WEBHOME     = 0xB2,    ///< Web home
		NUMPADCOMMA = 0xB3,    ///< , on numeric keypad (NEC PC98)
		DIVIDE      = 0xB5,    ///< / on numeric keypad
		SYSRQ       = 0xB7,
		RMENU       = 0xB8,    ///< right Alt
		PAUSE       = 0xC5,    ///< Pause
		HOME        = 0xC7,    ///< Home on arrow keypad
		UP          = 0xC8,    ///< UpArrow on arrow keypad
		PGUP        = 0xC9,    ///< PgUp on arrow keypad
		LEFT        = 0xCB,    ///< LeftArrow on arrow keypad
		RIGHT       = 0xCD,    ///< RightArrow on arrow keypad
		END         = 0xCF,    ///< End on arrow keypad
		DOWN        = 0xD0,    ///< DownArrow on arrow keypad
		PGDOWN      = 0xD1,    ///< PgDn on arrow keypad
		INSERT      = 0xD2,    ///< Insert on arrow keypad
		DEL         = 0xD3,    ///< Delete on arrow keypad
		LWIN        = 0xDB,    ///< Left Windows key
		RWIN        = 0xDC,    ///< Right Windows key
		APPS        = 0xDD,    ///< AppMenu key
		POWER       = 0xDE,    ///< System Power
		SLEEP       = 0xDF,    ///< System Sleep
		WAKE        = 0xE3,    ///< System Wake
		WEBSEARCH   = 0xE5,    ///< Web Search
		WEBFAVORITES= 0xE6,    ///< Web Favorites
		WEBREFRESH  = 0xE7,    ///< Web Refresh
		WEBSTOP     = 0xE8,    ///< Web Stop
		WEBFORWARD  = 0xE9,    ///< Web Forward
		WEBBACK     = 0xEA,    ///< Web Back
		MYCOMPUTER  = 0xEB,    ///< My Computer
		MAIL        = 0xEC,    ///< Mail
		MEDIASELECT = 0xED,    ///< Media Select
		
		NUM_KEYS = 0xEF, ///<
		NONE = 0xFF
	};

	/// Maps keyboard keys to strings.
	extern const char* const keyboardKeyToStringMap[static_cast<unsigned int>(KeyboardKey::NUM_KEYS)];

	/// \brief Keyboard state object. Contains a fixed number of currently pressed keys.
	struct InternalKeyboardState
	{
		/// \brief List of currently pressed keys.
		///
		/// Contains up to 8 valid keys. Krakenplays guarantes that after the first KeyboardKey::NONE in the list, no valid key will follow.
		/// All keys that are not mentioned in this list should be assumed to be up.
		KeyboardKey pressedKeys[8]; 
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