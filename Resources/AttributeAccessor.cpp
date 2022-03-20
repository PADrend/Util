/*
	This file is part of the Util library.
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "AttributeAccessor.h"
#include "ResourceFormat.h"

#include "../Macros.h"
#include "../StringUtils.h"

#include <limits>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <algorithm>

#ifdef _MSC_VER
#	pragma warning(disable : 4244) // 4244 - 'argument' : conversion from 'type1' to 'type2', possible loss of data
#endif

namespace Util {

#define OVERRIDE_READ_VALUES(function) \
void readValues(uint64_t index, int8_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, int16_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, int32_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, int64_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, uint8_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, uint16_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, uint32_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, uint64_t* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, float* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, double* values, uint64_t count) const override { function(index, values, count); } \
void readValues(uint64_t index, half_t* values, uint64_t count) const override { function(index, values, count); }

#define OVERRIDE_WRITE_VALUES(function) \
void writeValues(uint64_t index, const int8_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const int16_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const int32_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const int64_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const uint8_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const uint16_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const uint32_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const uint64_t* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const float* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const double* values, uint64_t count) const override { function(index, values, count); } \
void writeValues(uint64_t index, const half_t* values, uint64_t count) const override { function(index, values, count); }

static std::unordered_map<uint32_t, AttributeAccessor::AccessorFactory_t>& getAccessorRegistry() {
	static std::unordered_map<uint32_t, AttributeAccessor::AccessorFactory_t> registry;
	return registry;
}

//-------------------------------------------------------------
// StandardAttributeAccessor

template<typename T>
class StandardAttributeAccessor : public AttributeAccessor {
public:
	StandardAttributeAccessor(uint8_t* ptr, uint64_t size, const AttributeFormat& attr, uint64_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(uint64_t index, S* values, uint64_t count) const {	
		assertRange(index);
		count = std::min<uint64_t>(count, getAttribute().components);
		const T* v = _ptr<const T>(index);
		std::transform(v, v + count, values, [](T v) { return static_cast<S>(v);});
	}
	
	template<typename S>
	void _writeValues(uint64_t index, const S* values, uint64_t count) const {
		assertRange(index);
		count = std::min<uint64_t>(count, getAttribute().components);
		T* v = _ptr<T>(index);
		std::transform(values, values + count, v, [](S v) { return static_cast<T>(v);});
	}

	OVERRIDE_READ_VALUES(_readValues)
	OVERRIDE_WRITE_VALUES(_writeValues)
};

//-------------------------------------------------------------
// UnsignedNormalizedAttributeAccessor

template<typename T>
class UnsignedNormalizedAttributeAccessor : public AttributeAccessor {
public:
	UnsignedNormalizedAttributeAccessor(uint8_t* ptr, uint64_t size, const AttributeFormat& attr, uint64_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(uint64_t index, S* values, uint64_t count) const {
		assertRange(index);
		count = std::min<uint64_t>(count, getAttribute().components);
		const T* v = _ptr<const T>(index);
		for(uint64_t i=0; i<count; ++i)
			*(values+i) = unnormalizeUnsigned<S>(normalizeUnsigned<T>(*(v+i)));
	}
	
	template<typename S>
	void _writeValues(uint64_t index, const S* values, uint64_t count) const {
		assertRange(index);
		count = std::min<uint64_t>(count, getAttribute().components);
		T* v = _ptr<T>(index);
		for(uint64_t i=0; i<count; ++i)
			*(v+i) = unnormalizeUnsigned<T>(normalizeUnsigned<S>(*(values+i)));
	}
		
	OVERRIDE_READ_VALUES(_readValues)
	OVERRIDE_WRITE_VALUES(_writeValues)
};

//-------------------------------------------------------------
// SignedNormalizedAttributeAccessor

template<typename T>
class SignedNormalizedAttributeAccessor : public AttributeAccessor {
public:
	SignedNormalizedAttributeAccessor(uint8_t* ptr, uint64_t size, const AttributeFormat& attr, uint64_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(uint64_t index, S* values, uint64_t count) const {
		assertRange(index);
		count = std::min<uint64_t>(count, getAttribute().components);
		const T* v = _ptr<const T>(index);
		for(uint64_t i=0; i<count; ++i)
			*(values+i) = unnormalizeSigned<S>(normalizeSigned<T>(*(v+i)));
	}
	
	template<typename S>
	void _writeValues(uint64_t index, const S* values, uint64_t count) const {
		assertRange(index);
		count = std::min<uint64_t>(count, getAttribute().components);
		T* v = _ptr<T>(index);
		for(uint64_t i=0; i<count; ++i)
			*(v+i) = unnormalizeSigned<T>(normalizeSigned<S>(*(values+i)));
	}

	OVERRIDE_READ_VALUES(_readValues)
	OVERRIDE_WRITE_VALUES(_writeValues)
};

//-------------------------------------------------------------
// AttributeAccessor

Reference<AttributeAccessor> AttributeAccessor::create(uint8_t* ptr, uint64_t size, const AttributeFormat& attr, uint64_t stride) {
	
	if(attr.internalType != 0) {
		const auto& registry = getAccessorRegistry();
		auto factory = registry.find(attr.internalType);
		if(factory != registry.end()) {
			return factory->second(ptr, size, attr, stride);
		}
		WARN("AttributeAccessor: No accessor found for attribue " + attr.toString() + ". Using default accessor.");
	}

	if(attr.normalized) {
		switch (attr.baseType) {
			case BaseType::UInt8: return new UnsignedNormalizedAttributeAccessor<uint8_t>(ptr, size, attr, stride);
			case BaseType::UInt16: return new UnsignedNormalizedAttributeAccessor<uint16_t>(ptr, size, attr, stride);
			case BaseType::UInt32: return new UnsignedNormalizedAttributeAccessor<uint32_t>(ptr, size, attr, stride);
			case BaseType::UInt64: return new UnsignedNormalizedAttributeAccessor<uint64_t>(ptr, size, attr, stride);
			case BaseType::Int8: return new SignedNormalizedAttributeAccessor<int8_t>(ptr, size, attr, stride);
			case BaseType::Int16: return new SignedNormalizedAttributeAccessor<int16_t>(ptr, size, attr, stride);
			case BaseType::Int32: return new SignedNormalizedAttributeAccessor<int32_t>(ptr, size, attr, stride);
			case BaseType::Int64: return new SignedNormalizedAttributeAccessor<int64_t>(ptr, size, attr, stride);
			case BaseType::Float16: return new SignedNormalizedAttributeAccessor<half_t>(ptr, size, attr, stride);
			case BaseType::Float32: return new SignedNormalizedAttributeAccessor<float>(ptr, size, attr, stride);
			case BaseType::Float64: return new SignedNormalizedAttributeAccessor<double>(ptr, size, attr, stride);
			default: break;
		}
	} else {
		switch (attr.baseType) {
			case BaseType::UInt8: return new StandardAttributeAccessor<uint8_t>(ptr, size, attr, stride);
			case BaseType::UInt16: return new StandardAttributeAccessor<uint16_t>(ptr, size, attr, stride);
			case BaseType::UInt32: return new StandardAttributeAccessor<uint32_t>(ptr, size, attr, stride);
			case BaseType::UInt64: return new StandardAttributeAccessor<uint64_t>(ptr, size, attr, stride);
			case BaseType::Int8: return new StandardAttributeAccessor<int8_t>(ptr, size, attr, stride);
			case BaseType::Int16: return new StandardAttributeAccessor<int16_t>(ptr, size, attr, stride);
			case BaseType::Int32: return new StandardAttributeAccessor<int32_t>(ptr, size, attr, stride);
			case BaseType::Int64: return new StandardAttributeAccessor<int64_t>(ptr, size, attr, stride);
			case BaseType::Float16: return new StandardAttributeAccessor<half_t>(ptr, size, attr, stride);
			case BaseType::Float32: return new StandardAttributeAccessor<float>(ptr, size, attr, stride);
			case BaseType::Float64: return new StandardAttributeAccessor<double>(ptr, size, attr, stride);
			default: break;
		}
	}
	
	WARN("Could not create attribute accessor for attribute: " + attr.toString());
	return nullptr;
}

//-------------

Reference<AttributeAccessor> AttributeAccessor::create(uint8_t* ptr, uint64_t size, const ResourceFormat& format, const StringIdentifier& name) {
	return format.hasAttribute(name) ? create(ptr, size, format.getAttribute(name), format.getSize()) : nullptr;
}

//-------------

bool AttributeAccessor::registerAccessor(uint32_t type, const AccessorFactory_t& factory) {
	getAccessorRegistry().emplace(type, factory);
	return true;
}

//-------------

bool AttributeAccessor::hasAccessor(const AttributeFormat& attr) {
	if(attr.internalType != 0) {
		const auto& registry = getAccessorRegistry();
		return registry.find(attr.internalType) != registry.end();
	}
	return false;
}


//-------------

void AttributeAccessor::readRaw(uint64_t index, uint8_t* data, uint64_t size) const {
	assertRange(index);
	size = std::min<uint64_t>(size, getAttribute().dataSize);
	const uint8_t* ptr = _ptr<const uint8_t>(index);
	std::copy(ptr, ptr + size, data);
}

//-------------
	
void AttributeAccessor::writeRaw(uint64_t index, const uint8_t* data, uint64_t size) const {
	assertRange(index);
	size = std::min<uint64_t>(size, getAttribute().dataSize);
	uint8_t* ptr = _ptr<uint8_t>(index);
	std::copy(data, data + size, ptr);
}

//-------------
} /* Util */
