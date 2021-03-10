/*
	This file is part of the Util library.
	Copyright (C) 2014 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef TYPE_CONSTANT_H
#define TYPE_CONSTANT_H

#include <cstddef>
#include <cstdint>
#include <string>

namespace Util {

//! @addtogroup util_helper
//! @{
	
//! This constants should not change and may be used for serialization.
enum class TypeConstant : uint8_t{
	UINT8	= 0,
	UINT16	= 1,
	UINT32	= 2,
	UINT64	= 3,
	INT8	= 4,
	INT16	= 5,
	INT32	= 6,
	INT64	= 7,
	FLOAT	= 8,
	DOUBLE = 9,
	HALF = 10,
	BOOL = 11
};

//---------------------

UTILAPI uint8_t getNumBytes(TypeConstant t);

//---------------------

UTILAPI std::string getTypeString(TypeConstant t);

//---------------------

inline bool isFloatType(Util::TypeConstant type) {
	switch(type) {
		case Util::TypeConstant::FLOAT:
		case Util::TypeConstant::DOUBLE:
		case Util::TypeConstant::HALF:
			return true;
		default: return false;
	}
}

//---------------------

inline bool isSignedIntegerType(Util::TypeConstant type) {
	switch(type) {
		case Util::TypeConstant::INT8:
		case Util::TypeConstant::INT16:
		case Util::TypeConstant::INT32:
		case Util::TypeConstant::INT64:
			return true;
		default: return false;
	}
}

//---------------------

inline bool isUnsignedIntegerType(Util::TypeConstant type) {
	switch(type) {
		case Util::TypeConstant::UINT8:
		case Util::TypeConstant::UINT16:
		case Util::TypeConstant::UINT32:
		case Util::TypeConstant::UINT64:
			return true;
		default: return false;
	}
}

//! @}
	
}

#endif // TYPE_CONSTANT_H
