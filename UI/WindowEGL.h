/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_EGL)

#ifndef WINDOWEGL_H_
#define WINDOWEGL_H_

#include "WindowX11.h"
#include <memory>

namespace Util {
namespace UI {

/**
 * Class to create an X11 window with an embedded OpenGL rendering context by using EGL.
 *
 * @author Benjamin Eikel
 * @date 2010-06-30
 */
class WindowEGL : public WindowX11 {
	public:
		//! Destroy the window and free the allocated resources.
		virtual ~WindowEGL();

		//! Swap front and back buffer of the window.
		void swapBuffers() override;

	protected:
		//! Create a new window.
		WindowEGL(const Properties & properties);

		//! Allow access to constructor from factory.
		friend Window * createWindow(const Properties & properties);

	private:
		//! Structure used to decouple declaration and definition of attributes.
		struct WindowEGLData;

		//! Storage of attributes.
		std::unique_ptr<WindowEGLData> eglData;
};

}
}

#endif /* WINDOWEGL_H_ */

#endif /* defined(UTIL_HAVE_LIB_X11) and defined(UTIL_HAVE_LIB_EGL) */
