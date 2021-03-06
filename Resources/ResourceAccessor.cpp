/*
	This file is part of the Util library.
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ResourceAccessor.h"
#include "Resource.h"

namespace Util {

//-------------------

ResourceAccessor::ResourceAccessor(uint8_t* ptr, uint64_t size, ResourceFormat format) : resource(nullptr), format(format), dataPtr(ptr), dataSize(size), elementCount(size / format.getSize()) {
	const auto& attributes = format.getAttributes();
	accessors.resize(attributes.size());
	for(uint32_t i=0; i<attributes.size(); ++i) {
		locations[attributes[i].getNameId()] = i;
		accessors[i] = AttributeAccessor::create(dataPtr, size, attributes[i], format.getSize());
	}
}

//-------------------

ResourceAccessor::ResourceAccessor(const ResourceRef& _resource) : ResourceAccessor(_resource->map(), _resource->getSize(), _resource->getFormat()) {
	resource = _resource;
}


//-------------------

ResourceAccessor::~ResourceAccessor() {
	if(resource) {
		resource->unmap();
	}
};

//-------------------

void ResourceAccessor::readRaw(uint64_t index, uint8_t* targetPtr, uint64_t count) {
	assertRangeLocation(index+count-1, 0);
	const uint8_t* ptr = dataPtr + index*format.getSize();
	std::copy(ptr, ptr + count*format.getSize(), targetPtr);
}

//-------------------

void ResourceAccessor::writeRaw(uint64_t index, const uint8_t* sourcePtr, uint64_t count) {
	assertRangeLocation(index+count-1, 0);
	uint8_t* ptr = dataPtr + index*format.getSize();
	std::copy(sourcePtr, sourcePtr + count*format.getSize(), ptr);
}

//-------------------

} /* Util */
