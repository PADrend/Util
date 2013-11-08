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
#include "../Macros.h"
#include <exception>
#include <memory>
#include <string>

#include "Cursor.h"

#if defined(UTIL_HAVE_LIB_X11)
#include "SplashScreenX11.h"
#elif defined(_WIN32)
#include "SplashScreenWin.h"
#endif

#if defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_GLX) and defined(UTIL_HAVE_GLX_GETPROCADDRESSARB)
#include "WindowGLX.h"
#elif defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_EGL)
#include "WindowEGL.h"
#elif defined(UTIL_HAVE_LIB_SDL2)
#include "WindowSDL.h"
#endif

namespace Util {
namespace UI {

std::unique_ptr<Cursor> createCursor(const Reference<Bitmap> & image, unsigned int hotSpotX, unsigned int hotSpotY) {
	return std::unique_ptr<Cursor>(new Cursor(image, hotSpotX, hotSpotY));
}

std::unique_ptr<SplashScreen> createSplashScreen(const std::string & splashTitle __attribute__((unused)), const Reference<Bitmap> & splashImage __attribute__((unused))) {
	std::unique_ptr<SplashScreen> splash;
#if defined(UTIL_HAVE_LIB_X11)
	splash.reset(new SplashScreenX11(splashTitle, splashImage));
#elif defined(_WIN32)
	splash.reset(new SplashScreenWin(splashTitle, splashImage));
#endif
	if(splash && !splash->errorMessage.empty()) {
		WARN("Creating splash screen failed. " + splash->errorMessage);
		splash.reset();
	}
	return splash;
}

std::unique_ptr<Window> createWindow(const Window::Properties & properties) {
	std::unique_ptr<Window> window;
	try {
#if defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_GLX) and defined(UTIL_HAVE_GLX_GETPROCADDRESSARB)
		window.reset(new WindowGLX(properties));
#elif defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_EGL)
		window.reset(new WindowEGL(properties));
#elif defined(UTIL_HAVE_LIB_SDL2)
		window.reset(new WindowSDL(properties));
#else
		throw std::logic_error("no window library available");
#endif
	} catch(const std::exception & exception) {
		WARN(std::string("Creating window failed. ") + exception.what());
		window.reset();
	}
	return window;
}

}
}
