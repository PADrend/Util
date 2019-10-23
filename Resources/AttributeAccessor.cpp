/*
	This file is part of the Util library.
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "AttributeAccessor.h"

#include "../Macros.h"

#include <limits>
#include <sstream>
#include <cstring>
#include <unordered_map>

namespace Util {

/*static std::unordered_map<uint8_t, AttributeAccessor::AccessorFactory_t>& getAccessorRegistry() {
	static std::unordered_map<uint8_t, AttributeAccessor::AccessorFactory_t> registry;
	return registry;
}*/

template<typename T>
T clamp(T value, T min, T max) { return std::min(max, std::max(min, value)); }

//-------------

template<typename T>
double normalizeUnsigned(T value) { return static_cast<double>(value)/std::numeric_limits<T>::max(); }
template<>
double normalizeUnsigned(float value) { return clamp(value, 0.0f, 1.0f); }
template<>
double normalizeUnsigned(double value) { return clamp(value, 0.0, 1.0); }

//-------------

template<typename T>
T unnormalizeUnsigned(double value) { return static_cast<T>(clamp(value, 0.0, 1.0) * std::numeric_limits<T>::max()); }
template<>
float unnormalizeUnsigned(double value) { return clamp(value, 0.0, 1.0); }
template<>
double unnormalizeUnsigned(double value) { return clamp(value, 0.0, 1.0); }

//-------------

template<typename T>
double normalizeSigned(T value) { return std::max(static_cast<double>(value)/std::numeric_limits<T>::max(), -1.0); }
template<>
double normalizeSigned(float value) { return clamp(value, -1.0f, 1.0f); }
template<>
double normalizeSigned(double value) { return clamp(value, -1.0, 1.0); }

//-------------

template<typename T>
T unnormalizeSigned(double value) { return static_cast<T>(clamp(value, -1.0, 1.0) * std::numeric_limits<T>::max()); }
template<>
float unnormalizeSigned(double value) { return clamp(value, -1.0, 1.0); }
template<>
double unnormalizeSigned(double value) { return clamp(value, -1.0, 1.0); }

//-------------

/*static uint32_t toFloat11(float f) {
	uint32_t floatBits;
	memcpy(&floatBits, &f, sizeof(float));
	uint32_t exponent = (floatBits >> 23) & 0xffu;
	if(f <= 0 || exponent < 112) return 0;
	return ((exponent - 127 + 15) << 6) | ((floatBits & 0x7fffffu) >> 17);
}

//-------------

static uint32_t toFloat10(float f) {
	uint32_t floatBits;
	memcpy(&floatBits, &f, sizeof(float));
	uint32_t exponent = (floatBits >> 23) & 0xffu;
	if(f <= 0 || exponent < 112) return 0;
	return ((exponent - 127 + 15) << 5) | ((floatBits & 0x7fffffu) >> 18);
}

//-------------

static float fromFloat11(uint32_t float11Bits) {
	float f;
	uint32_t exponent = ((float11Bits & 0x7ffu) >> 6) + 127 - 15;
	uint32_t mantissa = (float11Bits & 0x3fu);
	uint32_t floatBits = (exponent << 23) | mantissa;
	memcpy(&f, &floatBits, sizeof(float));
	return f;
}

//-------------

static float fromFloat10(uint32_t float10Bits) {
	float f;
	uint32_t exponent = ((float10Bits & 0x3ffu) >> 5) + 127 - 15;
	uint32_t mantissa = (float10Bits & 0x1fu);
	uint32_t floatBits = (exponent << 23) | mantissa;
	memcpy(&f, &floatBits, sizeof(float));
	return f;
}*/

//-------------------------------------------------------------
// StandardAttributeAccessor

template<typename T>
class StandardAttributeAccessor : public AttributeAccessor {
public:
	StandardAttributeAccessor(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {	
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getNumValues());
		const T* v = _ptr<const T>(index);
		std::copy(v, v + count, values);
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getNumValues());
		T* v = _ptr<T>(index);
		std::copy(values, values + count, v);
	}
	
	virtual void readValues(size_t index, int8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, float* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, double* values, size_t count) const { _readValues(index, values, count); }
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const float* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const double* values, size_t count) const { _writeValues(index, values, count); }
};

//-------------------------------------------------------------
// UnsignedNormalizedAttributeAccessor

template<typename T>
class UnsignedNormalizedAttributeAccessor : public AttributeAccessor {
public:
	UnsignedNormalizedAttributeAccessor(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getNumValues());
		const T* v = _ptr<const T>(index);
		for(size_t i=0; i<count; ++i)
			*(values+i) = unnormalizeUnsigned<S>(normalizeUnsigned<T>(*(v+i)));
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getNumValues());
		T* v = _ptr<T>(index);
		for(size_t i=0; i<count; ++i)
			*(v+i) = unnormalizeUnsigned<T>(normalizeUnsigned<S>(*(values+i)));
	}
		
	virtual void readValues(size_t index, int8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, float* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, double* values, size_t count) const { _readValues(index, values, count); }
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const float* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const double* values, size_t count) const { _writeValues(index, values, count); }
};

//-------------------------------------------------------------
// SignedNormalizedAttributeAccessor

template<typename T>
class SignedNormalizedAttributeAccessor : public AttributeAccessor {
public:
	SignedNormalizedAttributeAccessor(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getNumValues());
		const T* v = _ptr<const T>(index);
		for(size_t i=0; i<count; ++i)
			*(values+i) = unnormalizeSigned<S>(normalizeSigned<T>(*(v+i)));
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getNumValues());
		T* v = _ptr<T>(index);
		for(size_t i=0; i<count; ++i)
			*(v+i) = unnormalizeSigned<T>(normalizeSigned<S>(*(values+i)));
	}
		
	virtual void readValues(size_t index, int8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, float* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, double* values, size_t count) const { _readValues(index, values, count); }
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const float* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const double* values, size_t count) const { _writeValues(index, values, count); }
};

//-------------------------------------------------------------
// R11G11B10FloatAccessor

/*class R11G11B10FloatAccessor : public AttributeAccessor {
public:
	R11G11B10FloatAccessor(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
		
	static Reference<AttributeAccessor> create(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) {
		return new R11G11B10FloatAccessor(ptr, size, attr, stride);
	}
		
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {	
		std::vector<float> floatValues(count);
		readValues(index, floatValues.data(), count);
		std::copy(floatValues.begin(), floatValues.end(), values);
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		std::vector<float> floatValues(values, values + count);
		writeValues(index, floatValues.data(), count);
	}
	
	virtual void readValues(size_t index, int8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, float* values, size_t count) const {
		assertRange(index);
		uint32_t v = *_ptr<const uint32_t>(index);
		if(count >= 0) *(values+0) = fromFloat11(v);
		if(count >= 1) *(values+1) = fromFloat11(v >> 11);
		if(count >= 2) *(values+2) = fromFloat10(v >> 22);
	}
	
	virtual void readValues(size_t index, double* values, size_t count) const { _readValues(index, values, count); }
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const float* values, size_t count) const {
		assertRange(index);
		uint32_t r = count >= 0 ? toFloat11(*(values+0)) : 0;
		uint32_t g = count >= 1 ? toFloat11(*(values+1)) : 0;
		uint32_t b = count >= 2 ? toFloat10(*(values+2)) : 0;
		*_ptr<uint32_t>(index) = r | (g << 11) | (b << 22);
	}
	
	virtual void writeValues(size_t index, const double* values, size_t count) const { _writeValues(index, values, count); }
};*/

//-------------------------------------------------------------
// RGBA8UnormSrgbAccessor

/*class RGBA8UnormSrgbAccessor : public AttributeAccessor {
public:
	RGBA8UnormSrgbAccessor(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
		
	static Reference<AttributeAccessor> create(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) {
		return new RGBA8UnormSrgbAccessor(ptr, size, attr, stride);
	}
	
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, 4);
		const uint8_t* v = _ptr<const uint8_t>(index);
		for(size_t i=0; i<count; ++i)
			*(values+i) = unnormalizeUnsigned<S>(normalizeUnsigned<uint8_t>(*(v+i)));
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, 4);
		uint8_t* v = _ptr<uint8_t>(index);
		for(size_t i=0; i<count; ++i)
			*(v+i) = unnormalizeUnsigned<uint8_t>(normalizeUnsigned<S>(*(values+i)));
	}
		
	virtual void readValues(size_t index, int8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, float* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, double* values, size_t count) const { _readValues(index, values, count); }
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const float* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const double* values, size_t count) const { _writeValues(index, values, count); }
};*/

//-------------------------------------------------------------
// AttributeAccessor

Reference<AttributeAccessor> AttributeAccessor::create(uint8_t* ptr, size_t size, const ResourceFormat::Attribute& attr, size_t stride) {
	if(attr.isNormalized()) {
		switch (attr.getDataType()) {
			case TypeConstant::UINT8: return new UnsignedNormalizedAttributeAccessor<uint8_t>(ptr, size, attr, stride);
			case TypeConstant::UINT16: return new UnsignedNormalizedAttributeAccessor<uint16_t>(ptr, size, attr, stride);
			case TypeConstant::UINT32: return new UnsignedNormalizedAttributeAccessor<uint32_t>(ptr, size, attr, stride);
			case TypeConstant::UINT64: return new UnsignedNormalizedAttributeAccessor<uint64_t>(ptr, size, attr, stride);
			case TypeConstant::INT8: return new SignedNormalizedAttributeAccessor<int8_t>(ptr, size, attr, stride);
			case TypeConstant::INT16: return new SignedNormalizedAttributeAccessor<int16_t>(ptr, size, attr, stride);
			case TypeConstant::INT32: return new SignedNormalizedAttributeAccessor<int32_t>(ptr, size, attr, stride);
			case TypeConstant::INT64: return new SignedNormalizedAttributeAccessor<int64_t>(ptr, size, attr, stride);
			case TypeConstant::FLOAT: return new SignedNormalizedAttributeAccessor<float>(ptr, size, attr, stride);
			case TypeConstant::DOUBLE: return new SignedNormalizedAttributeAccessor<double>(ptr, size, attr, stride);
			default: break;
		}
	} else {
		switch (attr.getDataType()) {
			case TypeConstant::UINT8: return new StandardAttributeAccessor<uint8_t>(ptr, size, attr, stride);
			case TypeConstant::UINT16: return new StandardAttributeAccessor<uint16_t>(ptr, size, attr, stride);
			case TypeConstant::UINT32: return new StandardAttributeAccessor<uint32_t>(ptr, size, attr, stride);
			case TypeConstant::UINT64: return new StandardAttributeAccessor<uint64_t>(ptr, size, attr, stride);
			case TypeConstant::INT8: return new StandardAttributeAccessor<int8_t>(ptr, size, attr, stride);
			case TypeConstant::INT16: return new StandardAttributeAccessor<int16_t>(ptr, size, attr, stride);
			case TypeConstant::INT32: return new StandardAttributeAccessor<int32_t>(ptr, size, attr, stride);
			case TypeConstant::INT64: return new StandardAttributeAccessor<int64_t>(ptr, size, attr, stride);
			case TypeConstant::FLOAT: return new StandardAttributeAccessor<float>(ptr, size, attr, stride);
			case TypeConstant::DOUBLE: return new StandardAttributeAccessor<double>(ptr, size, attr, stride);
			default: break;
		}
	}
	
	/*auto& registry = getAccessorRegistry();
	auto factory = registry.find(attr.getDataType());
	if(factory != registry.end()) {
		return factory->second(ptr, size, attr, stride);
	}*/
	
	WARN("Could not create attribute accessor for attribute: " + attr.toString());
	return nullptr;
}

//-------------

//! (internal)
void AttributeAccessor::throwRangeError(uint32_t index) const {
	std::ostringstream s;
	s << "Trying to access attribute at index " << index << " of overall " << (dataSize/stride) << " indices.";
	throw std::range_error(s.str());
}

//-------------

/*bool AttributeAccessor::registerAccessor(uint8_t type, const AccessorFactory_t& factory) {
	getAccessorRegistry().emplace(type, factory);
	return true;
}*/

//-------------------------------------------------------------
// Initialize special accessors

//static const bool R11G11B10FloatAccRegistered = AttributeAccessor::registerAccessor(ResourceFormat::Attribute::TYPE_R11G11B10Float, R11G11B10FloatAccessor::create);
//static const bool RGBA8UnormSrgbAccRegistered = AttributeAccessor::registerAccessor(ResourceFormat::Attribute::TYPE_RGBA8UnormSrgb, RGBA8UnormSrgbAccessor::create);

} /* Util */
