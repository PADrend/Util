/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef WINDOW_H_
#define WINDOW_H_

#include "Event.h"
#include "../ReferenceCounter.h"
#include <deque>
#include <memory>
#include <string>
#include <vector>

// forward declaration of vulkan handles
typedef struct VkInstance_T* VkInstance;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Util {
class Bitmap;
namespace UI {
class Cursor;

using APIHandle = VkInstance;
using Surface = VkSurfaceKHR;
	
/**
 * Abstract base class for all windows.
 *
 * @author Benjamin Eikel
 * @date 2010-06-28
 */
class Window : public Util::ReferenceCounter<Window> {
	public:
		//! The Property struct is used to parameterize the creation of a window.
		struct Properties{
			bool	borderless,				//! The window should not have a border.
					debug,					//! Create a rendering context with debugging support.
					compatibilityProfile,	//! Create a rendering context with a compatibility profile.
					fullscreen,				//! Create a fullscreen window.
					multisampled,			//! Create a rendering context with a multisample buffer.
					positioned,				//! The window's location is determined by the given posX,posY value.
					resizable,				//! The window should be resizable.
					shareContext;			//! Share the window's gl context if there is already one available.
			uint32_t clientAreaWidth,clientAreaHeight;	//! The size of the client area of the window.
			int32_t posX,posY;				//! (if positioned == true) The position of the upper left corner of the window 
			uint32_t multisamples;			//! (if multisampled == true) Number of samples used for multisampling; default is 4.
			uint32_t contextVersionMajor; //! The major rendering API version that the created context must be compatible with.
			uint32_t contextVersionMinor; //! The minor rendering API version that the created context must be compatible with.

			std::string title;				//! The title string commonly displayed in the title bar of the window.
			enum RenderingAPI {
				GL_ES_1, //!< OpenGL ES 1
				GL_ES_2, //!< OpenGL ES 2
				GL_ES_3, //!< OpenGL ES 3
				GL, //!< OpenGL (all versions)
				VULKAN //!< Vulkan
			} renderingAPI;
			Properties() : borderless(false),debug(false),compatibilityProfile(false),
					fullscreen(false),multisampled(false),positioned(false),resizable(false),
					clientAreaWidth(0),clientAreaHeight(0),posX(0),posY(0),multisamples(4),
					contextVersionMajor(1), contextVersionMinor(0), renderingAPI(VULKAN){}
		};
		
		//! Destroy the window and free the allocated resources.
		virtual ~Window() {
		}

		//! Return any pending events for this window.
		virtual std::deque<Event> fetchEvents() = 0;
		
		//! Return the height of the client area of the window.
		uint32_t getHeight() const {
			return height;
		}
		
		//! Return the width of the client area of the window.
		uint32_t getWidth() const {
			return width;
		}

		//! Swap front and back buffer of the window.
		virtual void swapBuffers() = 0;

		/**
		 * Return the swap interval for the window (known as "sync to vblank",
		 * or "vsync").
		 * 
		 * @return A value of @c -1 indicates that the window implementation
		 * does not support the query. A value of @c 0 means that the buffer
		 * swaps are not synchronized. A value greater than zero means that the
		 * buffer is swapped only after this number of video frames.
		 */
		virtual int32_t getSwapInterval() const {
			return -1;
		}

		//! Grap control of mouse and keyboard input.
		virtual void grabInput() = 0;

		//! Release control of mouse and keyboard input.
		virtual void ungrabInput() = 0;

		//! Set an image that is shown as the window's icon.
		virtual void setIcon(const Bitmap & icon) = 0;

		/**
		 * Read text from the clipboard.
		 * 
		 * @return String that was stored in the clipboard, or an empty string if the
		 * clipboard was empty.
		 */
		virtual std::string getClipboardText() const = 0;

		/**
		 * Write the given text to the clipboard.
		 * 
		 * @param text String that is written to the clipboard.
		 */
		virtual void setClipboardText(const std::string & text) = 0;
		
		//! Enables rendering to this window.
		virtual void makeCurrent() = 0;
		
		//! Returns the properties, the window was created with.
		const Properties& getProperties() const { return properties; }

		/**
		 * creates a Surface handle to render to.
		 * 
		 * @param apiHandle the API handle (e.g., VkInstance for Vulkan)
		 * @return The Surface handle
		 */
		virtual Surface createSurface(APIHandle apiHandle) = 0;
		
		/**
		 * Returns the extensions needed to create a surface from an API handle.
		 * 
		 * @return A list of extension names.
		 */
		virtual std::vector<const char*> getAPIExtensions() = 0;
	protected:
		//! Stores the size of the window's client area.
		uint32_t width,height;
		bool shareContext;
		Properties properties;

		/**
		 * Create the window and initialize a rendering context.
		 */
		Window(const Properties & properties) :
			 width(properties.clientAreaWidth), height(properties.clientAreaHeight),shareContext(properties.shareContext),properties(properties),cursorHidden(false),activeCursor(nullptr) {
		}

		//! Allow access to members from factory.
		friend Util::Reference<Window> createWindow(const Properties & properties);

	private:

		Window(const Window &) = delete;
		Window(Window &&) = delete;
		Window & operator=(const Window &) = delete;
		Window & operator=(Window &&) = delete;

		
	// ----------------------------------------------
	//! @name Cursor
	// @{
	public:
		//!	Get the current cursor.
		const std::shared_ptr<Cursor> & getCursor()const{
			return activeCursor;
		}
		//! Hide the cursor.
		void hideCursor(){
			if(!cursorHidden){
				cursorHidden=true;
				doHideCursor();
			}
		}
		/*! Set the given cursor inside the window (only takes effect, when the cursor is visible)
			\note if _cursor is nullptr, the system's default cursor is enabled. */
		void setCursor(std::shared_ptr<Cursor> _cursor){
			activeCursor = std::move(_cursor);
			if(!cursorHidden)
				doSetCursor(activeCursor.get());
		}
		//! Show the cursor.
		void showCursor(){
			if(cursorHidden){
				cursorHidden=false;
				doSetCursor(activeCursor.get());
			}
		}
		//! Set the cursor to the given location inside the window.
		virtual void warpCursor(int x, int y) = 0;
	private:
		virtual void doHideCursor() = 0;
		virtual void doSetCursor(const Cursor * _cursor) = 0;

		bool cursorHidden;
		std::shared_ptr<Cursor> activeCursor;
	// @}
		
};

}
}

#endif /* WINDOW_H_ */
