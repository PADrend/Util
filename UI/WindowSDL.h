/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_SDL2)

#ifndef WINDOWSDL_H_
#define WINDOWSDL_H_

#include "Window.h"
#include <memory>

struct SDL_Cursor;
struct SDL_Window;
typedef void * SDL_GLContext;

namespace Util {
namespace UI {

/**
 * Class to create a window with an embedded OpenGL rendering context by using SDL.
 *
 * @author Benjamin Eikel
 * @date 2010-07-20
 */
class WindowSDL : public Window {
	public:
		//! Destroy the window and free the allocated resources.
		virtual ~WindowSDL();

		//! Swap front and back buffer of the window.
		void swapBuffers() override;

		//! @see SDL_GL_GetAttribute
		int32_t getSwapInterval() const override;

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
		
		//! Enables rendering to this window.
		virtual void makeCurrent() override;
	private:
		//! Create a new window.
		WindowSDL(const Properties & properties);

		//! Allow access to constructor from factory.
		friend std::unique_ptr<Window> createWindow(const Properties & properties);

		//! ---|> Window
		void doSetCursor(const Cursor * cursor) override;
		//! ---|> Window
		void doHideCursor() override;

		//! Internal handle to the window.
		SDL_Window * sdlWindow;

		//! Internal handle to the GL context.
		SDL_GLContext sdlGlContext;

		//! Internal handle to the current cursor.
		SDL_Cursor * sdlCursor;
};

}
}

#endif /* WINDOWSDL_H_ */

#endif /* defined(UTIL_HAVE_LIB_SDL2) */
