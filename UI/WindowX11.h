/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11)

#ifndef WINDOWX11_H_
#define WINDOWX11_H_

#include "Window.h"
#include <memory>

namespace Util {
namespace UI {
struct WindowX11Data;

/**
 * Class to create an X11 window.
 *
 * @author Benjamin Eikel
 * @date 2012-10-08
 */
class WindowX11 : public Window {
	public:
		//! Destroy the window and free the allocated resources.
		virtual ~WindowX11();

		//! Set the cursor to the given location inside the window.
		void warpCursor(int x, int y) override;

		//! Grap control of mouse and keyboard input.
		void grabInput() override;

		//! Release control of mouse and keyboard input.
		void ungrabInput() override;

		//! Return any pending events for this window.
		std::deque<Event> fetchEvents() override;

		//! Set an image that is shown as the window's icon.
		void setIcon(const Reference<Bitmap> & icon) override;

		//! Read text from an internal string.
		std::string getClipboardText() const override;

		//! Write text to an internal string.
		void setClipboardText(const std::string & text) override;

	protected:
		//! Create a new window.
		WindowX11(const Properties & properties);

		//! Storage of attributes.
		std::unique_ptr<WindowX11Data> x11Data;

	private:
		//! Show the given cursor inside the window.
		void doSetCursor(UI::Cursor * cursor) override;

		//! Hide the cursor inside the window.
		void doHideCursor() override;
};

}
}

#endif /* WINDOWX11_H_ */

#endif /* defined(UTIL_HAVE_LIB_X11) */
