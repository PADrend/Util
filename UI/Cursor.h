/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef UTIL_CURSOR_H
#define UTIL_CURSOR_H

#include "../References.h"
#include <memory>

namespace Util {
class Bitmap;
namespace UI {

/**
 * Class representing the information needed to create an 
 * implementation-specific cursor.
 *
 * @author Benjamin Eikel
 * @date 2012-09-27
 */
class Cursor {
	private:
		Reference<Bitmap> _image;

		unsigned int _hotSpotX;
		unsigned int _hotSpotY;

		//! Store the data of a new cursor.
		Cursor(Reference<Bitmap> image, unsigned int hotSpotX, unsigned int hotSpotY);

		//! Allow access to members from factory.
		friend std::unique_ptr<Cursor> createCursor(const Reference<Bitmap> & image, unsigned int hotSpotX, unsigned int hotSpotY);
	public:
		Cursor(const Cursor &) = delete;
		Cursor(Cursor &&) = delete;
		Cursor & operator=(const Cursor &) = delete;
		Cursor & operator=(Cursor &&) = delete;

		//! Free the associated data.
		~Cursor();

		const Reference<Bitmap> & getBitmap() const {
			return _image;
		}

		unsigned int getHotSpotX() const {
			return _hotSpotX;
		}

		unsigned int getHotSpotY() const {
			return _hotSpotY;
		}
};

}
}

#endif /* UTIL_CURSOR_H */
