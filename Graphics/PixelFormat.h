/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_PIXELFORMAT_H
#define UTIL_PIXELFORMAT_H

#include "../Resources/ResourceFormat.h"
#include "../TypeConstant.h"
#include "../Utils.h"

#include <cstdint>

namespace Util {

/*!	Representation of the pixel data format.
\note if a color component (r,g,b or a) is not set, its byteOffset is set to the constant NONE. 
@ingroup graphics */
namespace PixelFormat {

	// ---------------------------------
	// default pixel formats
	extern const AttributeFormat RG;			// 0xR_G_
	extern const AttributeFormat RG_FLOAT;		// 0xR_______G_______
	extern const AttributeFormat RG_INT32;		// 0xR_______G_______
	extern const AttributeFormat RG_UINT32;		// 0xR_______G_______
	extern const AttributeFormat RGB;			// 0x00B_G_R_
	extern const AttributeFormat RGB_FLOAT;		// 0xR_______G_______B_______
	extern const AttributeFormat RGB_INT32;		// 0xR_______G_______B_______
	extern const AttributeFormat RGB_UINT32;		// 0xR_______G_______B_______
	extern const AttributeFormat BGR;			// 0x00R_G_B_
	extern const AttributeFormat BGR_FLOAT;		// 0xB_______G_______R_______
	extern const AttributeFormat BGR_INT32;		// 0xB_______G_______R_______
	extern const AttributeFormat BGR_UINT32;		// 0xB_______G_______R_______
	extern const AttributeFormat RGBA;			// 0xA_B_G_R_
	extern const AttributeFormat RGBA_FLOAT;	// 0xR_______G_______B_______A_______
	extern const AttributeFormat RGBA_INT32;	// 0xR_______G_______B_______A_______
	extern const AttributeFormat RGBA_UINT32;	// 0xR_______G_______B_______A_______
	extern const AttributeFormat BGRA;			// 0xA_R_G_B_
	extern const AttributeFormat BGRA_FLOAT;	// 0xB_______G_______R_______A_______
	extern const AttributeFormat BGRA_INT32;	// 0xB_______G_______R_______A_______
	extern const AttributeFormat BGRA_UINT32;	// 0xB_______G_______R_______A_______
	extern const AttributeFormat MONO;			// 0xR_
	extern const AttributeFormat MONO_FLOAT;	// 0xR_______
	extern const AttributeFormat MONO_INT32;	// 0xR_______
	extern const AttributeFormat MONO_UINT32;	// 0xR_______
	extern const AttributeFormat R11G11B10_FLOAT;	
	extern const AttributeFormat UNKNOWN;		// numComponents is 0. No direct pixel access is possible.

	
	//! Internal type identifiers for special pixel formats
	enum InternalType_t : uint32_t {
		INTERNAL_TYPE_R11G11B10_FLOAT = hash32("R11G11B10_FLOAT"),
		INTERNAL_TYPE_BGRA = hash32("BGRA"),
	};
};

}

#endif /* UTIL_PIXELFORMAT_H */
