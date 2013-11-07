/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11)

#include "WindowX11.h"
#include "Cursor.h"
#include "Event.h"
#include "UI.h"
#include "WindowX11Data.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/PixelAccessor.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <cstdint>
#include <locale>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace Util {
namespace UI {

static const std::unordered_map<KeySym, Key> & getKeyMapping() {
	static std::unordered_map<KeySym, Key> keyMapping;
	if(keyMapping.empty()) {
		keyMapping[XK_BackSpace] = KEY_BACKSPACE;
		keyMapping[XK_Tab] = KEY_TAB;
		keyMapping[XK_Linefeed] = KEY_LINEFEED;
		keyMapping[XK_Clear] = KEY_CLEAR;
		keyMapping[XK_Return] = KEY_RETURN;
		keyMapping[XK_Pause] = KEY_PAUSE;
		keyMapping[XK_Scroll_Lock] = KEY_SCROLLLOCK;
		keyMapping[XK_Sys_Req] = KEY_SYSREQ;
		keyMapping[XK_Escape] = KEY_ESCAPE;
		keyMapping[XK_Delete] = KEY_DELETE;
		keyMapping[XK_Multi_key] = KEY_MULTIKEY;
		keyMapping[XK_Home] = KEY_HOME;
		keyMapping[XK_Left] = KEY_LEFT;
		keyMapping[XK_Up] = KEY_UP;
		keyMapping[XK_Right] = KEY_RIGHT;
		keyMapping[XK_Down] = KEY_DOWN;
		keyMapping[XK_Page_Up] = KEY_PAGEUP;
		keyMapping[XK_Page_Down] = KEY_PAGEDOWN;
		keyMapping[XK_End] = KEY_END;
		keyMapping[XK_Print] = KEY_PRINT;
		keyMapping[XK_Insert] = KEY_INSERT;
		keyMapping[XK_Undo] = KEY_UNDO;
		keyMapping[XK_Redo] = KEY_REDO;
		keyMapping[XK_Menu] = KEY_MENU;
		keyMapping[XK_Help] = KEY_HELP;
		keyMapping[XK_Break] = KEY_BREAK;
		keyMapping[XK_Mode_switch] = KEY_MODESWITCH;
		keyMapping[XK_Num_Lock] = KEY_NUMLOCK;
		keyMapping[XK_KP_Enter] = KEY_KPENTER;
		keyMapping[XK_KP_Equal] = KEY_KPEQUAL;
		keyMapping[XK_KP_Multiply] = KEY_KPMULTIPLY;
		keyMapping[XK_KP_Add] = KEY_KPADD;
		keyMapping[XK_KP_Separator] = KEY_KPSEPARATOR;
		keyMapping[XK_KP_Subtract] = KEY_KPSUBTRACT;
		keyMapping[XK_KP_Divide] = KEY_KPDIVIDE;
		keyMapping[XK_KP_0] = KEY_KP0;
		keyMapping[XK_KP_Insert] = KEY_KP0;
		keyMapping[XK_KP_1] = KEY_KP1;
		keyMapping[XK_KP_End] = KEY_KP1;
		keyMapping[XK_KP_2] = KEY_KP2;
		keyMapping[XK_KP_Down] = KEY_KP2;
		keyMapping[XK_KP_3] = KEY_KP3;
		keyMapping[XK_KP_Page_Down] = KEY_KP3;
		keyMapping[XK_KP_4] = KEY_KP4;
		keyMapping[XK_KP_Left] = KEY_KP4;
		keyMapping[XK_KP_5] = KEY_KP5;
		keyMapping[XK_KP_Begin] = KEY_KP5;
		keyMapping[XK_KP_6] = KEY_KP6;
		keyMapping[XK_KP_Right] = KEY_KP6;
		keyMapping[XK_KP_7] = KEY_KP7;
		keyMapping[XK_KP_Home] = KEY_KP7;
		keyMapping[XK_KP_8] = KEY_KP8;
		keyMapping[XK_KP_Up] = KEY_KP8;
		keyMapping[XK_KP_9] = KEY_KP9;
		keyMapping[XK_KP_Page_Up] = KEY_KP9;
		keyMapping[XK_F1] = KEY_F1;
		keyMapping[XK_F2] = KEY_F2;
		keyMapping[XK_F3] = KEY_F3;
		keyMapping[XK_F4] = KEY_F4;
		keyMapping[XK_F5] = KEY_F5;
		keyMapping[XK_F6] = KEY_F6;
		keyMapping[XK_F7] = KEY_F7;
		keyMapping[XK_F8] = KEY_F8;
		keyMapping[XK_F9] = KEY_F9;
		keyMapping[XK_F10] = KEY_F10;
		keyMapping[XK_F11] = KEY_F11;
		keyMapping[XK_F12] = KEY_F12;
		keyMapping[XK_F13] = KEY_F13;
		keyMapping[XK_F14] = KEY_F14;
		keyMapping[XK_F15] = KEY_F15;
		keyMapping[XK_Shift_L] = KEY_SHIFTL;
		keyMapping[XK_Shift_R] = KEY_SHIFTR;
		keyMapping[XK_Control_L] = KEY_CONTROLL;
		keyMapping[XK_Control_R] = KEY_CONTROLR;
		keyMapping[XK_Caps_Lock] = KEY_CAPSLOCK;
		keyMapping[XK_Meta_L] = KEY_METAL;
		keyMapping[XK_Meta_R] = KEY_METAR;
		keyMapping[XK_Alt_L] = KEY_ALTL;
		keyMapping[XK_Alt_R] = KEY_ALTR;
		keyMapping[XK_Super_L] = KEY_SUPERL;
		keyMapping[XK_Super_R] = KEY_SUPERR;
		keyMapping[XK_space] = KEY_SPACE;
		keyMapping[XK_exclam] = KEY_EXCLAMATION;
		keyMapping[XK_quotedbl] = KEY_QUOTATION;
		keyMapping[XK_numbersign] = KEY_NUMBERSIGN;
		keyMapping[XK_dollar] = KEY_DOLLAR;
		keyMapping[XK_percent] = KEY_PERCENT;
		keyMapping[XK_ampersand] = KEY_AMPERSAND;
		keyMapping[XK_apostrophe] = KEY_APOSTROPHE;
		keyMapping[XK_parenleft] = KEY_PARENLEFT;
		keyMapping[XK_parenright] = KEY_PARENRIGHT;
		keyMapping[XK_asterisk] = KEY_ASTERISK;
		keyMapping[XK_plus] = KEY_PLUS;
		keyMapping[XK_comma] = KEY_COMMA;
		keyMapping[XK_minus] = KEY_MINUS;
		keyMapping[XK_period] = KEY_PERIOD;
		keyMapping[XK_slash] = KEY_SLASH;
		keyMapping[XK_0] = KEY_0;
		keyMapping[XK_1] = KEY_1;
		keyMapping[XK_2] = KEY_2;
		keyMapping[XK_3] = KEY_3;
		keyMapping[XK_4] = KEY_4;
		keyMapping[XK_5] = KEY_5;
		keyMapping[XK_6] = KEY_6;
		keyMapping[XK_7] = KEY_7;
		keyMapping[XK_8] = KEY_8;
		keyMapping[XK_9] = KEY_9;
		keyMapping[XK_colon] = KEY_COLON;
		keyMapping[XK_semicolon] = KEY_SEMICOLON;
		keyMapping[XK_less] = KEY_LESS;
		keyMapping[XK_equal] = KEY_EQUAL;
		keyMapping[XK_greater] = KEY_GREATER;
		keyMapping[XK_question] = KEY_QUESTION;
		keyMapping[XK_at] = KEY_AT;
		keyMapping[XK_A] = KEY_A;
		keyMapping[XK_a] = KEY_A;
		keyMapping[XK_B] = KEY_B;
		keyMapping[XK_b] = KEY_B;
		keyMapping[XK_C] = KEY_C;
		keyMapping[XK_c] = KEY_C;
		keyMapping[XK_D] = KEY_D;
		keyMapping[XK_d] = KEY_D;
		keyMapping[XK_E] = KEY_E;
		keyMapping[XK_e] = KEY_E;
		keyMapping[XK_F] = KEY_F;
		keyMapping[XK_f] = KEY_F;
		keyMapping[XK_G] = KEY_G;
		keyMapping[XK_g] = KEY_G;
		keyMapping[XK_H] = KEY_H;
		keyMapping[XK_h] = KEY_H;
		keyMapping[XK_I] = KEY_I;
		keyMapping[XK_i] = KEY_I;
		keyMapping[XK_J] = KEY_J;
		keyMapping[XK_j] = KEY_J;
		keyMapping[XK_K] = KEY_K;
		keyMapping[XK_k] = KEY_K;
		keyMapping[XK_L] = KEY_L;
		keyMapping[XK_l] = KEY_L;
		keyMapping[XK_M] = KEY_M;
		keyMapping[XK_m] = KEY_M;
		keyMapping[XK_N] = KEY_N;
		keyMapping[XK_n] = KEY_N;
		keyMapping[XK_O] = KEY_O;
		keyMapping[XK_o] = KEY_O;
		keyMapping[XK_P] = KEY_P;
		keyMapping[XK_p] = KEY_P;
		keyMapping[XK_Q] = KEY_Q;
		keyMapping[XK_q] = KEY_Q;
		keyMapping[XK_R] = KEY_R;
		keyMapping[XK_r] = KEY_R;
		keyMapping[XK_S] = KEY_S;
		keyMapping[XK_s] = KEY_S;
		keyMapping[XK_T] = KEY_T;
		keyMapping[XK_t] = KEY_T;
		keyMapping[XK_U] = KEY_U;
		keyMapping[XK_u] = KEY_U;
		keyMapping[XK_V] = KEY_V;
		keyMapping[XK_v] = KEY_V;
		keyMapping[XK_W] = KEY_W;
		keyMapping[XK_w] = KEY_W;
		keyMapping[XK_X] = KEY_X;
		keyMapping[XK_x] = KEY_X;
		keyMapping[XK_Y] = KEY_Y;
		keyMapping[XK_y] = KEY_Y;
		keyMapping[XK_Z] = KEY_Z;
		keyMapping[XK_z] = KEY_Z;
		keyMapping[XK_bracketleft] = KEY_BRACKETLEFT;
		keyMapping[XK_backslash] = KEY_BACKSLASH;
		keyMapping[XK_bracketright] = KEY_BRACKETRIGHT;
		keyMapping[XK_asciicircum] = KEY_CIRCUMFLEX;
		keyMapping[XK_underscore] = KEY_UNDERSCORE;
		keyMapping[XK_grave] = KEY_GRAVE;
		keyMapping[XK_braceleft] = KEY_BRACELEFT;
		keyMapping[XK_bar] = KEY_BAR;
		keyMapping[XK_braceright] = KEY_BRACERIGHT;
		keyMapping[XK_asciitilde] = KEY_TILDE;
	}
	return keyMapping;
}

static Key keySymToKey(KeySym sym) {
	const auto it = getKeyMapping().find(sym);
	if(it == getKeyMapping().end()) {
		return KEY_NONE;
	}
	return it->second;
}

static ::Cursor convertBitmapToX11Cursor(Display * display, ::Window window, const Reference<Bitmap> & image, unsigned int hotSpotX, unsigned int hotSpotY) {
	if(image == nullptr) {
		throw std::invalid_argument("No bitmap given.");
	}
	if(hotSpotX > image->getWidth() || hotSpotY > image->getHeight()) {
		throw std::invalid_argument("Hot spot is outside of bitmap.");
	}

	auto pa = PixelAccessor::create(image.get());

	unsigned int width_bytes = ((image->getWidth() + 7) & ~7) / 8;

	// Data: Pixels set to 0 get the background color, pixels set to 1 get the foreground color.
	std::vector<uint8_t> data(image->getHeight() * width_bytes, 0);
	// Mask: Pixels set to 0 are ignored, pixels set to 1 are shown.
	std::vector<uint8_t> mask(image->getHeight() * width_bytes, 0);

	unsigned int numFgBits = 0;
	unsigned int sumFgR = 0;
	unsigned int sumFgG = 0;
	unsigned int sumFgB = 0;
	unsigned int numBgBits = 0;
	unsigned int sumBgR = 0;
	unsigned int sumBgG = 0;
	unsigned int sumBgB = 0;
	for (uint_fast32_t y = 0; y < image->getHeight(); ++y) {
		for (uint_fast32_t  x = 0; x < image->getWidth(); ++x) {
			const auto color = pa->readColor4ub(x, y);
			if (color.getA() > 25) {
				mask[y * width_bytes + x / 8] |= (0x01 << (x % 8));
				if ((color.getR() + color.getG() + color.getB()) > 0x40) {
					++numFgBits;
					sumFgR += color.getR();
					sumFgG += color.getG();
					sumFgB += color.getB();
					data[y * width_bytes + x / 8] |= (0x01 << (x % 8));
				} else {
					++numBgBits;
					sumBgR += color.getR();
					sumBgG += color.getG();
					sumBgB += color.getB();
				}
			}
		}
	}

	XColor fgColor;
	if (numFgBits > 0) {
		fgColor.red   = sumFgR * 257 / numFgBits;
		fgColor.green = sumFgG * 257 / numFgBits;
		fgColor.blue  = sumFgB * 257 / numFgBits;
	} else {
		fgColor.red = 0;
		fgColor.green = 0;
		fgColor.blue = 0;
	}

	XColor bgColor;
	if (numBgBits > 0) {
		bgColor.red   = sumBgR * 257 / numBgBits;
		bgColor.green = sumBgG * 257 / numBgBits;
		bgColor.blue  = sumBgB * 257 / numBgBits;
	} else {
		bgColor.red = 0;
		bgColor.green = 0;
		bgColor.blue = 0;
	}

	Pixmap dataPixmap = XCreateBitmapFromData(display, window,
										reinterpret_cast<char *>(data.data()),
										image->getWidth(), image->getHeight());
	Pixmap maskPixmap = XCreateBitmapFromData(display, window,
										reinterpret_cast<char *>(mask.data()),
										image->getWidth(), image->getHeight());
	if (!dataPixmap || !maskPixmap) {
		return None;
	}
	::Cursor x11Cursor = XCreatePixmapCursor(display, dataPixmap, maskPixmap,
											 &fgColor, &bgColor, 
											 hotSpotX, hotSpotY);
	XFreePixmap(display, dataPixmap);
	XFreePixmap(display, maskPixmap);
	return x11Cursor;
}

WindowX11::WindowX11(const Window::Properties & properties) :
		Window(properties), x11Data(new WindowX11Data) {
	setlocale(LC_CTYPE, "");

	x11Data->display = XOpenDisplay(nullptr);

	if (x11Data->display == nullptr) {
		throw std::runtime_error("Failed to open X display.");
	}
}

WindowX11::~WindowX11() = default;

void WindowX11::doSetCursor(UI::Cursor * cursor) {
	if(x11Data->cursor != None) {
		XFreeCursor(x11Data->display, x11Data->cursor);
		x11Data->cursor = None;
	}
	if(cursor == nullptr) {
		XUndefineCursor(x11Data->display, x11Data->window);
	} else {
		x11Data->cursor = convertBitmapToX11Cursor(x11Data->display, x11Data->window, 
												   cursor->getBitmap(), cursor->getHotSpotX(), cursor->getHotSpotY());
		XDefineCursor(x11Data->display, x11Data->window, x11Data->cursor);
	}
}

void WindowX11::doHideCursor() {
	static std::unique_ptr<Cursor> hiddenCursor;
	if(hiddenCursor.get() == nullptr) {
		Reference<Bitmap> bitmap = new Bitmap(1, 1);
		bitmap->data()[0] = 0;
		bitmap->data()[1] = 0;
		bitmap->data()[2] = 0;
		bitmap->data()[3] = 0;
		hiddenCursor.reset(createCursor(bitmap, 0, 0));
	}
	doSetCursor(hiddenCursor.get());
}

void WindowX11::warpCursor(int x, int y) {
	XWarpPointer(x11Data->display, None, x11Data->window, 0, 0, 0, 0, x, y);
	XSync(x11Data->display, False);
}

void WindowX11::grabInput() {
	XGrabPointer(x11Data->display, x11Data->window, 
				 False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, 
				 GrabModeAsync, GrabModeAsync, 
				 x11Data->window, None, CurrentTime);
	XGrabKeyboard(x11Data->display, x11Data->window, 
				  False, 
				  GrabModeAsync, GrabModeAsync, 
				  CurrentTime);
}

void WindowX11::ungrabInput() {
	XUngrabKeyboard(x11Data->display, CurrentTime);
	XUngrabPointer(x11Data->display, CurrentTime);
}

std::deque<Event> WindowX11::fetchEvents() {
	static int lastX = 0;
	static int lastY = 0;
	std::deque<Event> events;
	while (XPending(x11Data->display) > 0) {
		XEvent xev;

		XNextEvent(x11Data->display, &xev);

		Event event;
		switch (xev.type) {
			case DestroyNotify:
				event.type = EVENT_QUIT;
				break;
			case ClientMessage:
				if (xev.xclient.data.l[0] == static_cast<long> (x11Data->deleteMessage)) {
					event.type = EVENT_QUIT;
				} else {
					continue;
				}
				break;
			case ConfigureNotify: {
				const uint32_t xWidth = static_cast<uint32_t>(xev.xconfigure.width);
				const uint32_t xHeight = static_cast<uint32_t>(xev.xconfigure.height);
				if (xWidth != width || xHeight != height) {
					event.type = EVENT_RESIZE;
					event.resize.width = xWidth;
					event.resize.height = xHeight;
					width = xWidth;
					height = xHeight;
				} else {
					continue;
				}
				break;
			}
			case KeyPress: {
				event.type = EVENT_KEYBOARD;

				const int maxLength = sizeof(event.keyboard.str);
#ifdef X_HAVE_UTF8_STRING
				const int length = Xutf8LookupString(x11Data->inputContext, &(xev.xkey), event.keyboard.str, maxLength, nullptr, nullptr);
#else
				const int length = XmbLookupString(x11Data->inputContext, &(xev.xkey), event.keyboard.str, maxLength, nullptr, nullptr);
#endif
				for(int i = length; i < maxLength; ++i) {
					event.keyboard.str[i] = '\0';
				}

				event.keyboard.key = keySymToKey(XLookupKeysym(&(xev.xkey), 0));
				event.keyboard.pressed = true;
				break;
			}
			case KeyRelease:
				// Handle key auto repeat
				if (XPending(x11Data->display) > 0) {
					XEvent nextEvent;
					XPeekEvent(xev.xkey.display, &nextEvent);
					if ((nextEvent.type == KeyPress) && (nextEvent.xkey.keycode == xev.xkey.keycode) && (nextEvent.xkey.time == xev.xkey.time)) {
						// Ignore the KeyRelease and remove the KeyPress from the queue
						XNextEvent(xev.xkey.display, &nextEvent);
						continue;
					}
				}
				event.type = EVENT_KEYBOARD;
				for(size_t i = 0; i < sizeof(event.keyboard.str); ++i) {
					event.keyboard.str[i] = '\0';
				}
				event.keyboard.key = keySymToKey(XLookupKeysym(&(xev.xkey), 0));
				event.keyboard.pressed = false;
				break;
			case ButtonPress:
			case ButtonRelease:
				event.type = EVENT_MOUSE_BUTTON;
				event.button.x = xev.xbutton.x;
				event.button.y = xev.xbutton.y;
				switch(xev.xbutton.button) {
					case Button1:
						event.button.button = MOUSE_BUTTON_LEFT;
						break;
					case Button2:
						event.button.button = MOUSE_BUTTON_MIDDLE;
						break;
					case Button3:
						event.button.button = MOUSE_BUTTON_RIGHT;
						break;
					case Button4:
						event.button.button = MOUSE_WHEEL_UP;
						break;
					case Button5:
						event.button.button = MOUSE_WHEEL_DOWN;
						break;
					default:
						event.button.button = MOUSE_BUTTON_OTHER;
						break;
				}
				event.button.pressed = (xev.type == ButtonPress);
				lastX = xev.xbutton.x;
				lastY = xev.xbutton.y;
				break;
			case MotionNotify:
				event.type = EVENT_MOUSE_MOTION;
				event.motion.x = xev.xmotion.x;
				event.motion.y = xev.xmotion.y;
				event.motion.buttonMask = MASK_NO_BUTTON;
				if(xev.xmotion.state & Button1Mask) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_LEFT;
				}
				if(xev.xmotion.state & Button2Mask) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_MIDDLE;
				}
				if(xev.xmotion.state & Button3Mask) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_RIGHT;
				}
				event.motion.deltaX = xev.xmotion.x - lastX;
				event.motion.deltaY = xev.xmotion.y - lastY;
				lastX = xev.xmotion.x;
				lastY = xev.xmotion.y;
				break;
			default:
				continue;
		}
		events.emplace_back(event);
	}
	return events;
}

void WindowX11::setIcon(const Bitmap & icon) {
	x11Data->setWindowIcon(icon);
}

std::string WindowX11::getClipboardText() const {
	return x11Data->clipboard;
}

void WindowX11::setClipboardText(const std::string & text) {
	x11Data->clipboard = text;
}

}
}

#endif /* defined(UTIL_HAVE_LIB_X11) */
