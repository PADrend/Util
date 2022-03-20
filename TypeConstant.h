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

#include "extern/half.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace Util {

//! @addtogroup util_helper
//! @{

//! This constants should not change and may be used for serialization.
//! @note deprecated! Use BaseType instead.
enum class TypeConstant : uint8_t {
	UINT8	 = 0,
	UINT16 = 1,
	UINT32 = 2,
	UINT64 = 3,
	INT8	 = 4,
	INT16	 = 5,
	INT32	 = 6,
	INT64	 = 7,
	FLOAT	 = 8,
	DOUBLE = 9,
	HALF	 = 10,
	BOOL	 = 11,
};

//---------------------

/**
 * @brief Represents a primitive base type.
 * The value of the enums encode the size in byte and if it is signed/unsigned or float.
 * | 4: unused | 1: signed | 1: float| 2: size|
 */
enum class BaseType : uint8_t {
	UInt8			= 0b00'00,
	Int8			= 0b10'00,
	UInt16		= 0b00'01,
	Int16			= 0b10'01,
	UInt32		= 0b00'10,
	Int32			= 0b10'10,
	UInt64		= 0b00'11,
	Int64			= 0b10'11,
	Float16		= 0b11'01,
	Float32		= 0b11'10,
	Float64		= 0b11'11,
	Undefined = 0xff,
};

//---------------------

constexpr BaseType toBaseType(TypeConstant t) {
	switch (t) {
		case TypeConstant::UINT8: return BaseType::UInt8;
		case TypeConstant::UINT16: return BaseType::UInt16;
		case TypeConstant::UINT32: return BaseType::UInt32;
		case TypeConstant::UINT64: return BaseType::UInt64;
		case TypeConstant::INT8: return BaseType::Int8;
		case TypeConstant::INT16: return BaseType::Int16;
		case TypeConstant::INT32: return BaseType::Int32;
		case TypeConstant::INT64: return BaseType::Int64;
		case TypeConstant::FLOAT: return BaseType::Float16;
		case TypeConstant::DOUBLE: return BaseType::Float32;
		case TypeConstant::HALF: return BaseType::Float64;
		case TypeConstant::BOOL: return BaseType::UInt8;
		default: return BaseType::Undefined;
	}
}

//---------------------

//! returns the size in bytes of the given type.
constexpr uint8_t getNumBytes(BaseType t) {
	// some stupid bit twiddling to avoid 'if(t==Undefined)', just because
	return (1 << (static_cast<uint8_t>(t) & 0b11)) & ~(static_cast<uint8_t>(t) >> 4);
}
constexpr uint8_t getNumBytes(TypeConstant t) {
	return getNumBytes(toBaseType(t));
}

//---------------------

//! returns a string representation of the given type.
UTILAPI std::string getTypeString(BaseType t);
inline std::string getTypeString(TypeConstant t) {
	return getTypeString(toBaseType(t));
}

//---------------------

//! checks if the given type is a floating-point type.
constexpr bool isFloatType(BaseType t) {
	return (static_cast<uint8_t>(t) & 0b1111'01'00) == 0b0000'01'00;
}
inline bool isFloatType(TypeConstant t) {
	isFloatType(toBaseType(t));
}

//---------------------

//! checks if the given type is a signed integer type.
constexpr bool isSignedIntegerType(BaseType t) {
	return (static_cast<uint8_t>(t) & 0b1111'11'00) == 0b0000'10'00;
}
inline bool isSignedIntegerType(TypeConstant t) {
	isSignedIntegerType(toBaseType(t));
}

//---------------------

//! checks if the given type is an unsigned integer type.
constexpr bool isUnsignedIntegerType(BaseType t) {
	return (static_cast<uint8_t>(t) & 0b1111'11'00) == 0b0000'00'00;
}
inline bool isUnsignedIntegerType(TypeConstant t) {
	isUnsignedIntegerType(toBaseType(t));
}

//---------------------

//! gets the type constant based on byte-size and if it is signed or float.
constexpr BaseType getBaseType(uint32_t numBytes, bool isSigned, bool isFloat) {
	// this could probably be done with bit twiddling, but this is (slightly) more readable.
	if (numBytes == 1) {
		return isFloat ? BaseType::Undefined : (isSigned ? BaseType::Int8 : BaseType::UInt8);
	} else if (numBytes == 2) {
		return isFloat ? (isSigned ? BaseType::Float16 : BaseType::Undefined)
									 : (isSigned ? BaseType::Int16 : BaseType::UInt16);
	} else if (numBytes == 4) {
		return isFloat ? (isSigned ? BaseType::Float32 : BaseType::Undefined)
									 : (isSigned ? BaseType::Int32 : BaseType::UInt32);
	} else if (numBytes == 8) {
		return isFloat ? (isSigned ? BaseType::Float64 : BaseType::Undefined)
									 : (isSigned ? BaseType::Int64 : BaseType::UInt64);
	}
	return BaseType::Undefined;
}

//! gets the type constant for the given template type.
template<typename T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, bool> = true>
constexpr BaseType getBaseType() {
	return getBaseType(sizeof(T), false, false);
}

//! gets the type constant for the given template type.
template<typename T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, bool> = true>
constexpr BaseType getBaseType() {
	return getBaseType(sizeof(T), true, false);
}

//! gets the type constant for the given template type.
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
constexpr BaseType getBaseType() {
	return getBaseType(sizeof(T), true, true);
}

//! gets the type constant for the given template type.
template<typename T, std::enable_if_t<std::is_same_v<T, half_t>, bool> = true>
constexpr BaseType getBaseType() {
	return BaseType::Float16;
}

//---------------------

//! @}

} // namespace Util

#endif // TYPE_CONSTANT_H
