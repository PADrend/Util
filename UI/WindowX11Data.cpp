/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11)

#include "WindowX11Data.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/Color.h"
#include "../Graphics/PixelAccessor.h"
#include "../References.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace Util {
namespace UI {

WindowX11Data::WindowX11Data() :
	display(nullptr),
	inputMethod(nullptr), inputContext(nullptr), cursor(None),
	clipboard(),
	freeColorMap(false), freeWindow(false), freeContext(false) {
}

WindowX11Data::~WindowX11Data() {
	if(display != nullptr) {
		if (cursor != None) {
			XFreeCursor(display, cursor);
		}
		if (inputContext) {
			XDestroyIC(inputContext);
		}
		if (inputMethod) {
			XCloseIM(inputMethod);
		}
		if(freeWindow) {
			XDestroyWindow(display, window);
		}
		if(freeColorMap) {
			XFreeColormap(display, colorMap);
		}
		XCloseDisplay(display);
	}
}

void WindowX11Data::removeWindowBorder() {
	// Remove border created by window manager.
	Atom wmHints = XInternAtom(display, "_MOTIF_WM_HINTS", True);
	if (wmHints != None) {
		struct {
				unsigned long flags;
				unsigned long functions;
				unsigned long decorations;
				long input_mode;
				unsigned long status;
		} motifWmHints = { 2, 0, 0, 0, 0 };
		XChangeProperty(display, window, wmHints, wmHints, 32, PropModeReplace, reinterpret_cast<unsigned char *> (&motifWmHints),
				sizeof(motifWmHints) / sizeof(long));
	}
}

void WindowX11Data::fixWindowSize(int width, int height) {
	XSizeHints * sizeHints = XAllocSizeHints();
	if (sizeHints == nullptr) {
		return;
	}
	sizeHints->min_width = width;
	sizeHints->max_width = width;
	sizeHints->min_height = height;
	sizeHints->max_height = height;
	sizeHints->flags = PMinSize | PMaxSize;
	XSetWMNormalHints(display, window, sizeHints);
	XFree(sizeHints);
}

void WindowX11Data::setWindowIcon(const Bitmap & icon) {
	Atom _NET_WM_ICON = XInternAtom(display, "_NET_WM_ICON", False);
	Atom CARDINAL = XInternAtom(display, "CARDINAL", False);
	if (_NET_WM_ICON == None || CARDINAL == None) {
		return;
	}

	auto pa = PixelAccessor::create(const_cast<Bitmap *>(&icon));

	const size_t dataSize = 2 + (icon.getWidth() * icon.getHeight());
	std::vector<long> data;
	data.reserve(dataSize);

	data.push_back(icon.getWidth());
	data.push_back(icon.getHeight());

	for (uint_fast32_t y = 0; y < icon.getHeight(); ++y) {
		for (uint_fast32_t  x = 0; x < icon.getWidth(); ++x) {
			const auto color = pa->readColor4ub(x, y);
			// Change the format to ARGB
			long outColor = 0;
			outColor |= color.getB() << 0;
			outColor |= color.getG() << 8;
			outColor |= color.getR() << 16;
			outColor |= color.getA() << 24;
			data.push_back(outColor);
		}
	}

	XChangeProperty(display, window, _NET_WM_ICON, CARDINAL,
					32, PropModeReplace,
					reinterpret_cast<unsigned char *>(data.data()),
					dataSize);

	XFlush(display);
}

}
}

#endif /* defined(UTIL_HAVE_LIB_X11) */
