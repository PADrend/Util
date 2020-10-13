/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_SDL2)

#include "WindowSDL.h"
#include "Event.h"
#include "UI.h"
#include "../StringUtils.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/BitmapUtils.h"
#include "../Graphics/PixelAccessor.h"

COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL.h>
#include <SDL_vulkan.h>
//#include <vulkan/vulkan.hpp>

// this is needed for setting the windows icon to the applications default icon ("IDI_MAIN_ICON").
#if defined(_WIN32)
#include <windows.h>
#include <SDL_syswm.h>
#endif

COMPILER_WARN_POP

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <iostream>
#include <sstream>
#include "../LibRegistry.h"

/*#if defined(__GNUC__)
#define DLLEXPORT __attribute__((dllexport))
#elif defined(_MSC_VER)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif*/
#define DLLEXPORT

#if defined(_WIN32)
extern "C" {
	// Tells the driver to prefer dedicated GPU instead of integrated when available
	DLLEXPORT unsigned long NvOptimusEnablement = 0x00000001;
	DLLEXPORT int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

static bool libNameInitailized = [](){	
	SDL_version version;
	SDL_GetVersion(&version);
	std::ostringstream s;
	s << "SDL " << static_cast<int>(version.major) << "." << static_cast<int>(version.minor) << "." << static_cast<int>(version.patch)<< " (www.libsdl.org)";
	Util::LibRegistry::registerLibVersionString("LibSDL2",s.str()); 
	return true;
}();

namespace Util {
namespace UI {

static const std::unordered_map<SDL_Keycode, Key> & getKeyMapping() {
	static std::unordered_map<SDL_Keycode, Key> keyMapping;
	if(keyMapping.empty()) {
		keyMapping[SDLK_BACKSPACE] = KEY_BACKSPACE;
		keyMapping[SDLK_TAB] = KEY_TAB;
		keyMapping[SDLK_CLEAR] = KEY_CLEAR;
		keyMapping[SDLK_RETURN] = KEY_RETURN;
		keyMapping[SDLK_PAUSE] = KEY_PAUSE;
		keyMapping[SDLK_ESCAPE] = KEY_ESCAPE;
		keyMapping[SDLK_SPACE] = KEY_SPACE;
		keyMapping[SDLK_EXCLAIM] = KEY_EXCLAMATION;
		keyMapping[SDLK_QUOTEDBL] = KEY_QUOTATION;
		keyMapping[SDLK_HASH] = KEY_NUMBERSIGN;
		keyMapping[SDLK_DOLLAR] = KEY_DOLLAR;
		keyMapping[SDLK_AMPERSAND] = KEY_AMPERSAND;
		keyMapping[SDLK_QUOTE] = KEY_APOSTROPHE;
		keyMapping[SDLK_LEFTPAREN] = KEY_PARENLEFT;
		keyMapping[SDLK_RIGHTPAREN] = KEY_PARENRIGHT;
		keyMapping[SDLK_ASTERISK] = KEY_ASTERISK;
		keyMapping[SDLK_PLUS] = KEY_PLUS;
		keyMapping[SDLK_COMMA] = KEY_COMMA;
		keyMapping[SDLK_MINUS] = KEY_MINUS;
		keyMapping[SDLK_PERIOD] = KEY_PERIOD;
		keyMapping[SDLK_SLASH] = KEY_SLASH;
		keyMapping[SDLK_0] = KEY_0;
		keyMapping[SDLK_1] = KEY_1;
		keyMapping[SDLK_2] = KEY_2;
		keyMapping[SDLK_3] = KEY_3;
		keyMapping[SDLK_4] = KEY_4;
		keyMapping[SDLK_5] = KEY_5;
		keyMapping[SDLK_6] = KEY_6;
		keyMapping[SDLK_7] = KEY_7;
		keyMapping[SDLK_8] = KEY_8;
		keyMapping[SDLK_9] = KEY_9;
		keyMapping[SDLK_COLON] = KEY_COLON;
		keyMapping[SDLK_SEMICOLON] = KEY_SEMICOLON;
		keyMapping[SDLK_LESS] = KEY_LESS;
		keyMapping[SDLK_EQUALS] = KEY_EQUAL;
		keyMapping[SDLK_GREATER] = KEY_GREATER;
		keyMapping[SDLK_QUESTION] = KEY_QUESTION;
		keyMapping[SDLK_AT] = KEY_AT;
		keyMapping[SDLK_LEFTBRACKET] = KEY_BRACKETLEFT;
		keyMapping[SDLK_BACKSLASH] = KEY_BACKSLASH;
		keyMapping[SDLK_RIGHTBRACKET] = KEY_BRACKETRIGHT;
		keyMapping[SDLK_CARET] = KEY_CIRCUMFLEX;
		keyMapping[SDLK_UNDERSCORE] = KEY_UNDERSCORE;
		keyMapping[SDLK_BACKQUOTE] = KEY_GRAVE;
		keyMapping[SDLK_a] = KEY_A;
		keyMapping[SDLK_b] = KEY_B;
		keyMapping[SDLK_c] = KEY_C;
		keyMapping[SDLK_d] = KEY_D;
		keyMapping[SDLK_e] = KEY_E;
		keyMapping[SDLK_f] = KEY_F;
		keyMapping[SDLK_g] = KEY_G;
		keyMapping[SDLK_h] = KEY_H;
		keyMapping[SDLK_i] = KEY_I;
		keyMapping[SDLK_j] = KEY_J;
		keyMapping[SDLK_k] = KEY_K;
		keyMapping[SDLK_l] = KEY_L;
		keyMapping[SDLK_m] = KEY_M;
		keyMapping[SDLK_n] = KEY_N;
		keyMapping[SDLK_o] = KEY_O;
		keyMapping[SDLK_p] = KEY_P;
		keyMapping[SDLK_q] = KEY_Q;
		keyMapping[SDLK_r] = KEY_R;
		keyMapping[SDLK_s] = KEY_S;
		keyMapping[SDLK_t] = KEY_T;
		keyMapping[SDLK_u] = KEY_U;
		keyMapping[SDLK_v] = KEY_V;
		keyMapping[SDLK_w] = KEY_W;
		keyMapping[SDLK_x] = KEY_X;
		keyMapping[SDLK_y] = KEY_Y;
		keyMapping[SDLK_z] = KEY_Z;
		keyMapping[SDLK_DELETE] = KEY_DELETE;
		keyMapping[SDLK_KP_0] = KEY_KP0;
		keyMapping[SDLK_KP_1] = KEY_KP1;
		keyMapping[SDLK_KP_2] = KEY_KP2;
		keyMapping[SDLK_KP_3] = KEY_KP3;
		keyMapping[SDLK_KP_4] = KEY_KP4;
		keyMapping[SDLK_KP_5] = KEY_KP5;
		keyMapping[SDLK_KP_6] = KEY_KP6;
		keyMapping[SDLK_KP_7] = KEY_KP7;
		keyMapping[SDLK_KP_8] = KEY_KP8;
		keyMapping[SDLK_KP_9] = KEY_KP9;
		keyMapping[SDLK_KP_PERIOD] = KEY_KPSEPARATOR;
		keyMapping[SDLK_KP_DIVIDE] = KEY_KPDIVIDE;
		keyMapping[SDLK_KP_MULTIPLY] = KEY_KPMULTIPLY;
		keyMapping[SDLK_KP_MINUS] = KEY_KPSUBTRACT;
		keyMapping[SDLK_KP_PLUS] = KEY_KPADD;
		keyMapping[SDLK_KP_ENTER] = KEY_KPENTER;
		keyMapping[SDLK_KP_EQUALS] = KEY_KPEQUAL;
		keyMapping[SDLK_UP] = KEY_UP;
		keyMapping[SDLK_DOWN] = KEY_DOWN;
		keyMapping[SDLK_RIGHT] = KEY_RIGHT;
		keyMapping[SDLK_LEFT] = KEY_LEFT;
		keyMapping[SDLK_INSERT] = KEY_INSERT;
		keyMapping[SDLK_HOME] = KEY_HOME;
		keyMapping[SDLK_END] = KEY_END;
		keyMapping[SDLK_PAGEUP] = KEY_PAGEUP;
		keyMapping[SDLK_PAGEDOWN] = KEY_PAGEDOWN;
		keyMapping[SDLK_F1] = KEY_F1;
		keyMapping[SDLK_F2] = KEY_F2;
		keyMapping[SDLK_F3] = KEY_F3;
		keyMapping[SDLK_F4] = KEY_F4;
		keyMapping[SDLK_F5] = KEY_F5;
		keyMapping[SDLK_F6] = KEY_F6;
		keyMapping[SDLK_F7] = KEY_F7;
		keyMapping[SDLK_F8] = KEY_F8;
		keyMapping[SDLK_F9] = KEY_F9;
		keyMapping[SDLK_F10] = KEY_F10;
		keyMapping[SDLK_F11] = KEY_F11;
		keyMapping[SDLK_F12] = KEY_F12;
		keyMapping[SDLK_F13] = KEY_F13;
		keyMapping[SDLK_F14] = KEY_F14;
		keyMapping[SDLK_F15] = KEY_F15;
		keyMapping[SDLK_NUMLOCKCLEAR] = KEY_NUMLOCK;
		keyMapping[SDLK_CAPSLOCK] = KEY_CAPSLOCK;
		keyMapping[SDLK_SCROLLLOCK] = KEY_SCROLLLOCK;
		keyMapping[SDLK_RSHIFT] = KEY_SHIFTR;
		keyMapping[SDLK_LSHIFT] = KEY_SHIFTL;
		keyMapping[SDLK_RCTRL] = KEY_CONTROLR;
		keyMapping[SDLK_LCTRL] = KEY_CONTROLL;
		keyMapping[SDLK_RALT] = KEY_ALTR;
		keyMapping[SDLK_LALT] = KEY_ALTL;
		keyMapping[SDLK_RGUI] = KEY_METAR;
		keyMapping[SDLK_LGUI] = KEY_METAL;
		keyMapping[SDLK_MODE] = KEY_MODESWITCH;
		keyMapping[SDLK_APPLICATION] = KEY_MULTIKEY;
		keyMapping[SDLK_HELP] = KEY_HELP;
		keyMapping[SDLK_PRINTSCREEN] = KEY_PRINT;
		keyMapping[SDLK_SYSREQ] = KEY_SYSREQ;
		keyMapping[SDLK_MENU] = KEY_MENU;
		keyMapping[SDLK_UNDO] = KEY_UNDO;
	}
	return keyMapping;
}

static Key keySymToKey(SDL_Keycode sym) {
	const auto it = getKeyMapping().find(sym);
	if(it == getKeyMapping().end()) {
		return KEY_NONE;
	}
	return it->second;
}

static SDL_Cursor * convertBitmapToSDLCursor(const Reference<Bitmap> & image, unsigned int hotSpotX, unsigned int hotSpotY) {
	if(image == nullptr) {
		throw std::invalid_argument("No bitmap given.");
	}
	if(hotSpotX > image->getWidth() || hotSpotY > image->getHeight()) {
		throw std::invalid_argument("Hot spot is outside of bitmap.");
	}

	auto pa = PixelAccessor::create(image.get());

	std::vector<uint8_t> data;
	data.reserve(pa->getWidth() * pa->getHeight());
	std::vector<uint8_t> mask;
	mask.reserve(pa->getWidth() * pa->getHeight());

	for(uint32_t y = 0; y < pa->getHeight(); ++y) {
		for(uint32_t x = 0; x < pa->getWidth(); ++x) {
			if(x % 8) {
				data.back() <<= 1;
				mask.back() <<= 1;
			} else {
				data.emplace_back(0x00);
				mask.emplace_back(0x00);
			}

			const auto color = pa->readColor4ub(x, y);
			if(color.getA() > 127) {
				mask.back() |= 0x01;
				if(color.getR() + color.getG() + color.getB() < (3 * 127)) {
					data.back() |= 0x01;
				} 
			}
		}
	}

	return SDL_CreateCursor(data.data(), mask.data(), 
							static_cast<int>(image->getWidth()), static_cast<int>(image->getHeight()), 
							static_cast<int>(hotSpotX), static_cast<int>(hotSpotY));
}

WindowSDL::WindowSDL(const Window::Properties & properties) :
		Window(properties), sdlWindow(nullptr), sdlCursor(nullptr) {

			

#if defined(_WIN32)
	// workaround against high DPI scaling (https://discourse.libsdl.org/t/sdl-getdesktopdisplaymode-resolution-reported-in-windows-10-when-using-app-scaling/22389)
	typedef enum PROCESS_DPI_AWARENESS {
	    PROCESS_DPI_UNAWARE = 0,
	    PROCESS_SYSTEM_DPI_AWARE = 1,
	    PROCESS_PER_MONITOR_DPI_AWARE = 2
	} PROCESS_DPI_AWARENESS;
	BOOL(WINAPI *SetProcessDPIAware)(void) = nullptr; // Vista and later
	HRESULT(WINAPI *SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS dpiAwareness) = nullptr; // Windows 8.1 and later
	void* userDLL = SDL_LoadObject("USER32.DLL");
	if (userDLL)
	    SetProcessDPIAware = (BOOL(WINAPI *)(void)) SDL_LoadFunction(userDLL, "SetProcessDPIAware");
	void* shcoreDLL = SDL_LoadObject("SHCORE.DLL");
	if (shcoreDLL)
	    SetProcessDpiAwareness = (HRESULT(WINAPI *)(PROCESS_DPI_AWARENESS)) SDL_LoadFunction(shcoreDLL, "SetProcessDpiAwareness");
	bool success = true;
	if (SetProcessDpiAwareness) {
	    // Try Windows 8.1+ version
	    success = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK;
	} else if (SetProcessDPIAware) {
	    // Try Vista - Windows 8 version. This has a constant scale factor for all monitors.
	    success = SetProcessDPIAware();
	}
	if(!success)
		WARN("Failed to set DPI Awareness.");
#endif

	if(SDL_WasInit(SDL_INIT_VIDEO) == 0) {
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			throw std::runtime_error(std::string("SDL_INIT_VIDEO failed: ") + SDL_GetError());
		}
	}

	uint32_t sdlFlags = SDL_WINDOW_VULKAN;
	if (properties.resizable) {
		sdlFlags |= SDL_WINDOW_RESIZABLE;
	}
	if (properties.borderless) {
		sdlFlags |= SDL_WINDOW_BORDERLESS;
	}
	if (properties.fullscreen) {
		sdlFlags |= SDL_WINDOW_FULLSCREEN;
	}

	auto windowPosX = properties.posX;
	auto windowPosY = properties.posY;
	if(!properties.positioned) {
		windowPosX = SDL_WINDOWPOS_UNDEFINED;
		windowPosY = SDL_WINDOWPOS_UNDEFINED;
	}
	// create a new window
	sdlWindow = SDL_CreateWindow(properties.title.c_str(),
								 windowPosX,
								 windowPosY,
								 static_cast<int>(properties.clientAreaWidth),
								 static_cast<int>(properties.clientAreaHeight),
								 sdlFlags);
	if (sdlWindow == nullptr) {
		throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
	}
	#if defined(_WIN32) && defined(GCL_HICON)
	// On Windows, use the "IDI_MAIN_ICON" icon resource as the window's default icon.
	// If the resource is not defined, this is ignored.
	{
		HINSTANCE handle = ::GetModuleHandle(nullptr);
		HICON icon = ::LoadIcon(handle, "IDI_MAIN_ICON");
		if(icon!=nullptr){
			SDL_SysWMinfo wminfo;
			SDL_VERSION(&wminfo.version)
			if(SDL_GetWindowWMInfo(sdlWindow,&wminfo) == 1){
				HWND hwnd = wminfo.info.win.window;
				::SetClassLong(hwnd, GCL_HICON, reinterpret_cast<LONG>(icon));
			}
		}
	}
	#endif
		
	// Initialize joystick/-pad input
	if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0) {
		if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
			throw std::runtime_error(std::string("SDL_INIT_JOYSTICK failed: ") + SDL_GetError());
		}
		const int numJoy = SDL_NumJoysticks();
		if (numJoy > 0) {
			for (int i = 0; i < numJoy; ++i) {
				if(SDL_JoystickOpen(i) == nullptr) {
					throw std::runtime_error(std::string("Opening joystick ") + StringUtils::toString(i) + " failed: " + SDL_GetError());
				}
			}
			SDL_JoystickEventState(SDL_ENABLE);
		}
	}
}

WindowSDL::~WindowSDL() {
	if(sdlCursor != nullptr) {
		SDL_FreeCursor(sdlCursor);
	}
	SDL_Quit();
}

void WindowSDL::swapBuffers() {
	// unused
}

int32_t WindowSDL::getSwapInterval() const {
	return SDL_GL_GetSwapInterval();
}

Surface WindowSDL::createSurface(APIHandle apiHandle) {
	Surface surface;
	if(!SDL_Vulkan_CreateSurface(sdlWindow, apiHandle, &surface)) {
		throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
	}
	return surface;
}

std::vector<const char*> WindowSDL::getAPIExtensions() {
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(sdlWindow, &extensionCount, nullptr);
	std::vector<const char*> extensionNames(extensionCount);
	SDL_Vulkan_GetInstanceExtensions(sdlWindow, &extensionCount, extensionNames.data());
	return extensionNames;
}

//! ---|> Window
void WindowSDL::doSetCursor(const Cursor * cursor) {
	static SDL_Cursor * systemCursor = SDL_GetCursor();
	if(sdlCursor != nullptr) {
		SDL_FreeCursor(sdlCursor);
		sdlCursor = nullptr;
	}
	if(cursor == nullptr) {
		SDL_SetCursor(systemCursor);
	} else {
		sdlCursor = convertBitmapToSDLCursor(cursor->getBitmap(), cursor->getHotSpotX(), cursor->getHotSpotY());
		SDL_SetCursor(sdlCursor);
	}
}

//! ---|> Window
void WindowSDL::doHideCursor() {
	static std::unique_ptr<Cursor> hiddenCursor;
	if(hiddenCursor.get() == nullptr) {
		Reference<Bitmap> bitmap = new Bitmap(1, 1);
		bitmap->data()[0] = 0;
		bitmap->data()[1] = 0;
		bitmap->data()[2] = 0;
		bitmap->data()[3] = 0;
		hiddenCursor = createCursor(bitmap, 0, 0);
	}
	doSetCursor(hiddenCursor.get());
}

//! ---|> Window
void WindowSDL::warpCursor(int x, int y) {
	SDL_WarpMouseInWindow(sdlWindow, x, y);
}

void WindowSDL::grabInput() {
	SDL_SetWindowGrab(sdlWindow, SDL_TRUE);
}

void WindowSDL::ungrabInput() {
	SDL_SetWindowGrab(sdlWindow, SDL_FALSE);
}

//! ---|> Window
std::deque<Event> WindowSDL::fetchEvents() {
	const auto sdlWindowId = SDL_GetWindowID(sdlWindow);
	std::deque<Event> events;
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		Event event;
		switch (sdlEvent.type) {
			case SDL_QUIT:
				event.type = EVENT_QUIT;
				break;
			case SDL_WINDOWEVENT:
				if(sdlWindowId != sdlEvent.window.windowID) {
					continue;
				}
				if(sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
					const auto sdlWidth = static_cast<uint32_t>(sdlEvent.window.data1);
					const auto sdlHeight = static_cast<uint32_t>(sdlEvent.window.data2);
					if (sdlWidth != width || sdlHeight != height) {
						event.type = EVENT_RESIZE;
						event.resize.width = sdlWidth;
						event.resize.height = sdlHeight;
						width = sdlWidth;
						height = sdlHeight;
					} else {
						continue;
					}
				} else if(sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
					event.type = EVENT_QUIT;
				} else {
					continue;
				}
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				// Ignore repeated keys.
				if(sdlEvent.key.repeat) {
					continue;
				}
				event.type = EVENT_KEYBOARD;
				event.keyboard.pressed = (sdlEvent.key.state == SDL_PRESSED);
				std::fill(std::begin(event.keyboard.str), std::end(event.keyboard.str), '\0');
				event.keyboard.key = keySymToKey(sdlEvent.key.keysym.sym);
				break;
			case SDL_TEXTINPUT: {
				if(events.empty()) {
					continue;
				}
				Event & lastEvent = events.back();
				if(lastEvent.type == EVENT_KEYBOARD && lastEvent.keyboard.pressed) {
					const std::string utf8String(sdlEvent.text.text);
					std::copy(utf8String.cbegin(), utf8String.cend(), lastEvent.keyboard.str);
				}
				continue;
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				event.type = EVENT_MOUSE_BUTTON;
				event.button.x = sdlEvent.button.x;
				event.button.y = sdlEvent.button.y;
				switch(sdlEvent.button.button) {
					case SDL_BUTTON_LEFT:
						event.button.button = MOUSE_BUTTON_LEFT;
						break;
					case SDL_BUTTON_MIDDLE:
						event.button.button = MOUSE_BUTTON_MIDDLE;
						break;
					case SDL_BUTTON_RIGHT:
						event.button.button = MOUSE_BUTTON_RIGHT;
						break;
					default:
						event.button.button = MOUSE_BUTTON_OTHER;
						break;
				}
				event.button.pressed = (sdlEvent.button.state == SDL_PRESSED);
				break;
			case SDL_MOUSEWHEEL:
				event.type = EVENT_MOUSE_BUTTON;
				int x;
				int y;
				SDL_GetMouseState(&x, &y);
				event.button.x = x;
				event.button.y = y;
				if(sdlEvent.wheel.y < 0) {
					event.button.button = MOUSE_WHEEL_DOWN;
				} else if(sdlEvent.wheel.y > 0) {
					event.button.button = MOUSE_WHEEL_UP;
				} else {
					event.button.button = MOUSE_BUTTON_OTHER;
				}
				event.button.pressed = true;
				break;
			case SDL_MOUSEMOTION:
				event.type = EVENT_MOUSE_MOTION;
				event.motion.x = sdlEvent.motion.x;
				event.motion.y = sdlEvent.motion.y;
				event.motion.buttonMask = MASK_NO_BUTTON;
				if(sdlEvent.motion.state & SDL_BUTTON_LMASK) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_LEFT;
				}
				if(sdlEvent.motion.state & SDL_BUTTON_MMASK) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_MIDDLE;
				}
				if(sdlEvent.motion.state & SDL_BUTTON_RMASK) {
					event.motion.buttonMask |= MASK_MOUSE_BUTTON_RIGHT;
				}
				event.motion.deltaX = sdlEvent.motion.xrel;
				event.motion.deltaY = sdlEvent.motion.yrel;
				break;
			case SDL_JOYAXISMOTION:
				event.type = EVENT_JOY_AXIS;
				event.joyAxis.joystick = sdlEvent.jaxis.which;
				event.joyAxis.axis = sdlEvent.jaxis.axis;
				event.joyAxis.value = sdlEvent.jaxis.value;
				break;
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				event.type = EVENT_JOY_BUTTON;
				event.joyButton.joystick = sdlEvent.jbutton.which;
				event.joyButton.button = sdlEvent.jbutton.button;
				event.joyButton.pressed = (sdlEvent.jbutton.state == SDL_PRESSED);
				break;
			case SDL_JOYHATMOTION:
				event.type = EVENT_JOY_HAT;
				event.joyHat.joystick = sdlEvent.jhat.which;
				event.joyHat.value = MASK_HAT_CENTER;
				if(sdlEvent.jhat.value & SDL_HAT_UP) {
					event.joyHat.value |= MASK_HAT_UP;
				}
				if(sdlEvent.jhat.value & SDL_HAT_RIGHT) {
					event.joyHat.value |= MASK_HAT_RIGHT;
				}
				if(sdlEvent.jhat.value & SDL_HAT_DOWN) {
					event.joyHat.value |= MASK_HAT_DOWN;
				}
				if(sdlEvent.jhat.value & SDL_HAT_LEFT) {
					event.joyHat.value |= MASK_HAT_LEFT;
				}
				event.joyHat.hat = sdlEvent.jhat.hat;
				break;
			default:
				continue;
		}
		events.emplace_back(event);
	}
	return events;
}

void WindowSDL::setIcon(const Bitmap & icon) {
	// FIXME: The SDL_Surface is never freed.
	SDL_SetWindowIcon(sdlWindow, BitmapUtils::createSDLSurfaceFromBitmap(icon));
}

std::string WindowSDL::getClipboardText() const {
	if(SDL_HasClipboardText() == SDL_FALSE) {
		return std::string();
	}
	auto sdlText = SDL_GetClipboardText();
	if(sdlText == nullptr) {
		WARN(std::string("SDL_GetClipboardText failed: ") + SDL_GetError());
		return std::string();
	}
	const std::string text(sdlText);
	SDL_free(sdlText);
	return text;
}

void WindowSDL::setClipboardText(const std::string & text) {
	const auto result = SDL_SetClipboardText(text.c_str());
	if(result != 0) {
		WARN(std::string("SDL_SetClipboardText failed: ") + SDL_GetError());
	}
}

void WindowSDL::makeCurrent() {
	// unused
}

}
}

#endif /* defined(UTIL_HAVE_LIB_SDL2) */
