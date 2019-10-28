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
#ifndef UTIL_PIXELFORMAT_H
#define UTIL_PIXELFORMAT_H

#include "../Resources/ResourceFormat.h"
#include "../TypeConstant.h"

#include <cstdint>

namespace Util {

/*!	Representation of the pixel data format.
\note if a color component (r,g,b or a) is not set, its byteOffset is set to the constant NONE. 
@ingroup graphics */
class PixelFormat : public ResourceFormat {
private:
	uint8_t byteOffset_r, byteOffset_g, byteOffset_b, byteOffset_a;
	
	// make append functions private
	using ResourceFormat::appendAttribute;
	using ResourceFormat::appendFloat;
	using ResourceFormat::appendInt;
	using ResourceFormat::appendUInt;
	using ResourceFormat::updateAttribute;
public:
	
	static const uint8_t NONE = 0xff;
	static const StringIdentifier COLOR;

	PixelFormat(TypeConstant valueType, uint8_t _byteOffset_r, uint8_t _byteOffset_g, uint8_t _byteOffset_b, uint8_t _byteOffset_a=NONE) : ResourceFormat(), 
		byteOffset_r(_byteOffset_r), byteOffset_g(_byteOffset_g), byteOffset_b(_byteOffset_b), byteOffset_a(_byteOffset_a) {
		appendAttribute(COLOR,valueType,(_byteOffset_r==NONE ? 0 : 1) + (_byteOffset_g==NONE ? 0 : 1) + (_byteOffset_b==NONE ? 0 : 1) + (_byteOffset_a==NONE ? 0 : 1));
	}

	uint8_t getByteOffset_r() const { return byteOffset_r; }
	uint8_t getByteOffset_g() const { return byteOffset_g; }
	uint8_t getByteOffset_b() const { return byteOffset_b; }
	uint8_t getByteOffset_a() const { return byteOffset_a; }
	uint16_t getBytesPerPixel() const { return getSize(); }
	uint8_t getNumComponents() const { return getAttributes().front().getNumValues(); }
	uint8_t getBytesPerComponent() const { return getNumBytes(getValueType()); }
	TypeConstant getValueType() const { return static_cast<TypeConstant>(getAttributes().front().getDataType()); }
	bool isValid() const { return !getAttributes().front().empty(); }

	// ---------------------------------
	// default pixel formats (assuming little endianess)
	static const PixelFormat RGB;			// 0x00B_G_R_
	static const PixelFormat RGB_FLOAT;		// 0xR_______G_______B_______
	static const PixelFormat BGR;			// 0x00R_G_B_
	static const PixelFormat BGR_FLOAT;		// 0xB_______G_______R_______
	static const PixelFormat RGBA;			// 0xA_B_G_R_
	static const PixelFormat RGBA_FLOAT;	// 0xR_______G_______B_______A_______
	static const PixelFormat BGRA;			// 0xA_R_G_B_
	static const PixelFormat BGRA_FLOAT;	// 0xB_______G_______R_______A_______
	static const PixelFormat MONO;			// 0xR_
	static const PixelFormat MONO_FLOAT;	// 0xR_______
	static const PixelFormat UNKNOWN;		// numComponents is 0. No direct pixel access is possible.
};

}

#endif /* UTIL_PIXELFORMAT_H */
