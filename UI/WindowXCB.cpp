/*
	This file is part of the Util library.
	Copyright (C) 2014 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_XCB) and defined(UTIL_HAVE_LIB_XCB_KEYSYMS) and defined(UTIL_HAVE_LIB_EGL)

#include "WindowXCB.h"
#include "../StringUtils.h"

#include <EGL/egl.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <X11/keysymdef.h>

#include <unordered_map>

namespace Util {
namespace UI {

static const std::unordered_map<xcb_keysym_t, Key> & getKeyMapping() {
	static std::unordered_map<xcb_keysym_t, Key> keyMapping;
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

static Key keySymToKey(xcb_keysym_t sym) {
	const auto it = getKeyMapping().find(sym);
	if(it == getKeyMapping().end()) {
		return KEY_NONE;
	}
	return it->second;
}

struct WindowXCB::WindowEGLData {
	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;

	WindowEGLData() :
		display(EGL_NO_DISPLAY), context(EGL_NO_CONTEXT), surface(EGL_NO_SURFACE) {
	}

	~WindowEGLData() {
		if (display != EGL_NO_DISPLAY) {
			eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			if (surface != EGL_NO_SURFACE) {
				eglDestroySurface(display, surface);
			}
			if (context != EGL_NO_CONTEXT) {
				eglDestroyContext(display, context);
			}
			eglTerminate(display);
		}
	}
};

struct WindowXCB::WindowXCBData {
	xcb_connection_t * connection;
	xcb_window_t window;
	xcb_atom_t deleteAtom;
	xcb_key_symbols_t * keySymbols;

	WindowXCBData() :
		connection(nullptr), window(0), deleteAtom(0), keySymbols(nullptr) {
	}

	~WindowXCBData() {
		if (nullptr != keySymbols) {
			xcb_key_symbols_free(keySymbols);
		}
		if (0 != window) {
			xcb_destroy_window(connection, window);
		}
		if (connection != nullptr) {
			xcb_disconnect(connection);
		}
	}
};

WindowXCB::WindowXCB(const Window::Properties & properties) :
		Window(properties), 
		xcbData(new WindowXCBData), 
		eglData(new WindowEGLData) {
	xcbData->connection = xcb_connect(nullptr, nullptr);

	eglData->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (EGL_NO_DISPLAY == eglData->display) {
		throw std::runtime_error("Failed to open display.");
	}

	EGLint versionMajor;
	EGLint versionMinor;
	if (EGL_FALSE == eglInitialize(eglData->display, &versionMajor, &versionMinor)) {
		throw std::runtime_error("Failed to initialize display.");
	}

	// EGL version 1.4 is needed for EGL_OPENGL_BIT
	if ((1 > versionMajor) || ((1 == versionMajor) && (4 > versionMinor))) {
		throw std::runtime_error("EGL version less than 1.4 detected.");
	}
	
	if (EGL_TRUE != eglBindAPI(EGL_OPENGL_API)) {
		throw std::runtime_error("EGL cannot bind to the OpenGL API.");
	}

	// Define attributes of desired framebuffer configurations
	const EGLint fbAttribs[] = { 
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER, 
		EGL_BUFFER_SIZE, 24, 
		EGL_RED_SIZE, 8, 
		EGL_GREEN_SIZE, 8, 
		EGL_BLUE_SIZE, 8, 
		EGL_DEPTH_SIZE, 8,
		EGL_CONFIG_CAVEAT, EGL_NONE,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT, 
		EGL_NONE
	};
	// Request a single framebuffer configuration
	EGLConfig fbConfig;
	EGLint fbCount;
	if (EGL_TRUE != eglChooseConfig(eglData->display, fbAttribs, &fbConfig, 1, &fbCount)) {
		throw std::runtime_error("Failed to retrieve a matching framebuffer configuration.");
	}

	EGLint visualID;
	eglGetConfigAttrib(eglData->display, fbConfig, EGL_NATIVE_VISUAL_ID, &visualID);

	const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	eglData->context = eglCreateContext(eglData->display, fbConfig, EGL_NO_CONTEXT, contextAttribs);
	if (EGL_NO_CONTEXT == eglData->context) {
		throw std::runtime_error("Failed to create OpenGL ES 2.x context. " + 
								 StringUtils::toString(eglGetError()));
	}

	// Create XCB window
	const xcb_setup_t * setup = xcb_get_setup(xcbData->connection);
	xcb_screen_t * screen = xcb_setup_roots_iterator(setup).data;

	xcbData->window = xcb_generate_id(xcbData->connection);
	const uint32_t valueList[1] = { 
		XCB_EVENT_MASK_STRUCTURE_NOTIFY | 
		XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | 
		XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | 
		XCB_EVENT_MASK_POINTER_MOTION
	};
	xcb_create_window(xcbData->connection,
					  XCB_COPY_FROM_PARENT,
					  xcbData->window,
					  screen->root,
					  0, 0,
					  properties.clientAreaWidth, properties.clientAreaHeight,
					  0,
					  XCB_WINDOW_CLASS_INPUT_OUTPUT,
					  screen->root_visual,
					  XCB_CW_EVENT_MASK,
					  valueList);

	xcbData->keySymbols = xcb_key_symbols_alloc(xcbData->connection);

	// Set window title
	xcb_change_property(xcbData->connection,
						XCB_PROP_MODE_REPLACE,
						xcbData->window,
						XCB_ATOM_WM_NAME,
						XCB_ATOM_STRING,
						8,
						properties.title.size(),
						properties.title.c_str());

	// The window should receive a message when the close/"X" button is clicked.
	{
		const std::string WM_PROTOCOLS("WM_PROTOCOLS");
		const std::string WM_DELETE_WINDOW("WM_DELETE_WINDOW");
		auto cookie = xcb_intern_atom(xcbData->connection,
									  1,
									  WM_PROTOCOLS.size(),
									  WM_PROTOCOLS.c_str());
		auto cookie2 = xcb_intern_atom(xcbData->connection,
									   1,
									   WM_DELETE_WINDOW.size(),
									   WM_DELETE_WINDOW.c_str());
		auto reply = xcb_intern_atom_reply(xcbData->connection, cookie, nullptr);
		auto reply2 = xcb_intern_atom_reply(xcbData->connection, cookie2, nullptr);
		xcbData->deleteAtom = reply2->atom;
		xcb_change_property(xcbData->connection, 
							XCB_PROP_MODE_REPLACE,
							xcbData->window,
							reply->atom,
							XCB_ATOM_ATOM,
							32, 
							1, 
							&xcbData->deleteAtom);
	}

	/*
	 TODO implement
	if (properties.borderless) {
		x11Data->removeWindowBorder();
	}
	if (!properties.resizable) {
		x11Data->fixWindowSize(static_cast<int>(properties.clientAreaWidth), 
							   static_cast<int>(properties.clientAreaHeight));
	}

	x11Data->inputMethod = XOpenIM(x11Data->display, nullptr, nullptr, nullptr);
	if (x11Data->inputMethod == nullptr) {
		throw std::runtime_error("Failed to create input method.");
	}

	x11Data->inputContext = XCreateIC(x11Data->inputMethod,
								   XNInputStyle, XIMPreeditNone | XIMStatusNone,
								   XNClientWindow, x11Data->window,
								   XNFocusWindow, x11Data->window,
								   nullptr);
	if (x11Data->inputContext == nullptr) {
		throw std::runtime_error("Failed to create input context.");
	}*/
	xcb_map_window(xcbData->connection, xcbData->window);

	if (properties.positioned) {
		const static uint32_t pos[] = { 
			static_cast<uint32_t>(properties.posX), 
			static_cast<uint32_t>(properties.posY)
		};
		xcb_configure_window(xcbData->connection,
							 xcbData->window,
							 XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, 
							 pos);
	}

	xcb_flush(xcbData->connection);

	eglData->surface = eglCreateWindowSurface(eglData->display, fbConfig, xcbData->window, nullptr);
	if (EGL_NO_SURFACE == eglData->surface) {
		throw std::runtime_error("Failed to create window surface.");
	}

	eglMakeCurrent(eglData->display, eglData->surface, eglData->surface, eglData->context);
}

WindowXCB::~WindowXCB() = default;

void WindowXCB::swapBuffers() {
	eglSwapBuffers(eglData->display, eglData->surface);
}

void WindowXCB::doSetCursor(const Cursor * cursor) {
	// TODO implement
}

void WindowXCB::doHideCursor() {
	// TODO implement
}

void WindowXCB::warpCursor(int x, int y) {
	xcb_warp_pointer(xcbData->connection, XCB_NONE, xcbData->window, 0, 0, 0, 0, x, y);
	xcb_flush(xcbData->connection);
}

void WindowXCB::grabInput() {
	xcb_grab_pointer(xcbData->connection, 
					 1,
					 xcbData->window, 
					 XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION,
					 XCB_GRAB_MODE_ASYNC, 
					 XCB_GRAB_MODE_ASYNC, 
					 xcbData->window, 
					 XCB_NONE,
					 XCB_CURRENT_TIME);
	xcb_grab_keyboard(xcbData->connection, 
					  1,
					  xcbData->window, 
					  XCB_GRAB_MODE_ASYNC,
					  XCB_GRAB_MODE_ASYNC, 
					  XCB_CURRENT_TIME);
}

void WindowXCB::ungrabInput() {
	xcb_ungrab_keyboard(xcbData->connection, XCB_CURRENT_TIME);
	xcb_ungrab_pointer(xcbData->connection, XCB_CURRENT_TIME);
}

std::deque<Event> WindowXCB::fetchEvents() {
	static int lastX = 0;
	static int lastY = 0;
	std::deque<Event> events;
	xcb_generic_event_t * xcbEvent;
    while ((xcbEvent = xcb_poll_for_event(xcbData->connection))) {
		Event event;
		const auto eventType = xcbEvent->response_type & ~0x80;
		switch (eventType) {
			case XCB_DESTROY_NOTIFY:
				event.type = EVENT_QUIT;
				break;
			case XCB_CLIENT_MESSAGE:
			{
				auto clientMsg = reinterpret_cast<const xcb_client_message_event_t *>(xcbEvent);
				if (clientMsg->data.data32[0] == xcbData->deleteAtom) {
					event.type = EVENT_QUIT;
				} else {
					continue;
				}
				break;
			}
			case XCB_CONFIGURE_NOTIFY:
			{
				auto cfgNotify = reinterpret_cast<const xcb_configure_notify_event_t *>(xcbEvent);
				const uint32_t xWidth = static_cast<uint32_t>(cfgNotify->width);
				const uint32_t xHeight = static_cast<uint32_t>(cfgNotify->height);
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
			case XCB_KEY_PRESS:
			{
				auto key = reinterpret_cast<const xcb_key_press_event_t *>(xcbEvent);
				event.type = EVENT_KEYBOARD;

				// FIXME Extract string from key event
				for(size_t i = 0; i < sizeof(event.keyboard.str); ++i) {
					event.keyboard.str[i] = '\0';
				}

				int col = key->state & XCB_MOD_MASK_SHIFT ? 1 : 0;
				const int altGrOffset = 4;
				if (key->state & 128) {
					col += altGrOffset;
				}
				event.keyboard.key = keySymToKey(xcb_key_symbols_get_keysym(xcbData->keySymbols, key->detail, col));
				event.keyboard.pressed = true;
				break;
			}
			case XCB_KEY_RELEASE:
			{
				auto key = reinterpret_cast<const xcb_key_release_event_t *>(xcbEvent);
				// FIXME Handle key auto repeat
				event.type = EVENT_KEYBOARD;

				for(size_t i = 0; i < sizeof(event.keyboard.str); ++i) {
					event.keyboard.str[i] = '\0';
				}

				int col = key->state & XCB_MOD_MASK_SHIFT ? 1 : 0;
				const int altGrOffset = 4;
				if (key->state & 128) {
					col += altGrOffset;
				}
				event.keyboard.key = keySymToKey(xcb_key_symbols_get_keysym(xcbData->keySymbols, key->detail, col));
				event.keyboard.pressed = false;
				break;
			}
			case XCB_BUTTON_PRESS:
			case XCB_BUTTON_RELEASE:
			{
				auto button = reinterpret_cast<const xcb_button_press_event_t *>(xcbEvent);
				event.type = EVENT_MOUSE_BUTTON;
				event.button.x = button->event_x;
				event.button.y = button->event_y;
				switch(button->detail) {
					case 1:
						event.button.button = MOUSE_BUTTON_LEFT;
						break;
					case 2:
						event.button.button = MOUSE_BUTTON_MIDDLE;
						break;
					case 3:
						event.button.button = MOUSE_BUTTON_RIGHT;
						break;
					case 4:
						event.button.button = MOUSE_WHEEL_UP;
						break;
					case 5:
						event.button.button = MOUSE_WHEEL_DOWN;
						break;
					default:
						event.button.button = MOUSE_BUTTON_OTHER;
						break;
				}
				event.button.pressed = (eventType == XCB_BUTTON_PRESS);
				lastX = button->event_x;
				lastY = button->event_y;
				break;
			}
			case XCB_MOTION_NOTIFY:
			{
				auto motion = reinterpret_cast<const xcb_motion_notify_event_t *>(xcbEvent);
				event.type = EVENT_MOUSE_MOTION;
				event.motion.x = motion->event_x;
				event.motion.y = motion->event_y;
				event.motion.buttonMask = MASK_NO_BUTTON;
				if(motion->state & XCB_BUTTON_MASK_1) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_LEFT;
				}
				if(motion->state & XCB_BUTTON_MASK_2) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_MIDDLE;
				}
				if(motion->state & XCB_BUTTON_MASK_3) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_RIGHT;
				}
				event.motion.deltaX = motion->event_x - lastX;
				event.motion.deltaY = motion->event_y - lastY;
				lastX = motion->event_x;
				lastY = motion->event_y;
				break;
			}
			default:
				continue;
		}
		events.emplace_back(event);
		free(xcbEvent);
	}
	return events;
}

void WindowXCB::setIcon(const Bitmap & icon) {
	// TODO implement
}

std::string WindowXCB::getClipboardText() const {
	return clipboard;
}

void WindowXCB::setClipboardText(const std::string & text) {
	clipboard = text;
}

}
}

#endif /* defined(UTIL_HAVE_LIB_XCB) and defined(UTIL_HAVE_LIB_XCB_KEYSYMS) and defined(UTIL_HAVE_LIB_EGL) */
