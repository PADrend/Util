/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_GLFW)

#ifndef WINDOWGLFW_H_
#define WINDOWGLFW_H_

#include "Window.h"
#include <memory>

namespace Util {
namespace UI {
struct WindowGLFWInternal;

/**
 * Class to create a window with an embedded OpenGL rendering context by using GLFW.
 */
class WindowGLFW : public Window {
	public:
		//! Destroy the window and free the allocated resources.
		UTILAPI virtual ~WindowGLFW();

		//! Swap front and back buffer of the window.
		UTILAPI void swapBuffers() override;

		//! @see SDL_GL_GetAttribute
		UTILAPI int32_t getSwapInterval() const override;

		//! Set the cursor to the given location inside the window.
		UTILAPI void warpCursor(int x, int y) override;

		//! Grap control of mouse and keyboard input.
		UTILAPI void grabInput() override;

		//! Release control of mouse and keyboard input.
		UTILAPI void ungrabInput() override;

		//! Return any pending events for this window.
		UTILAPI std::deque<Event> fetchEvents() override;

		//! Set an image that is shown as the window's icon.
		UTILAPI void setIcon(const Bitmap & icon) override;

		//! Read text from the clipboard.
		UTILAPI std::string getClipboardText() const override;

		//! Write text to the clipboard.
		UTILAPI void setClipboardText(const std::string & text) override;
		
		//! Enables rendering to this window.
		UTILAPI virtual void makeCurrent() override;
	private:
		//! Create a new window.
		UTILAPI WindowGLFW(const Properties & properties);

		//! Allow access to constructor from factory.
		UTILAPI friend std::unique_ptr<Window> createWindow(const Properties & properties);

		//! ---|> Window
		UTILAPI void doSetCursor(const Cursor * cursor) override;
		//! ---|> Window
		UTILAPI void doHideCursor() override;
		
		//! Internal data.
		std::unique_ptr<WindowGLFWInternal> data;

};

}
}

#endif /* WINDOWGLFW_H_ */

#endif /* defined(UTIL_HAVE_LIB_GLFW) */
