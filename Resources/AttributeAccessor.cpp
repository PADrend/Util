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

namespace Util {

static std::unordered_map<uint8_t, AttributeAccessor::AccessorFactory_t>& getAccessorRegistry() {
	static std::unordered_map<uint8_t, AttributeAccessor::AccessorFactory_t> registry;
	return registry;
}

//-------------------------------------------------------------
// StandardAttributeAccessor

template<typename T>
class StandardAttributeAccessor : public AttributeAccessor {
public:
	StandardAttributeAccessor(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {	
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getComponentCount());
		const T* v = _ptr<const T>(index);
		std::copy(v, v + count, values);
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getComponentCount());
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
	UnsignedNormalizedAttributeAccessor(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getComponentCount());
		const T* v = _ptr<const T>(index);
		for(size_t i=0; i<count; ++i)
			*(values+i) = unnormalizeUnsigned<S>(normalizeUnsigned<T>(*(v+i)));
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getComponentCount());
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
	SignedNormalizedAttributeAccessor(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) :
		AttributeAccessor(ptr, size, attr, stride) {}
	
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getComponentCount());
		const T* v = _ptr<const T>(index);
		for(size_t i=0; i<count; ++i)
			*(values+i) = unnormalizeSigned<S>(normalizeSigned<T>(*(v+i)));
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		assertRange(index);
		count = std::min<size_t>(count, getAttribute().getComponentCount());
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
// AttributeAccessor

Reference<AttributeAccessor> AttributeAccessor::create(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) {
	
	if(attr.getInternalType() != 0) {
		const auto& registry = getAccessorRegistry();
		auto factory = registry.find(attr.getInternalType());
		if(factory != registry.end()) {
			return factory->second(ptr, size, attr, stride);
		}
		WARN("AttributeAccessor: No accessor found for internal type " + StringUtils::toString(attr.getInternalType()) + ". Using default accessor.");
	}

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
	
	WARN("Could not create attribute accessor for attribute: " + attr.toString());
	return nullptr;
}

//-------------

Reference<AttributeAccessor> AttributeAccessor::create(uint8_t* ptr, size_t size, const ResourceFormat& format, const StringIdentifier& name) {
	return format.hasAttribute(name) ? create(ptr, size, format.getAttribute(name), format.getSize()) : nullptr;
}

//-------------

bool AttributeAccessor::registerAccessor(uint32_t type, const AccessorFactory_t& factory) {
	getAccessorRegistry().emplace(type, factory);
	return true;
}

//-------------

bool AttributeAccessor::hasAccessor(const AttributeFormat& attr) {
	if(attr.getInternalType() != 0) {
		const auto& registry = getAccessorRegistry();
		return registry.find(attr.getInternalType()) != registry.end();
	} else {
		return attr.getDataType() != TypeConstant::HALF;
	}
}

} /* Util */
