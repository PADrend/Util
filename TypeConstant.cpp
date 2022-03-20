/*
	This file is part of the Util library.
	Copyright (C) 2014 Claudius Jähn <claudius@uni-paderborn.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "TypeConstant.h"
#include <stdexcept>

namespace Util {

std::string getTypeString(BaseType t) {
	switch (t) {
		case BaseType::UInt8: return "uint8";
		case BaseType::Int8: return "int8";
		case BaseType::UInt16: return "uint16";
		case BaseType::Int16: return "int16";
		case BaseType::UInt32: return "uint32";
		case BaseType::Int32: return "int32";
		case BaseType::UInt64: return "uint64";
		case BaseType::Int64: return "int64";
		case BaseType::Float16: return "half";
		case BaseType::Float32: return "float";
		case BaseType::Float64: return "double";
		default: return "";
	}
}

//=====================================================================
// static tests

static_assert(getBaseType(0, false, false) == BaseType::Undefined);
static_assert(getBaseType(0, false, true) == BaseType::Undefined);
static_assert(getBaseType(0, true, false) == BaseType::Undefined);
static_assert(getBaseType(0, true, true) == BaseType::Undefined);

static_assert(getBaseType(1, false, false) == BaseType::UInt8);
static_assert(getBaseType(1, false, true) == BaseType::Undefined);
static_assert(getBaseType(1, true, false) == BaseType::Int8);
static_assert(getBaseType(1, true, true) == BaseType::Undefined);

static_assert(getBaseType(2, false, false) == BaseType::UInt16);
static_assert(getBaseType(2, false, true) == BaseType::Undefined);
static_assert(getBaseType(2, true, false) == BaseType::Int16);
static_assert(getBaseType(2, true, true) == BaseType::Float16);

static_assert(getBaseType(3, false, false) == BaseType::Undefined);
static_assert(getBaseType(3, false, true) == BaseType::Undefined);
static_assert(getBaseType(3, true, false) == BaseType::Undefined);
static_assert(getBaseType(3, true, true) == BaseType::Undefined);

static_assert(getBaseType(4, false, false) == BaseType::UInt32);
static_assert(getBaseType(4, false, true) == BaseType::Undefined);
static_assert(getBaseType(4, true, false) == BaseType::Int32);
static_assert(getBaseType(4, true, true) == BaseType::Float32);

static_assert(getBaseType(8, false, false) == BaseType::UInt64);
static_assert(getBaseType(8, false, true) == BaseType::Undefined);
static_assert(getBaseType(8, true, false) == BaseType::Int64);
static_assert(getBaseType(8, true, true) == BaseType::Float64);

static_assert(getNumBytes(BaseType::UInt8) == 1);
static_assert(getNumBytes(BaseType::Int8) == 1);
static_assert(getNumBytes(BaseType::UInt16) == 2);
static_assert(getNumBytes(BaseType::Int16) == 2);
static_assert(getNumBytes(BaseType::UInt32) == 4);
static_assert(getNumBytes(BaseType::Int32) == 4);
static_assert(getNumBytes(BaseType::UInt64) == 8);
static_assert(getNumBytes(BaseType::Int64) == 8);
static_assert(getNumBytes(BaseType::Float16) == 2);
static_assert(getNumBytes(BaseType::Float32) == 4);
static_assert(getNumBytes(BaseType::Float64) == 8);
static_assert(getNumBytes(BaseType::Undefined) == 0);

static_assert(getBaseType<uint8_t>() == BaseType::UInt8);
static_assert(getBaseType<uint16_t>() == BaseType::UInt16);
static_assert(getBaseType<uint32_t>() == BaseType::UInt32);
static_assert(getBaseType<uint64_t>() == BaseType::UInt64);
static_assert(getBaseType<int8_t>() == BaseType::Int8);
static_assert(getBaseType<int16_t>() == BaseType::Int16);
static_assert(getBaseType<int32_t>() == BaseType::Int32);
static_assert(getBaseType<int64_t>() == BaseType::Int64);
static_assert(getBaseType<half_t>() == BaseType::Float16);
static_assert(getBaseType<float>() == BaseType::Float32);
static_assert(getBaseType<double>() == BaseType::Float64);

static_assert(!isFloatType(BaseType::UInt8));
static_assert(!isFloatType(BaseType::Int8));
static_assert(!isFloatType(BaseType::UInt16));
static_assert(!isFloatType(BaseType::Int16));
static_assert(!isFloatType(BaseType::UInt32));
static_assert(!isFloatType(BaseType::Int32));
static_assert(!isFloatType(BaseType::UInt64));
static_assert(!isFloatType(BaseType::Int64));
static_assert(isFloatType(BaseType::Float16));
static_assert(isFloatType(BaseType::Float32));
static_assert(isFloatType(BaseType::Float64));
static_assert(!isFloatType(BaseType::Undefined));

static_assert(!isSignedIntegerType(BaseType::UInt8));
static_assert(isSignedIntegerType(BaseType::Int8));
static_assert(!isSignedIntegerType(BaseType::UInt16));
static_assert(isSignedIntegerType(BaseType::Int16));
static_assert(!isSignedIntegerType(BaseType::UInt32));
static_assert(isSignedIntegerType(BaseType::Int32));
static_assert(!isSignedIntegerType(BaseType::UInt64));
static_assert(isSignedIntegerType(BaseType::Int64));
static_assert(!isSignedIntegerType(BaseType::Float16));
static_assert(!isSignedIntegerType(BaseType::Float32));
static_assert(!isSignedIntegerType(BaseType::Float64));
static_assert(!isSignedIntegerType(BaseType::Undefined));

static_assert(isUnsignedIntegerType(BaseType::UInt8));
static_assert(!isUnsignedIntegerType(BaseType::Int8));
static_assert(isUnsignedIntegerType(BaseType::UInt16));
static_assert(!isUnsignedIntegerType(BaseType::Int16));
static_assert(isUnsignedIntegerType(BaseType::UInt32));
static_assert(!isUnsignedIntegerType(BaseType::Int32));
static_assert(isUnsignedIntegerType(BaseType::UInt64));
static_assert(!isUnsignedIntegerType(BaseType::Int64));
static_assert(!isUnsignedIntegerType(BaseType::Float16));
static_assert(!isUnsignedIntegerType(BaseType::Float32));
static_assert(!isUnsignedIntegerType(BaseType::Float64));
static_assert(!isUnsignedIntegerType(BaseType::Undefined));

} // Util
