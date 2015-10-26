/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>

	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11)

#ifndef WINDOWX11DATA_H_
#define WINDOWX11DATA_H_

#include "../References.h"
#include <X11/Xlib.h>
#include <string>
#include <vector>

#if defined(UTIL_X11_JOYSTICK_SUPPORT)
// taken from <linux/input.h>, including it would redefine key mappings
#define ABS_HAT0X		0x10
#define ABS_HAT0Y		0x11
#define ABS_HAT1X		0x12
#define ABS_HAT1Y		0x13
#define ABS_HAT2X		0x14
#define ABS_HAT2Y		0x15
#define ABS_HAT3X		0x16
#define ABS_HAT3Y		0x17

#define ABS_MAX 		0x3f
#define ABS_CNT			(ABS_MAX+1)
#endif
/**
 * @file
 * This file is not part of the public API of Util.
 * Therefore, it must not be included from other header files.
 */

namespace Util {
class Bitmap;
namespace UI {

#if defined(UTIL_X11_JOYSTICK_SUPPORT)
struct X11JoystickInfo {
	int handle;
	int axes;
	int buttons;
	uint8_t hat;
	char axesMap[ABS_CNT];
	X11JoystickInfo() : handle(-1), axes(0), buttons(0), hat(0) { }
};
#endif

/**
 * Container for X11 structures.
 *
 * @author Benjamin Eikel
 * @date 2012-10-08
 */
struct WindowX11Data {
	Display * display;
	Colormap colorMap;
	XID window;
	Atom deleteMessage;
	XIM inputMethod;
	XIC inputContext;
	XID cursor;

	//! Storage for the internal clipboard text.
	std::string clipboard;

	bool freeColorMap;
	bool freeWindow;
	bool freeContext;

	WindowX11Data();
	~WindowX11Data();

	//! Remove the border created by the window manager.
	void removeWindowBorder();

	//! Make sure that the window cannot be resized.
	void fixWindowSize(int width, int height);

	//! Assign an icon to the window.
	void setWindowIcon(const Bitmap & icon);


#if defined(UTIL_X11_JOYSTICK_SUPPORT)
	std::vector<X11JoystickInfo> activeJoysticks;
#endif
};

}
}

#endif /* WINDOWX11DATA_H_ */

#endif /* defined(UTIL_HAVE_LIB_X11) */
