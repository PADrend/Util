/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "UI.h"
#include "../Graphics/Bitmap.h"
#include "../Macros.h"
#include <exception>
#include <memory>
#include <string>

#include "Cursor.h"

#if defined(_WIN32)
#include "SplashScreenWin.h"
#endif

#if defined(UTIL_PREFER_SDL_CONTEXT) && defined(UTIL_HAVE_LIB_SDL2)
#include "WindowSDL.h"
#elif defined(UTIL_HAVE_LIB_GLFW)
#include "WindowGLFW.h"
#endif

namespace Util {
namespace UI {

std::unique_ptr<Cursor> createCursor(const Reference<Bitmap> & image, unsigned int hotSpotX, unsigned int hotSpotY) {
	return std::unique_ptr<Cursor>(new Cursor(image, hotSpotX, hotSpotY));
}

#if defined(_WIN32)
std::unique_ptr<SplashScreen> createSplashScreen(const std::string & splashTitle, const Reference<Bitmap> & splashImage) {
#else
std::unique_ptr<SplashScreen> createSplashScreen(const std::string & splashTitle __attribute__((unused)), const Reference<Bitmap> & splashImage __attribute__((unused))) {
#endif
	std::unique_ptr<SplashScreen> splash;
#if defined(_WIN32)
	splash.reset(new SplashScreenWin(splashTitle, splashImage));
#endif
	if(splash && !splash->errorMessage.empty()) {
		WARN("Creating splash screen failed. " + splash->errorMessage);
		splash.reset();
	}
	return splash;
}

Util::Reference<Window> createWindow(const Window::Properties & properties) {
	Util::Reference<Window> window;
	try {
#if defined(UTIL_PREFER_SDL_CONTEXT) && defined(UTIL_HAVE_LIB_SDL2)
		window = new WindowSDL(properties);
#elif defined(UTIL_HAVE_LIB_GLFW)
		window = new WindowGLFW(properties);
#else
		throw std::logic_error("no window library available");
#endif
	} catch(const std::exception & exception) {
		WARN(std::string("Creating window failed. ") + exception.what());
		window = nullptr;
	}
	return window;
}

}
}
