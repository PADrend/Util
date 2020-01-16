/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef PIXELACCESSOR_H_
#define PIXELACCESSOR_H_

#include "Bitmap.h"
#include "Color.h"
#include "PixelFormat.h"
#include "../Macros.h"
#include "../ReferenceCounter.h"
#include "../References.h"
#include <cstdint>

namespace Util {
class AttributeAccessor;

/*! Class for direct access to the pixels of a Bitmap.

	\note After structural modifications of a Bitmap (e.g. changes of the size or the PixelFormat)
		all corresponding PixelAccessors become invalid and may no longer be used.
	\note A PixelAccessor keeps a Reference to the associated Bitmap. If there exists no other reference to
		the Bitmap, it will be deleted if the PixelAccessor is deleted.
	@ingroup graphics
*/
class PixelAccessor : public ReferenceCounter<PixelAccessor> {
	private:
		Reference<Bitmap> myBitmap;
	protected:
		bool checkRange(uint32_t x,uint32_t y) const { return x<myBitmap->getWidth() && y<myBitmap->getHeight(); }
		bool crop(uint32_t & x,uint32_t & y,uint32_t & width,uint32_t & height) const;
		inline uint32_t getIndex(uint32_t x,uint32_t y) const { return (y * myBitmap->getWidth() + x); }

		PixelAccessor(Reference<Bitmap> bitmap) :
			ReferenceCounter<PixelAccessor>(),
			myBitmap(std::move(bitmap)) {
		}
	public:
		using Ref = Reference<PixelAccessor>;
		/*! Create a PixelAccessor for the given bitmap. According to the format of the Bitmap,
			an appropriate Accesor-type is chosen. If no accessor is available due to an unsupported
			format, nullptr is returned.	*/
		static Ref create(Reference<Bitmap> bitmap);

		virtual ~PixelAccessor() = default;

		const AttributeFormat& getPixelFormat() const { return myBitmap->getPixelFormat(); }
		const Reference<Bitmap>& getBitmap() const { return myBitmap; }
		uint32_t getWidth() const { return myBitmap->getWidth(); }
		uint32_t getHeight() const { return myBitmap->getHeight(); }

		inline Color4f readColor4f(uint32_t x, uint32_t y) const;
		inline Color4ub readColor4ub(uint32_t x, uint32_t y) const;
		//! Retrieve a single value from the bitmap (a value from the red channel for most bitmaps).
		inline float readSingleValueFloat(uint32_t x, uint32_t y) const;
		//! Retrieve a single value from the bitmap (a value from the red channel for most bitmaps).
		inline uint8_t readSingleValueByte(uint32_t x, uint32_t y) const;
		inline void writeColor(uint32_t x, uint32_t y, const Color4f & c);
		inline void writeColor(uint32_t x, uint32_t y, const Color4ub & c);
		//! Write a single value to the bitmap (e.g., a value to the red channel for monochrome bitmaps).
		inline void writeSingleValueFloat(uint32_t x, uint32_t y, float value);

		/*! Fill the given area with the given color.
			\note Specific PixelAccessors may provide an optimized implementation */
		void fill(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color4f & c){
			if(crop(x,y,width,height))
				doFill(x,y,width,height,c);
		}

		/*! copies all pixels from source to dest
		 *  if source and dest have incompatible dimensions only those pixels are copied which are valid in both
		 */
		static void copy(PixelAccessor * source, PixelAccessor * dest);

		/*! Direct access to the pixel data.
			\note Be careful: No boundary checks are performed! */
		template<typename _T> _T * _ptr(const uint32_t x, const uint32_t y){
			return reinterpret_cast<_T*>( myBitmap->data() + getIndex(x,y) * myBitmap->getPixelFormat().getDataSize() );
		}
		template<typename _T> const _T * _ptr(const uint32_t x, const uint32_t y) const{
			return reinterpret_cast<_T*>( myBitmap->data() + getIndex(x,y) * myBitmap->getPixelFormat().getDataSize() );
		}
	private:
		//! ---o
		virtual Color4f doReadColor4f(uint32_t x, uint32_t y) const = 0;

		//! ---o
		virtual Color4ub doReadColor4ub(uint32_t x, uint32_t y) const = 0;

		//! ---o
		virtual float doReadSingleValueFloat(uint32_t x, uint32_t y) const = 0;

		//! ---o
		virtual uint8_t doReadSingleValueByte(uint32_t x, uint32_t y) const = 0;

		//! ---o
		virtual void doWriteColor(uint32_t x, uint32_t y, const Color4f & c) = 0;

		//! ---o
		virtual void doWriteColor(uint32_t x, uint32_t y, const Color4ub & c) = 0;

		//! ---o
		virtual void doWriteSingleValueFloat(uint32_t x, uint32_t y, float value) = 0;

		//! ---o
		virtual void doFill(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color4f & c);
};

// -----------------------------------------

inline Color4f PixelAccessor::readColor4f(uint32_t x, uint32_t y) const {
	if(checkRange(x,y))
		return doReadColor4f(x,y);
	WARN("readColor4f: out of range");
	return Color4f();
}

inline Color4ub PixelAccessor::readColor4ub(uint32_t x, uint32_t y) const {
	if(checkRange(x,y))
		return doReadColor4ub(x,y);
	WARN("readColor4ub: out of range");
	return Color4ub();
}

inline float PixelAccessor::readSingleValueFloat(uint32_t x, uint32_t y) const {
	if(checkRange(x, y)) {
		return doReadSingleValueFloat(x, y);
	}
	WARN("readSingleValueFloat: out of range");
	return 0.0f;
}

inline uint8_t PixelAccessor::readSingleValueByte(uint32_t x, uint32_t y) const {
	if(checkRange(x, y)) {
		return doReadSingleValueByte(x, y);
	}
	WARN("readSingleValueByte: out of range");
	return 0;
}

inline void PixelAccessor::writeColor(uint32_t x, uint32_t y, const Color4f & c) {
	if(checkRange(x,y))
		doWriteColor(x,y,c);
	else
		WARN("writeColor: out of range");
}

inline void PixelAccessor::writeColor(uint32_t x, uint32_t y, const Color4ub & c) {
	if(checkRange(x,y))
		doWriteColor(x,y,c);
	else
		WARN("writeColor: out of range");
}

inline void PixelAccessor::writeSingleValueFloat(uint32_t x, uint32_t y, float value) {
	if(checkRange(x, y)) {
		doWriteSingleValueFloat(x, y, value);
	} else {
		WARN("writeSingleValueFloat: out of range");
	}
}

}

#endif /* PIXELACCESSOR_H_ */
