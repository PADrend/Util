/*
	This file is part of the Util library.
	Copyright (C) 2014 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_XCB) and defined(UTIL_HAVE_LIB_XCB_KEYSYMS) and defined(UTIL_HAVE_LIB_EGL)

#ifndef UTIL_UI_WINDOWXCB_H
#define UTIL_UI_WINDOWXCB_H

#include "Window.h"
#include <memory>
#include <string>

struct SDL_Cursor;
struct SDL_Window;
typedef void * SDL_GLContext;

namespace Util {
namespace UI {

/**
 * Class to create a window with an embedded OpenGL rendering context by using
 * XCB and EGL.
 *
 * @author Benjamin Eikel
 * @date 2014-04-26
 */
class WindowXCB : public Window {
	public:
		//! Destroy the window and free the allocated resources.
		virtual ~WindowXCB();

		//! Swap front and back buffer of the window.
		void swapBuffers() override;

		//! Set the cursor to the given location inside the window.
		void warpCursor(int x, int y) override;

		//! Grap control of mouse and keyboard input.
		void grabInput() override;

		//! Release control of mouse and keyboard input.
		void ungrabInput() override;

		//! Return any pending events for this window.
		std::deque<Event> fetchEvents() override;

		//! Set an image that is shown as the window's icon.
		void setIcon(const Bitmap & icon) override;

		//! Read text from the clipboard.
		std::string getClipboardText() const override;

		//! Write text to the clipboard.
		void setClipboardText(const std::string & text) override;

	private:
		//! Create a new window.
		WindowXCB(const Properties & properties);

		//! Allow access to constructor from factory.
		friend std::unique_ptr<Window> createWindow(const Properties & properties);

		void doSetCursor(const Cursor * cursor) override;
		void doHideCursor() override;

		//! Structure used to decouple declaration and definition of attributes.
		struct WindowXCBData;

		//! Storage of attributes for XCB.
		std::unique_ptr<WindowXCBData> xcbData;

		//! Structure used to decouple declaration and definition of attributes.
		struct WindowEGLData;

		//! Storage of attributes for EGL.
		std::unique_ptr<WindowEGLData> eglData;

		//! Storage of clipboard contents.
		std::string clipboard;
};

}
}

#endif /* UTIL_UI_WINDOWXCB_H */

#endif /* defined(UTIL_HAVE_LIB_XCB) and defined(UTIL_HAVE_LIB_XCB_KEYSYMS) and defined(UTIL_HAVE_LIB_EGL) */
