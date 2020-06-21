#pragma once

enum class EKeyModifier
{
	None			= 0x00,
	Ctrl			= 0x01,
	Shift			= 0x02,
	Alt				= 0x04,
	DontCare		= 0xFF,

	Ctrl_Shift		= Ctrl | Shift,
	Ctrl_Alt		= Ctrl | Alt,
	Shift_Alt		= Shift | Alt,
	Ctrl_Shift_Alt	= Ctrl | Shift | Alt,
};

static constexpr uint8_t kScanCode_E0_Mask = 0x80;

// Keyboard (scan codes)
enum EKeyCode
{
	// Ordinary scan codes
	KEY_ESCAPE			= 0x01,
	KEY_1				= 0x02,
	KEY_2				= 0x03,
	KEY_3				= 0x04,
	KEY_4				= 0x05,
	KEY_5				= 0x06,
	KEY_6				= 0x07,
	KEY_7				= 0x08,
	KEY_8				= 0x09,
	KEY_9				= 0x0A,
	KEY_0				= 0x0B,
	KEY_MINUS			= 0x0C,		// - on main keyboard
	KEY_EQUALS			= 0x0D,
	KEY_BACKSPACE		= 0x0E,		// backspace
	KEY_TAB				= 0x0F,
	KEY_Q				= 0x10,
	KEY_W				= 0x11,
	KEY_E				= 0x12,
	KEY_R				= 0x13,
	KEY_T				= 0x14,
	KEY_Y				= 0x15,
	KEY_U				= 0x16,
	KEY_I				= 0x17,
	KEY_O				= 0x18,
	KEY_P				= 0x19,
	KEY_LBRACKET		= 0x1A,
	KEY_RBRACKET		= 0x1B,
	KEY_RETURN			= 0x1C,		// Enter on main keyboard
	KEY_LCONTROL		= 0x1D,
	KEY_A				= 0x1E,
	KEY_S				= 0x1F,
	KEY_D				= 0x20,
	KEY_F				= 0x21,
	KEY_G				= 0x22,
	KEY_H				= 0x23,
	KEY_J				= 0x24,
	KEY_K				= 0x25,
	KEY_L				= 0x26,
	KEY_SEMICOLON		= 0x27,
	KEY_APOSTROPHE		= 0x28,
	KEY_GRAVE			= 0x29,		// accent grave
	KEY_LSHIFT			= 0x2A,
	KEY_BACKSLASH		= 0x2B,
	KEY_Z				= 0x2C,
	KEY_X				= 0x2D,
	KEY_C				= 0x2E,
	KEY_V				= 0x2F,
	KEY_B				= 0x30,
	KEY_N				= 0x31,
	KEY_M				= 0x32,
	KEY_COMMA			= 0x33,
	KEY_PERIOD			= 0x34,		// . on main keyboard
	KEY_SLASH			= 0x35,		// / on main keyboard
	KEY_RSHIFT			= 0x36,
	KEY_NUMPADSTAR		= 0x37,		// * on numeric keypad
	KEY_LALT			= 0x38,		// Left Alt
	KEY_SPACE			= 0x39,
	KEY_CAPSLOCK		= 0x3A,
	KEY_F1				= 0x3B,
	KEY_F2				= 0x3C,
	KEY_F3				= 0x3D,
	KEY_F4				= 0x3E,
	KEY_F5				= 0x3F,
	KEY_F6				= 0x40,
	KEY_F7				= 0x41,
	KEY_F8				= 0x42,
	KEY_F9				= 0x43,
	KEY_F10				= 0x44,
	KEY_NUMLOCK			= 0x45,
	KEY_SCROLL			= 0x46,		// Scroll Lock
	KEY_NUMPAD7			= 0x47,
	KEY_NUMPAD8			= 0x48,
	KEY_NUMPAD9			= 0x49,
	KEY_NUMPADMINUS		= 0x4A,		// - on numeric keypad
	KEY_NUMPAD4			= 0x4B,
	KEY_NUMPAD5			= 0x4C,
	KEY_NUMPAD6			= 0x4D,
	KEY_NUMPADPLUS		= 0x4E,		// + on numeric keypad
	KEY_NUMPAD1			= 0x4F,
	KEY_NUMPAD2			= 0x50,
	KEY_NUMPAD3			= 0x51,
	KEY_NUMPAD0			= 0x52,
	KEY_NUMPADPERIOD	= 0x53,		// . on numeric keypad
	KEY_OEM_102			= 0x56,		// <> or \| on RT 102-key keyboard (Non-U.S.)
	KEY_F11				= 0x57,
	KEY_F12				= 0x58,
	// Escaped scan codes
	KEY_NUMPADENTER		= 0x1C | kScanCode_E0_Mask,	// Enter on numeric keypad
	KEY_RCONTROL		= 0x1D | kScanCode_E0_Mask,
	KEY_NUMPADSLASH		= 0x35 | kScanCode_E0_Mask,	// / on numeric keypad
	KEY_RALT			= 0x38 | kScanCode_E0_Mask,	// Right Alt
	KEY_HOME			= 0x47 | kScanCode_E0_Mask,	// Home on arrow keypad
	KEY_UP				= 0x48 | kScanCode_E0_Mask,	// UpArrow on arrow keypad
	KEY_PGUP			= 0x49 | kScanCode_E0_Mask,	// PgUp on arrow keypad
	KEY_LEFT			= 0x4B | kScanCode_E0_Mask,	// LeftArrow on arrow keypad
	KEY_RIGHT			= 0x4D | kScanCode_E0_Mask,	// RightArrow on arrow keypad
	KEY_END				= 0x4F | kScanCode_E0_Mask,	// End on arrow keypad
	KEY_DOWN			= 0x50 | kScanCode_E0_Mask,	// DownArrow on arrow keypad
	KEY_PGDOWN			= 0x51 | kScanCode_E0_Mask,	// PgDn on arrow keypad
	KEY_INSERT			= 0x52 | kScanCode_E0_Mask,	// Insert on arrow keypad
	KEY_DELETE			= 0x53 | kScanCode_E0_Mask,	// Delete on arrow keypad
	KEY_LWIN			= 0x5B | kScanCode_E0_Mask,	// Left Windows key
	KEY_RWIN			= 0x5C | kScanCode_E0_Mask,	// Right Windows key
	KEY_APPS			= 0x5D | kScanCode_E0_Mask,	// AppMenu key
};
