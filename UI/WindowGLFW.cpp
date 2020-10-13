/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_GLFW)

#include "WindowGLFW.h"
#include "Event.h"
#include "UI.h"
#include "../Utils.h"
#include "../StringUtils.h"
#include "../LibRegistry.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/BitmapUtils.h"
#include "../Graphics/PixelAccessor.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <limits>
#include <sstream>

template<typename T>
static inline T convert(float value) {
	return value < 0 ? -static_cast<T>(value * std::numeric_limits<T>::min()) : static_cast<T>(value * std::numeric_limits<T>::max());
}

static bool libNameInitailized = [](){
	int major, minor, patch;
	glfwGetVersion(&major, &minor, &patch);
	std::ostringstream s;
	s << "GLFW " << static_cast<int>(major) << "." << static_cast<int>(minor) << "." << static_cast<int>(patch)<< " (/www.glfw.org)";
	Util::LibRegistry::registerLibVersionString("LibGLFW",s.str()); 
	return true;
}();

namespace Util {
namespace UI {
		
struct JoystickState {
	JoystickState() : id(-1) {}
	JoystickState(int32_t id) : id(id) {}
	int32_t id;
	std::vector<bool> buttons;
	std::vector<int16_t> axes;
	std::vector<uint8_t> hats;
};

//------------
	
struct WindowGLFWInternal {
	GLFWwindow* window = nullptr;
	GLFWcursor* cursor = nullptr;
	std::deque<Event> eventQueue;
	uint16_t lastCursorX = 0;
	uint16_t lastCursorY = 0;
	int32_t width;
	int32_t height;
	JoystickState joystickState[GLFW_JOYSTICK_LAST+1];
};

//=========================================================================

static void error_callback(int error, const char* description) {
	WARN(description);
}

//------------

static const std::unordered_map<int16_t, Key> & getKeyMapping() {
	static std::unordered_map<int16_t, Key> keyMapping;
	if(keyMapping.empty()) {
		keyMapping[GLFW_KEY_BACKSPACE] = KEY_BACKSPACE;
		keyMapping[GLFW_KEY_TAB] = KEY_TAB;
		//keyMapping[GLFW_KEY_CLEAR] = KEY_CLEAR;
		keyMapping[GLFW_KEY_ENTER] = KEY_RETURN;
		keyMapping[GLFW_KEY_PAUSE] = KEY_PAUSE;
		keyMapping[GLFW_KEY_ESCAPE] = KEY_ESCAPE;
		keyMapping[GLFW_KEY_SPACE] = KEY_SPACE;
		//keyMapping[GLFW_KEY_EXCLAIM] = KEY_EXCLAMATION;
		//keyMapping[GLFW_KEY_QUOTEDBL] = KEY_QUOTATION;
		//keyMapping[GLFW_KEY_HASH] = KEY_NUMBERSIGN;
		//keyMapping[GLFW_KEY_DOLLAR] = KEY_DOLLAR;
		//keyMapping[GLFW_KEY_AMPERSAND] = KEY_AMPERSAND;
		//keyMapping[GLFW_KEY_QUOTE] = KEY_APOSTROPHE;
		//keyMapping[GLFW_KEY_LEFTPAREN] = KEY_PARENLEFT;
		//keyMapping[GLFW_KEY_RIGHTPAREN] = KEY_PARENRIGHT;
		//keyMapping[GLFW_KEY_ASTERISK] = KEY_ASTERISK;
		//keyMapping[GLFW_KEY_PLUS] = KEY_PLUS;
		keyMapping[GLFW_KEY_COMMA] = KEY_COMMA;
		keyMapping[GLFW_KEY_MINUS] = KEY_MINUS;
		keyMapping[GLFW_KEY_PERIOD] = KEY_PERIOD;
		keyMapping[GLFW_KEY_SLASH] = KEY_SLASH;
		keyMapping[GLFW_KEY_0] = KEY_0;
		keyMapping[GLFW_KEY_1] = KEY_1;
		keyMapping[GLFW_KEY_2] = KEY_2;
		keyMapping[GLFW_KEY_3] = KEY_3;
		keyMapping[GLFW_KEY_4] = KEY_4;
		keyMapping[GLFW_KEY_5] = KEY_5;
		keyMapping[GLFW_KEY_6] = KEY_6;
		keyMapping[GLFW_KEY_7] = KEY_7;
		keyMapping[GLFW_KEY_8] = KEY_8;
		keyMapping[GLFW_KEY_9] = KEY_9;
		//keyMapping[GLFW_KEY_COLON] = KEY_COLON;
		keyMapping[GLFW_KEY_SEMICOLON] = KEY_SEMICOLON;
		//keyMapping[GLFW_KEY_LESS] = KEY_LESS;
		keyMapping[GLFW_KEY_EQUAL] = KEY_EQUAL;
		//keyMapping[GLFW_KEY_GREATER] = KEY_GREATER;
		//keyMapping[GLFW_KEY_QUESTION] = KEY_QUESTION;
		//keyMapping[GLFW_KEY_AT] = KEY_AT;
		keyMapping[GLFW_KEY_LEFT_BRACKET] = KEY_BRACKETLEFT;
		keyMapping[GLFW_KEY_BACKSLASH] = KEY_BACKSLASH;
		keyMapping[GLFW_KEY_RIGHT_BRACKET] = KEY_BRACKETRIGHT;
		//keyMapping[GLFW_KEY_CARET] = KEY_CIRCUMFLEX;
		//keyMapping[GLFW_KEY_UNDERSCORE] = KEY_UNDERSCORE;
		keyMapping[GLFW_KEY_GRAVE_ACCENT] = KEY_GRAVE;
		keyMapping[GLFW_KEY_A] = KEY_A;
		keyMapping[GLFW_KEY_B] = KEY_B;
		keyMapping[GLFW_KEY_C] = KEY_C;
		keyMapping[GLFW_KEY_D] = KEY_D;
		keyMapping[GLFW_KEY_E] = KEY_E;
		keyMapping[GLFW_KEY_F] = KEY_F;
		keyMapping[GLFW_KEY_G] = KEY_G;
		keyMapping[GLFW_KEY_H] = KEY_H;
		keyMapping[GLFW_KEY_I] = KEY_I;
		keyMapping[GLFW_KEY_J] = KEY_J;
		keyMapping[GLFW_KEY_K] = KEY_K;
		keyMapping[GLFW_KEY_L] = KEY_L;
		keyMapping[GLFW_KEY_M] = KEY_M;
		keyMapping[GLFW_KEY_N] = KEY_N;
		keyMapping[GLFW_KEY_O] = KEY_O;
		keyMapping[GLFW_KEY_P] = KEY_P;
		keyMapping[GLFW_KEY_Q] = KEY_Q;
		keyMapping[GLFW_KEY_R] = KEY_R;
		keyMapping[GLFW_KEY_S] = KEY_S;
		keyMapping[GLFW_KEY_T] = KEY_T;
		keyMapping[GLFW_KEY_U] = KEY_U;
		keyMapping[GLFW_KEY_V] = KEY_V;
		keyMapping[GLFW_KEY_W] = KEY_W;
		keyMapping[GLFW_KEY_X] = KEY_X;
		keyMapping[GLFW_KEY_Y] = KEY_Y;
		keyMapping[GLFW_KEY_Z] = KEY_Z;
		keyMapping[GLFW_KEY_DELETE] = KEY_DELETE;
		keyMapping[GLFW_KEY_KP_0] = KEY_KP0;
		keyMapping[GLFW_KEY_KP_1] = KEY_KP1;
		keyMapping[GLFW_KEY_KP_2] = KEY_KP2;
		keyMapping[GLFW_KEY_KP_3] = KEY_KP3;
		keyMapping[GLFW_KEY_KP_4] = KEY_KP4;
		keyMapping[GLFW_KEY_KP_5] = KEY_KP5;
		keyMapping[GLFW_KEY_KP_6] = KEY_KP6;
		keyMapping[GLFW_KEY_KP_7] = KEY_KP7;
		keyMapping[GLFW_KEY_KP_8] = KEY_KP8;
		keyMapping[GLFW_KEY_KP_9] = KEY_KP9;
		keyMapping[GLFW_KEY_KP_DECIMAL] = KEY_KPSEPARATOR;
		keyMapping[GLFW_KEY_KP_DIVIDE] = KEY_KPDIVIDE;
		keyMapping[GLFW_KEY_KP_MULTIPLY] = KEY_KPMULTIPLY;
		keyMapping[GLFW_KEY_KP_SUBTRACT] = KEY_KPSUBTRACT;
		keyMapping[GLFW_KEY_KP_ADD] = KEY_KPADD;
		keyMapping[GLFW_KEY_KP_ENTER] = KEY_KPENTER;
		keyMapping[GLFW_KEY_KP_EQUAL] = KEY_KPEQUAL;
		keyMapping[GLFW_KEY_UP] = KEY_UP;
		keyMapping[GLFW_KEY_DOWN] = KEY_DOWN;
		keyMapping[GLFW_KEY_RIGHT] = KEY_RIGHT;
		keyMapping[GLFW_KEY_LEFT] = KEY_LEFT;
		keyMapping[GLFW_KEY_INSERT] = KEY_INSERT;
		keyMapping[GLFW_KEY_HOME] = KEY_HOME;
		keyMapping[GLFW_KEY_END] = KEY_END;
		keyMapping[GLFW_KEY_PAGE_UP] = KEY_PAGEUP;
		keyMapping[GLFW_KEY_PAGE_DOWN] = KEY_PAGEDOWN;
		keyMapping[GLFW_KEY_F1] = KEY_F1;
		keyMapping[GLFW_KEY_F2] = KEY_F2;
		keyMapping[GLFW_KEY_F3] = KEY_F3;
		keyMapping[GLFW_KEY_F4] = KEY_F4;
		keyMapping[GLFW_KEY_F5] = KEY_F5;
		keyMapping[GLFW_KEY_F6] = KEY_F6;
		keyMapping[GLFW_KEY_F7] = KEY_F7;
		keyMapping[GLFW_KEY_F8] = KEY_F8;
		keyMapping[GLFW_KEY_F9] = KEY_F9;
		keyMapping[GLFW_KEY_F10] = KEY_F10;
		keyMapping[GLFW_KEY_F11] = KEY_F11;
		keyMapping[GLFW_KEY_F12] = KEY_F12;
		keyMapping[GLFW_KEY_F13] = KEY_F13;
		keyMapping[GLFW_KEY_F14] = KEY_F14;
		keyMapping[GLFW_KEY_F15] = KEY_F15;
		keyMapping[GLFW_KEY_NUM_LOCK] = KEY_NUMLOCK;
		keyMapping[GLFW_KEY_CAPS_LOCK] = KEY_CAPSLOCK;
		keyMapping[GLFW_KEY_SCROLL_LOCK] = KEY_SCROLLLOCK;
		keyMapping[GLFW_KEY_RIGHT_SHIFT] = KEY_SHIFTR;
		keyMapping[GLFW_KEY_LEFT_SHIFT] = KEY_SHIFTL;
		keyMapping[GLFW_KEY_RIGHT_CONTROL] = KEY_CONTROLR;
		keyMapping[GLFW_KEY_LEFT_CONTROL] = KEY_CONTROLL;
		keyMapping[GLFW_KEY_RIGHT_ALT] = KEY_ALTR;
		keyMapping[GLFW_KEY_LEFT_ALT] = KEY_ALTL;
		keyMapping[GLFW_KEY_RIGHT_SUPER] = KEY_METAR;
		keyMapping[GLFW_KEY_LEFT_SUPER] = KEY_METAL;
		//keyMapping[GLFW_KEY_MODE] = KEY_MODESWITCH;
		//keyMapping[GLFW_KEY_APPLICATION] = KEY_MULTIKEY;
		//keyMapping[GLFW_KEY_HELP] = KEY_HELP;
		keyMapping[GLFW_KEY_PRINT_SCREEN] = KEY_PRINT;
		//keyMapping[GLFW_KEY_SYSREQ] = KEY_SYSREQ;
		keyMapping[GLFW_KEY_MENU] = KEY_MENU;
		//keyMapping[GLFW_KEY_UNDO] = KEY_UNDO;
	}
	return keyMapping;
}

//------------

static Key keySymToKey(int16_t sym) {
	const auto it = getKeyMapping().find(sym);
	if(it == getKeyMapping().end()) {
		return KEY_NONE;
	}
	return it->second;
}

//------------

static GLFWcursor * convertBitmapToglfwCursor(const Reference<Bitmap> & image, unsigned int hotSpotX, unsigned int hotSpotY) {
	if(image == nullptr)
		throw std::invalid_argument("No bitmap given.");	
	Reference<Bitmap> converted = image;
	if(image->getPixelFormat() != PixelFormat::RGBA)
		converted = BitmapUtils::convertBitmap(*image.get(), PixelFormat::RGBA);	
	GLFWimage glfwImage;
	glfwImage.width = static_cast<int>(image->getWidth());
	glfwImage.height = static_cast<int>(image->getHeight());
	glfwImage.pixels = converted->data();	
	GLFWimage cursor {glfwImage.width, glfwImage.height, glfwImage.pixels};
	return glfwCreateCursor(&cursor, static_cast<int>(hotSpotX), static_cast<int>(hotSpotY));
}

//=========================================================================
// Event handling

static void handleMousePosition(GLFWwindow* window, double xpos, double ypos) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetWindowUserPointer(window));
	Event event;
	event.type = EVENT_MOUSE_MOTION;
	event.motion.x = static_cast<uint16_t>(xpos);
	event.motion.y = static_cast<uint16_t>(ypos);
	event.motion.buttonMask = MASK_NO_BUTTON;
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		event.motion.buttonMask |= MASK_MOUSE_BUTTON_LEFT;
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
		event.motion.buttonMask |= MASK_MOUSE_BUTTON_MIDDLE;
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		event.motion.buttonMask |= MASK_MOUSE_BUTTON_RIGHT;
	event.motion.deltaX = static_cast<int16_t>(event.motion.x - data->lastCursorX);
	event.motion.deltaY = static_cast<int16_t>(event.motion.y - data->lastCursorY);	
	data->lastCursorX = event.motion.x;
	data->lastCursorY = event.motion.y;
	data->eventQueue.emplace_back(event);
}

//------------

static void handleMouseButton(GLFWwindow* window, int button, int action, int mods) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetWindowUserPointer(window));
	Event event;
	event.type = EVENT_MOUSE_BUTTON;	
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);	
	event.button.x = static_cast<uint16_t>(xpos);
	event.button.y = static_cast<uint16_t>(ypos);
	switch(button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			event.button.button = MOUSE_BUTTON_LEFT;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			event.button.button = MOUSE_BUTTON_MIDDLE;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			event.button.button = MOUSE_BUTTON_RIGHT;
			break;
		default:
			event.button.button = MOUSE_BUTTON_OTHER;
			break;
	}
	event.button.pressed = (action == GLFW_PRESS);		
	data->eventQueue.emplace_back(event);
}

//------------

static void handleMouseWheel(GLFWwindow* window, double xoffset, double yoffset) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetWindowUserPointer(window));
	Event event;
	event.type = EVENT_MOUSE_BUTTON;
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);	
	event.button.x = static_cast<uint16_t>(xpos);
	event.button.y = static_cast<uint16_t>(ypos);
	if(yoffset < 0) {
		event.button.button = MOUSE_WHEEL_DOWN;
	} else if(yoffset > 0) {
		event.button.button = MOUSE_WHEEL_UP;
	} else {
		event.button.button = MOUSE_BUTTON_OTHER;
	}
	event.button.pressed = true;
	data->eventQueue.emplace_back(event);
}

//------------

static void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetWindowUserPointer(window));
	if(action == GLFW_REPEAT)
		return; // ignore repeated keys
	Event event;
	event.type = EVENT_KEYBOARD;
	event.keyboard.pressed = (action == GLFW_PRESS);
	std::fill(std::begin(event.keyboard.str), std::end(event.keyboard.str), '\0');
	event.keyboard.key = keySymToKey(key);	
	data->eventQueue.emplace_back(event);
}

//------------

static void handleText(GLFWwindow* window, unsigned int codepoint) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetWindowUserPointer(window));
	Event event;
	event.type = EVENT_KEYBOARD;
	event.keyboard.pressed = true;
	event.keyboard.key = KEY_NONE; // dont't trigger the actual key event a second time
	std::fill(std::begin(event.keyboard.str), std::end(event.keyboard.str), '\0');
	std::string utf8String(StringUtils::utf32_to_utf8(codepoint));
	std::copy(utf8String.cbegin(), utf8String.cend(), event.keyboard.str);
	data->eventQueue.emplace_back(event);
	
	// Doesn't work as the char callback is not executed in the same frame
	/*if(data->eventQueue.empty())
		return;
	Event& lastEvent = data->eventQueue.back();
	if(lastEvent.type == EVENT_KEYBOARD && lastEvent.keyboard.pressed) {
		std::string utf8String(StringUtils::utf32_to_utf8(codepoint));
		std::copy(utf8String.cbegin(), utf8String.cend(), lastEvent.keyboard.str);
	}*/
}

//------------

static void handleWindowClose(GLFWwindow* window) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetWindowUserPointer(window));
	Event event;
	event.type = EVENT_QUIT;
	
	data->eventQueue.emplace_back(event);
}

//------------

static void handleWindowSize(GLFWwindow* window, int newWidth, int newHeight) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetWindowUserPointer(window));
	if (newWidth != data->width || newHeight != data->height) {
		Event event;
		event.type = EVENT_RESIZE;
		event.resize.width = newWidth;
		event.resize.height = newHeight;
		data->width = newWidth;
		data->height = newHeight;
		data->eventQueue.emplace_back(event);
	} 
}

//------------

static void handleJoystickConntection(int jid, int event) {
	auto* data = reinterpret_cast<WindowGLFWInternal*>(glfwGetJoystickUserPointer(jid));
	if(event == GLFW_CONNECTED) {
		data->joystickState[jid] = {jid};
		int count;
		glfwGetJoystickButtons(jid, &count);
		data->joystickState[jid].buttons.resize(count, false);
		glfwGetJoystickHats(jid, &count);
		data->joystickState[jid].hats.resize(count, 0);
		glfwGetJoystickAxes(jid, &count);
		data->joystickState[jid].axes.resize(count, 0);
	} else if(event == GLFW_DISCONNECTED) {
		data->joystickState[jid].id = -1;
	}
}

//=========================================================================

WindowGLFW::WindowGLFW(const Window::Properties & properties) :
		Window(properties), data(new WindowGLFWInternal) {
	
	glfwSetErrorCallback(error_callback);
	if(!glfwInit()) {
		throw std::runtime_error(std::string("glfw Init failed: "));
	}
	if(!glfwVulkanSupported())
		throw std::runtime_error(std::string("WindowGLFW: Vulkan not supported."));
	
	// window
	glfwWindowHint(GLFW_RESIZABLE, properties.resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, properties.borderless ? GLFW_FALSE : GLFW_TRUE);
	
	// GL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, properties.contextVersionMajor);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, properties.contextVersionMinor);
	
	/*if(properties.compatibilityProfile) {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	} else {
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}*/

	if (properties.multisampled) {
		glfwWindowHint(GLFW_SAMPLES, static_cast<int>(properties.multisamples));
	} else {
		glfwWindowHint(GLFW_SAMPLES, 0);
	}
	data->width = static_cast<int>(properties.clientAreaWidth);
	data->height = static_cast<int>(properties.clientAreaHeight);

	// create a new window
	data->window = glfwCreateWindow(
		data->width, 
		data->height, 
		properties.title.c_str(), 
		properties.fullscreen ? glfwGetPrimaryMonitor() : nullptr, 
		nullptr
	);
	
	if (!data->window) {
		throw std::runtime_error(std::string("GLFW_CreateWindow failed"));
	}
	glfwSetWindowUserPointer(data->window, data.get());
	
	if(properties.positioned) {
		glfwSetWindowPos(data->window, properties.posX, properties.posY);
	}
	
	glfwMakeContextCurrent(data->window);
	glfwSwapInterval(0); // disable vsync
	
	glfwSetCursorPosCallback(data->window, handleMousePosition);
	glfwSetMouseButtonCallback(data->window, handleMouseButton);
	glfwSetScrollCallback(data->window, handleMouseWheel);
	glfwSetKeyCallback(data->window, handleKeyboard);
	glfwSetCharCallback(data->window, handleText);
	glfwSetWindowSizeCallback(data->window, handleWindowSize);
	glfwSetWindowCloseCallback(data->window, handleWindowClose);
	glfwSetJoystickCallback(handleJoystickConntection);

	// Initialize joystick/-pad input
	for(auto jid=0; jid<=GLFW_JOYSTICK_LAST; ++jid) {
		glfwSetJoystickUserPointer(jid, data.get());
		if(glfwJoystickPresent(jid) == GLFW_TRUE)
			handleJoystickConntection(jid, GLFW_CONNECTED);
	}
}

//------------

WindowGLFW::~WindowGLFW() {
	if(data->window) {
		glfwDestroyWindow(data->window);
	}
	if(data->cursor) {
		glfwDestroyCursor(data->cursor);
	}
	glfwTerminate();
}

//------------

void WindowGLFW::swapBuffers() {
	glfwSwapBuffers(data->window);
}

//------------

int32_t WindowGLFW::getSwapInterval() const {
	return 0;
}

//------------

Surface WindowGLFW::createSurface(APIHandle apiHandle) {
	Surface surface;
	if(!glfwCreateWindowSurface(apiHandle, data->window, NULL, &surface)) {
		throw std::runtime_error("WindowGLFW::createSurface failed.");
	}
	return surface;
}

//------------

std::vector<const char*> WindowGLFW::getAPIExtensions() {
	uint32_t extensionCount;	
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> extensionNames(extensions, extensions + extensionCount);
	return extensionNames;
}

//------------

//! ---|> Window
void WindowGLFW::doSetCursor(const Cursor * cursor) {
	if(data->cursor) {
		glfwDestroyCursor(data->cursor);
		data->cursor = nullptr;
	}
	if(!cursor) {
		data->cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	} else {
		data->cursor = convertBitmapToglfwCursor(cursor->getBitmap(), cursor->getHotSpotX(), cursor->getHotSpotY());
	}
	if(data->cursor)
		glfwSetCursor(data->window, data->cursor);
}

//------------

//! ---|> Window
void WindowGLFW::doHideCursor() {
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

//------------

//! ---|> Window
void WindowGLFW::warpCursor(int x, int y) {
	glfwSetCursorPos(data->window, x, y);
}

//------------

void WindowGLFW::grabInput() {
	glfwSetInputMode(data->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

//------------

void WindowGLFW::ungrabInput() {
	glfwSetInputMode(data->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

//------------

//! ---|> Window
std::deque<Event> WindowGLFW::fetchEvents() {
	glfwPollEvents();
	std::deque<Event> events;
	data->eventQueue.swap(events);
	
	for(auto jid=0; jid<=GLFW_JOYSTICK_LAST; ++jid) {
		auto& joy = data->joystickState[jid];
		if(joy.id < 0) continue;
		int count;
		// Joy axes
		const float* axes = glfwGetJoystickAxes(jid, &count);
		for(uint_fast32_t i=0; i<count; ++i) {
			int16_t value = convert<int16_t>(axes[i]);
			if(value != joy.axes[i]) {
				Event event;
				event.type = EVENT_JOY_AXIS;
				event.joyAxis.joystick = jid;
				event.joyAxis.axis = i;
				event.joyAxis.value = value;
				joy.axes[i] = value;
				events.emplace_back(event);
			}
		}
		
		// Joy buttons
		const uint8_t* buttons = glfwGetJoystickButtons(jid, &count);
		for(uint_fast32_t i=0; i<count; ++i) {
			bool pressed = buttons[i] == GLFW_PRESS;
			if(pressed != joy.buttons[i]) {
				Event event;
				event.type = EVENT_JOY_BUTTON;
				event.joyButton.joystick = jid;
				event.joyButton.button = i;
				event.joyButton.pressed = pressed;
				joy.buttons[i] = pressed;
				events.emplace_back(event);
			}
		}
		
		// Joy hats
		const uint8_t* hats = glfwGetJoystickHats(jid, &count);
		for(uint_fast32_t i=0; i<count; ++i) {
			if(hats[i] != joy.hats[i]) {
				Event event;
				event.type = EVENT_JOY_HAT;
				event.joyHat.joystick = jid;
				event.joyHat.hat = i;
				event.joyHat.value = MASK_HAT_CENTER;
				if(hats[i] & GLFW_HAT_UP)
					event.joyHat.value |= MASK_HAT_UP;
				if(hats[i] & GLFW_HAT_RIGHT)
					event.joyHat.value |= MASK_HAT_RIGHT;
				if(hats[i] & GLFW_HAT_DOWN)
					event.joyHat.value |= MASK_HAT_DOWN;
				if(hats[i] & GLFW_HAT_LEFT)
					event.joyHat.value |= MASK_HAT_LEFT;
				joy.hats[i] = hats[i];
				events.emplace_back(event);
			}
		}
	}
	
	return events;
}

//------------

void WindowGLFW::setIcon(const Bitmap & icon) {
	Reference<Bitmap> converted = BitmapUtils::convertBitmap(icon, PixelFormat::RGBA);	
	GLFWimage glfwImage;
	glfwImage.width = static_cast<int>(converted->getWidth());
	glfwImage.height = static_cast<int>(converted->getHeight());
	glfwImage.pixels = converted->data();
	glfwSetWindowIcon(data->window, 1, &glfwImage);
}

//------------

std::string WindowGLFW::getClipboardText() const {
	const char* text = glfwGetClipboardString(nullptr);
	if(text) {
		return std::string(text);
	} else {
		return "";
	}
}

//------------

void WindowGLFW::setClipboardText(const std::string & text) {
	glfwSetClipboardString(nullptr, text.c_str());
}

//------------

void WindowGLFW::makeCurrent() {
	glfwMakeContextCurrent(data->window);
}

//------------

}
}

#endif /* defined(UTIL_HAVE_LIB_SDL2) */
