/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11)

#include "SplashScreenX11.h"
#include "WindowX11Data.h"
#include "../Graphics/Bitmap.h"
#include "../Utils.h"
#include "../Macros.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>

namespace Util {
namespace UI {

struct SplashScreenX11::SplashScreenData {
	std::unique_ptr<WindowX11Data> x11Data;
	GC graphicsContext;
	Pixmap pixmap;

	int width;
	int height;

	bool freeGC;
	bool freePixmap;

	bool keepRunning;

	mutable std::mutex dataMutex;

	SplashScreenData() : 
		x11Data(new WindowX11Data), 
		freeGC(false), 
		freePixmap(false),
		keepRunning(true),
		dataMutex() {
	}

	~SplashScreenData() {
		if(freeGC) {
			XFreeGC(x11Data->display, graphicsContext);
		}
		if(freePixmap) {
			XFreePixmap(x11Data->display, pixmap);
		}
		x11Data.reset();
	}

	bool getKeepRunning() const {
		std::lock_guard<std::mutex> lock(dataMutex);
		return keepRunning;
	}
	void setKeepRunning(bool newState) {
		std::lock_guard<std::mutex> lock(dataMutex);
		keepRunning = newState;
	}
};

void SplashScreenX11::eventLoop() {
	XEvent xev;
	while(data->getKeepRunning()) {
		XLockDisplay(data->x11Data->display);
		while (XPending(data->x11Data->display) > 0) {
			XNextEvent(data->x11Data->display, &xev);
		}
		XUnlockDisplay(data->x11Data->display);
		Utils::sleep(20);
	}
}

SplashScreenX11::SplashScreenX11(const std::string & splashTitle, const Reference<Bitmap> & splashImage) :
	SplashScreen(), data(new SplashScreenData) {
	if (splashImage == nullptr) {
		errorMessage = "nullptr pointer given as splash image.";
		return;
	}

	XInitThreads();

	data->x11Data->display = XOpenDisplay(nullptr);

	if (data->x11Data->display == nullptr) {
		errorMessage = "Failed to open X display.";
		return;
	}

	const int defaultScreen = XDefaultScreen(data->x11Data->display);
	const int dWidth = XDisplayWidth(data->x11Data->display, defaultScreen);
	const int dHeight = XDisplayHeight(data->x11Data->display, defaultScreen);
	const int iWidth = splashImage->getWidth();
	const int iHeight = splashImage->getHeight();
	int x = 0;
	if (iWidth < dWidth) {
		// Center the image horizontally.
		x = (dWidth - iWidth) / 2;
	}
	int y = 0;
	if (iHeight < dHeight) {
		// Center the image vertically.
		y = (dHeight - iHeight) / 2;
	}

	const int depth = XDefaultDepth(data->x11Data->display, defaultScreen);
	Visual * defaultVisual = XDefaultVisual(data->x11Data->display, defaultScreen);
	const XID rootWindow = XDefaultRootWindow(data->x11Data->display);

	XSetWindowAttributes attributes;
	attributes.override_redirect = true;
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wzero-as-null-pointer-constant)
	data->x11Data->window = XCreateWindow(data->x11Data->display, rootWindow, 0, 0, iWidth, iHeight, 0, depth, InputOutput, CopyFromParent, CWOverrideRedirect, &attributes);
COMPILER_WARN_POP
	if (!data->x11Data->window) {
		errorMessage = "Failed to create simple window.";
		return;
	} else {
		data->x11Data->freeWindow = true;
	}

	data->width = iWidth;
	data->height = iHeight;

	data->x11Data->removeWindowBorder();

	// See http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#id2577988
	Atom _NET_WM_WINDOW_TYPE = XInternAtom(data->x11Data->display, "_NET_WM_WINDOW_TYPE", False);
	Atom _NET_WM_WINDOW_TYPE_SPLASH = XInternAtom(data->x11Data->display, "_NET_WM_WINDOW_TYPE_SPLASH", False);
	Atom ATOM = XInternAtom(data->x11Data->display, "ATOM", False);
	long windowTypes = _NET_WM_WINDOW_TYPE_SPLASH;
	XChangeProperty(data->x11Data->display, data->x11Data->window, _NET_WM_WINDOW_TYPE, ATOM, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&windowTypes), 1);
	
	XSetTransientForHint(data->x11Data->display, data->x11Data->window, None);

	data->graphicsContext = XCreateGC(data->x11Data->display, data->x11Data->window, 0, nullptr);
	data->freeGC = true;

	double ratioR = defaultVisual->red_mask / 255.0;
	double ratioG = defaultVisual->green_mask / 255.0;
	double ratioB = defaultVisual->blue_mask / 255.0;


	// Always use 32 bit.
	const size_t newDataSize = iWidth * iHeight;
	std::vector<uint32_t> newBuffer;
	newBuffer.reserve(newDataSize);

	const uint8_t * oldBuffer = splashImage->data();

	for (size_t i = 0; i < newDataSize; ++i) {
		uint32_t r = *oldBuffer * ratioR;
		++oldBuffer;
		uint32_t g = *oldBuffer * ratioG;
		++oldBuffer;
		uint32_t b = *oldBuffer * ratioB;
		++oldBuffer;
		if (splashImage->getPixelFormat().getBytesPerPixel() == 4) {
			++oldBuffer;
		}
		r &= defaultVisual->red_mask;
		g &= defaultVisual->green_mask;
		b &= defaultVisual->blue_mask;
		newBuffer.push_back(r | g | b);
	}

COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wzero-as-null-pointer-constant)
	XImage * image = XCreateImage(data->x11Data->display, CopyFromParent, depth, ZPixmap, 0, reinterpret_cast<char *>(newBuffer.data()), iWidth, iHeight, 32, 0);
COMPILER_WARN_POP

	data->pixmap = XCreatePixmap(data->x11Data->display, rootWindow, iWidth, iHeight, depth);
	data->freePixmap = true;

	XPutImage(data->x11Data->display, data->pixmap, data->graphicsContext, image, 0, 0, 0, 0, iWidth, iHeight);

	// Make sure XDestroyImage does not mess with our pointer.
	image->data = nullptr;
	XDestroyImage(image);

	XSetWindowBackgroundPixmap(data->x11Data->display, data->x11Data->window, data->pixmap);

	XStoreName(data->x11Data->display, data->x11Data->window, splashTitle.c_str());

	XMapRaised(data->x11Data->display, data->x11Data->window);
	XMoveWindow(data->x11Data->display, data->x11Data->window, x, y);

	eventThread = std::thread(std::bind(&SplashScreenX11::eventLoop, this));
}

SplashScreenX11::~SplashScreenX11() {
	data->setKeepRunning(false);
	eventThread.join();
}

void SplashScreenX11::showMessage(const std::string & message) {
	XLockDisplay(data->x11Data->display);
	XSetForeground(data->x11Data->display, data->graphicsContext, 0xFFFFFF);
	XFillRectangle(data->x11Data->display, data->x11Data->window, data->graphicsContext, 5, data->height - 25, data->width - 10, 20);
	XSetForeground(data->x11Data->display, data->graphicsContext, 0x000000);
	XDrawRectangle(data->x11Data->display, data->x11Data->window, data->graphicsContext, 5, data->height - 25, data->width - 10, 20);
	XDrawString(data->x11Data->display, data->x11Data->window, data->graphicsContext, 10, data->height - 10, message.data(), message.length());
	XUnlockDisplay(data->x11Data->display);
}

void SplashScreenX11::removeMessage() {

}

}
}

#endif /* defined(UTIL_HAVE_LIB_X11) */
