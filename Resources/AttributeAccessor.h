/*
	This file is part of the Util library.
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_ATTRIBUTEACCESSOR_H_
#define UTIL_RESOURCES_ATTRIBUTEACCESSOR_H_

#include "AttributeFormat.h"
#include "../ReferenceCounter.h"
#include "../StringIdentifier.h"

#include <vector>
#include <cstdlib>
#include <functional>
#include <sstream>

namespace Util {
class ResourceFormat;

/** AttributeAccessor
	Generic accessor for attributes of a resource format.
	@ingroup resources
*/
class AttributeAccessor : public ReferenceCounter<AttributeAccessor> {
protected:
	AttributeAccessor(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride=0) :
		dataPtr(ptr + attr.getOffset()), dataSize(size), attribute(attr), stride(stride == 0 ? attr.getDataSize() : stride) {}
		
	inline void assertRange(uint32_t index) const { 
		if(!checkRange(index)) {
			std::ostringstream s;
			s << "Trying to access attribute at index " << index << " of overall " << (dataSize/stride) << " indices.";
			throw std::range_error(s.str());
		}
	 }
public:
	using Ref = Reference<AttributeAccessor>;
	virtual ~AttributeAccessor() = default;
		
	//! Creates a new attribute accessor for the given data using the specified resource format.
	static Ref create(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride=0);
	static Ref create(uint8_t* ptr, size_t size, const ResourceFormat& format, const StringIdentifier& name);
	
	//! (uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) -> Ref
	using AccessorFactory_t = std::function<Ref(uint8_t*, size_t, const AttributeFormat&, size_t)>;

	//! Registers an accessor for an internal type.
	static bool registerAccessor(uint32_t internalType, const AccessorFactory_t& factory);
	static bool hasAccessor(const AttributeFormat& attr);
	
	void readRaw(size_t index, uint8_t* data, size_t size) const;
	virtual void readValues(size_t index, int8_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, int16_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, int32_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, int64_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, uint8_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, uint16_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, uint32_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, uint64_t* values, size_t count) const = 0;
	virtual void readValues(size_t index, float* values, size_t count) const = 0;
	virtual void readValues(size_t index, double* values, size_t count) const = 0;
	
	/**
	* Reads a single value of any primitive type from the resource, starting at the given index.
	* The value is internally converted to the correct type.
	*/
	template<typename T> 
	T readValue(size_t index) {
		T value;
		readValues(index, &value, 1);
		return value;
	}
	
	/**
	* Reads multiple values of any primitive type from the resource, starting at the given index.
	* The values are internally converted to the correct type.
	*/
	template<typename T> 
	std::vector<T> readValues(size_t index) {
		std::vector<T> values;
		readValues(index, values.data(), values.size());
		return values;
	}
	
	void writeRaw(size_t index, const uint8_t* data, size_t size) const;
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const float* values, size_t count) const = 0;
	virtual void writeValues(size_t index, const double* values, size_t count) const = 0;
	
	/**
	* Writes a single value of any primitive type into the resource, starting at the given index. 
	* The value is internally converted to the correct type.
	*/
	template<typename T> 
	void writeValue(size_t index, const T& value) {
		writeValues(index, &value, 1);
	}
	
	/**
	* Writes multiple values of any primitive type into the resource, starting at the given index. 
	* The values are internally converted to the correct type.
	*/
	template<typename T> 
	void writeValues(size_t index, const std::vector<T>& values) {
		writeValues(index, values.data(), values.size());
	}
	
	/**
	* Returns the resource format attribute this accessor is associated with. 
	*/
	const AttributeFormat& getAttribute() const { return attribute; }
	
	/**
	* Checks whether the @p index is in range. 
	*/
	inline bool checkRange(uint32_t index) const { return index*stride<dataSize; }
	
	/**
	* Returns the raw data pointer to the resource attribute at the given index. 
	*/
	template<typename number_t>
	number_t * _ptr(size_t index) const { return reinterpret_cast<number_t*>(dataPtr+index*stride); }	

	//! Get the size in bytes of the accessed data.
	size_t getDataSize() const { return dataSize; }
private:
	uint8_t* const dataPtr;
	const size_t dataSize;
	const AttributeFormat attribute;
	const size_t stride;
};

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_ATTRIBUTEACCESSOR_H_ */
