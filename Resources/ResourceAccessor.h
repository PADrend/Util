/*
	This file is part of the Util library.
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_RESOURCEACCESSOR_H_
#define UTIL_RESOURCES_RESOURCEACCESSOR_H_

#include "../ReferenceCounter.h"
#include "../StringIdentifier.h"
#include "AttributeAccessor.h"
#include "ResourceFormat.h"

#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <sstream>

namespace Util {
class Resource;
using ResourceRef = Util::Reference<Resource>;

/** ResourceAccessor
	@ingroup resources
*/
class ResourceAccessor : public ReferenceCounter<ResourceAccessor> {
protected:
	UTILAPI void assertRangeLocation(uint64_t index, uint32_t location) const;
	UTILAPI void assertAttribute(const StringIdentifier& id) const;
public:
	using Ref = Util::Reference<ResourceAccessor>;
	static Ref create(uint8_t* ptr, uint64_t size, ResourceFormat format) { return new ResourceAccessor(ptr, size, format); }
	static Ref create(const ResourceRef& resource) { return new ResourceAccessor(resource); }

	UTILAPI explicit ResourceAccessor(uint8_t* ptr, uint64_t size, ResourceFormat format);
	UTILAPI explicit ResourceAccessor(const ResourceRef& resource);
	UTILAPI virtual ~ResourceAccessor();
	
	/** Reads one or more elements without any conversion
		Reads count many elements from the resource at the given index
		@node targetPtr needs to be large enough to hold count many elements of the resources format size
	*/
	UTILAPI void readRaw(uint64_t index, uint8_t* targetPtr, uint64_t count=1);
	
	/** Writes one or more elements without any conversion
		Writes count many elements to the resource at the given index
		@node ptr needs to be large enough to hold count many elements of the resources format size
	*/
	UTILAPI void writeRaw(uint64_t index, const uint8_t* sourcePtr, uint64_t count=1);
	
	template<typename T>
	void readValues(uint64_t index, uint32_t location, T* values, uint64_t count) const {
		assertRangeLocation(index, location);
		accessors[location]->readValues(index, values, count);
	}
		
	template<typename T> 
	T readValue(uint64_t index, uint32_t location) const {
		T value;
		readValues(index, location, &value, 1);
		return value;
	}
	
	void readRawValue(uint64_t index, uint32_t location, uint8_t* data, uint64_t size) const {
		assertRangeLocation(index, location);
		accessors[location]->readRaw(index, data, size);
	}
	
	void readRawValue(uint64_t index, const StringIdentifier& id, uint8_t* data, uint64_t size) const {
		assertAttribute(id);
		readRawValue(index, locations.at(id), data, size);
	}
	
	template<typename T> 
	std::vector<T> readValues(uint64_t index, uint32_t location, uint64_t count) const {
		std::vector<T> values(count);
		readValues(index, location, values.data(), values.size());
		return values;
	}
	
	template<typename T>
	void readValues(uint64_t index, const StringIdentifier& id, T* values, uint64_t count) const {
		assertAttribute(id);
		return readValues<T>(index, locations.at(id), values, count);
	}
		
	template<typename T> 
	T readValue(uint64_t index, const StringIdentifier& id) const {
		assertAttribute(id);
		return readValue<T>(index, locations.at(id));
	}
	
	template<typename T> 
	std::vector<T> readValues(uint64_t index, const StringIdentifier& id, uint64_t count) const {
		assertAttribute(id);
		return readValues<T>(index, locations.at(id), count);
	}
	
	template<typename T>
	void writeValues(uint64_t index, uint32_t location, const T* values, uint64_t count) {
		if(location >= format.getNumAttributes()) return; // ignore invalid locations
		assertRangeLocation(index, location);
		accessors[location]->writeValues(index, values, count);
	}
	
	template<typename T>
	void writeValues(uint64_t index, const StringIdentifier& id, const T* values, uint64_t count) {
		writeValues(index, locations.find(id) != locations.end() ? locations.at(id) : format.getNumAttributes(), values, count);
	}
	
	template<typename T> 
	void writeValue(uint64_t index, uint32_t location, const T& value) {
		writeValues(index, location, &value, 1);
	}
	
	template<typename T> 
	void writeValue(uint64_t index, const StringIdentifier& id, const T& value) {
		writeValues(index, locations.find(id) != locations.end() ? locations.at(id) : format.getNumAttributes(), &value, 1);
	}
	
	void writeRawValue(uint64_t index, uint32_t location, const uint8_t* data, uint64_t size) {
		if(location >= format.getNumAttributes()) return; // ignore invalid locations
		accessors[location]->writeRaw(index, data, size);
	}
	
	void writeRawValue(uint64_t index, const StringIdentifier& id, const uint8_t* data, uint64_t size) {
		writeRawValue(index, locations.find(id) != locations.end() ? locations.at(id) : format.getNumAttributes(), data, size);
	}
	
	template<typename T> 
	void writeValues(uint64_t index, uint32_t location, const std::vector<T>& values) {
		writeValues(index, location, values.data(), values.size());
	}
	
	template<typename T> 
	void writeValues(uint64_t index, const StringIdentifier& id, const std::vector<T>& values) {
		writeValues(index, locations.find(id) != locations.end() ? locations.at(id) : format.getNumAttributes(), values.data(), values.size());
	}
	
	const ResourceFormat& getFormat() const { return format; }
	uint64_t getDataSize() const { return dataSize; }
	uint64_t getElementCount() const { return elementCount; }
	uint32_t getAttributeLocation(const StringIdentifier& id) const { return format.getAttributeLocation(id); }
private:
	ResourceRef resource;
	const ResourceFormat format;
	uint8_t* const dataPtr;
	const uint64_t dataSize;
	const uint64_t elementCount;
	std::unordered_map<StringIdentifier, uint32_t> locations;
	std::vector<Reference<AttributeAccessor>> accessors;
};

inline
void ResourceAccessor::assertRangeLocation(uint64_t index, uint32_t location) const {
	if(location >= format.getNumAttributes()) {
		std::ostringstream s;
		s << "Trying to access attribute at location " << location << " of overall " << format.getNumAttributes() << " attributes.";
		throw std::range_error(s.str());
	}
	if(index >= elementCount) {
		std::ostringstream s;
		s << "Trying to access element at index " << index << " of overall " << elementCount << " elements.";
		throw std::range_error(s.str());
	}
}

inline
void ResourceAccessor::assertAttribute(const StringIdentifier& id) const {
	if(!format.hasAttribute(id)) {
		std::ostringstream s;
		s << "There is no attribute named '" << id.toString() << "'.";
		throw std::range_error(s.str());
	}
}

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCEACCESSOR_H_ */
