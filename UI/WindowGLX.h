/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_GLX) and defined(UTIL_HAVE_GLX_GETPROCADDRESSARB)

#ifndef WINDOWGLX_H_
#define WINDOWGLX_H_

#include "WindowX11.h"
#include <memory>

namespace Util {
namespace UI {

/**
 * Class to create an X11 window with an embedded OpenGL rendering context by using GLX.
 *
 * @author Benjamin Eikel
 * @date 2010-06-22
 */
class WindowGLX : public WindowX11 {
	public:
		//! Destroy the window and free the allocated resources.
		virtual ~WindowGLX();

		//! Swap front and back buffer of the window.
		void swapBuffers() override;

		//! @see GLX_EXT_swap_control
		int32_t getSwapInterval() const override;

	private:
		//! Create a new window.
		WindowGLX(const Properties & properties);

		//! Allow access to constructor from factory.
		friend Window * createWindow(const Properties & properties);

	private:
		//! Structure used to decouple declaration and definition of attributes.
		struct WindowGLXData;

		//! Storage of attributes.
		std::unique_ptr<WindowGLXData> glxData;
};

}
}

#endif /* WINDOWGLX_H_ */

#endif /* defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_GLX) and defined(UTIL_HAVE_GLX_GETPROCADDRESSARB) */
