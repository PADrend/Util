/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef EVENT_H_
#define EVENT_H_

#include <cstdint>
#include <functional>

namespace Util {
namespace UI {

enum EventType_t {
	EVENT_QUIT,
	EVENT_RESIZE,
	EVENT_KEYBOARD,
	EVENT_MOUSE_BUTTON,
	EVENT_MOUSE_MOTION,
	EVENT_JOY_AXIS,
	EVENT_JOY_BUTTON,
	EVENT_JOY_HAT
};

struct QuitEvent {
	EventType_t type;
};

struct ResizeEvent {
	EventType_t type;
	uint32_t width;
	uint32_t height;
	float contentScale;
};

enum Key : uint8_t {
	KEY_NONE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_LINEFEED,
	KEY_CLEAR,
	KEY_RETURN,
	KEY_PAUSE,
	KEY_SCROLLLOCK,
	KEY_SYSREQ,
	KEY_ESCAPE,
	KEY_DELETE,
	KEY_MULTIKEY,
	KEY_HOME,
	KEY_LEFT,
	KEY_UP,
	KEY_RIGHT,
	KEY_DOWN,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_END,
	KEY_PRINT,
	KEY_INSERT,
	KEY_UNDO,
	KEY_REDO,
	KEY_MENU,
	KEY_HELP,
	KEY_BREAK,
	KEY_MODESWITCH,
	KEY_NUMLOCK,
	KEY_KPENTER,
	KEY_KPEQUAL,
	KEY_KPMULTIPLY,
	KEY_KPADD,
	KEY_KPSEPARATOR,
	KEY_KPSUBTRACT,
	KEY_KPDIVIDE,
	KEY_KP0,
	KEY_KP1,
	KEY_KP2,
	KEY_KP3,
	KEY_KP4,
	KEY_KP5,
	KEY_KP6,
	KEY_KP7,
	KEY_KP8,
	KEY_KP9,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
	KEY_SHIFTL,
	KEY_SHIFTR,
	KEY_CONTROLL,
	KEY_CONTROLR,
	KEY_METAL,
	KEY_METAR,
	KEY_ALTL,
	KEY_ALTR,
	KEY_SUPERL,
	KEY_SUPERR,
	KEY_CAPSLOCK,
	KEY_SPACE,
	KEY_EXCLAMATION,
	KEY_QUOTATION,
	KEY_NUMBERSIGN,
	KEY_DOLLAR,
	KEY_PERCENT,
	KEY_AMPERSAND,
	KEY_APOSTROPHE,
	KEY_PARENLEFT,
	KEY_PARENRIGHT,
	KEY_ASTERISK,
	KEY_PLUS,
	KEY_COMMA,
	KEY_MINUS,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_COLON,
	KEY_SEMICOLON,
	KEY_LESS,
	KEY_EQUAL,
	KEY_GREATER,
	KEY_QUESTION,
	KEY_AT,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_BRACKETLEFT,
	KEY_BACKSLASH,
	KEY_BRACKETRIGHT,
	KEY_CIRCUMFLEX,
	KEY_UNDERSCORE,
	KEY_GRAVE,
	KEY_BRACELEFT,
	KEY_BAR,
	KEY_BRACERIGHT,
	KEY_TILDE,
	KEY_EUROSIGN
};
struct KeyboardEvent {
	EventType_t type;
	/**
	 * Multi-byte character representing the key event.
	 * UTF-8 encoded characters consist of at most four bytes.
	 * Unused entries are null.
	 */
	char str[4];
	Key key;
	bool pressed;
};

enum Button : uint8_t {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT,
	MOUSE_WHEEL_UP,
	MOUSE_WHEEL_DOWN,
	MOUSE_BUTTON_OTHER
};
struct ButtonEvent {
	EventType_t type;
	uint16_t x;
	uint16_t y;
	Button button;
	bool pressed;
};

const uint8_t MASK_NO_BUTTON = 0;
const uint8_t MASK_MOUSE_BUTTON_LEFT = 1 << 0;
const uint8_t MASK_MOUSE_BUTTON_MIDDLE = 1 << 1;
const uint8_t MASK_MOUSE_BUTTON_RIGHT = 1 << 2;
struct MotionEvent {
	EventType_t type;
	uint16_t x;
	uint16_t y;
	uint8_t buttonMask;
	int16_t deltaX;
	int16_t deltaY;
};

struct JoyAxisEvent {
	EventType_t type;
	uint8_t joystick;
	uint8_t axis;
	int16_t value;
};

struct JoyButtonEvent {
	EventType_t type;
	uint8_t joystick;
	uint8_t button;
	bool pressed;
};

const uint8_t MASK_HAT_CENTER = 0;
const uint8_t MASK_HAT_UP = 1 << 0;
const uint8_t MASK_HAT_RIGHT = 1 << 1;
const uint8_t MASK_HAT_DOWN = 1 << 2;
const uint8_t MASK_HAT_LEFT = 1 << 3;
struct JoyHatEvent {
	EventType_t type;
	uint8_t joystick;
	uint8_t hat;
	uint8_t value;
};

union Event {
	EventType_t type;
	QuitEvent quit;
	ResizeEvent resize;
	KeyboardEvent keyboard;
	ButtonEvent button;
	MotionEvent motion;
	JoyAxisEvent joyAxis;
	JoyButtonEvent joyButton;
	JoyHatEvent joyHat;
};

}
}

// Allow Key to be stored in unordered containers
namespace std {
template <> struct hash<Util::UI::Key> {
	size_t operator()(Util::UI::Key value) const noexcept {
		return static_cast<size_t>(value);
	}
};
}

#endif /* EVENT_H_ */
