/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef UTIL_RESOURCES_RESOURCE_H_
#define UTIL_RESOURCES_RESOURCE_H_

#include "ResourceAllocator.h"
#include "ResourceFormat.h"
#include "../ReferenceCounter.h"

#include <vector>
#include <cstdint>

namespace Util {

class Resource : public Util::ReferenceCounter<Resource> {
public:
	explicit Resource(const ResourceFormat& format, ResourceAllocator* allocator=nullptr);
	virtual ~Resource();
	Resource(const Resource&) = delete;
	Resource(Resource&&) = default;
	Resource& operator=(const Resource& o) = delete;
	Resource& operator=(Resource&& o) = default;

	virtual void upload(const uint8_t* srcData, size_t size, size_t offset=0);

	template<typename T>
	void upload(const std::vector<T>& data, size_t offset = 0) {
		upload(reinterpret_cast<const uint8_t*>(data.data()), data.size() * sizeof(T), offset);
	}

	virtual void download(uint8_t* tgtData, size_t size, size_t offset=0);

	template<typename T>
	std::vector<T> download(size_t numberOfElements, size_t offset=0) {
		std::vector<T> result(numberOfElements);
		download(reinterpret_cast<const T*>(result.data()), numberOfElements*sizeof(T), offset);
		return result;
	}

	virtual void release() {}

	virtual void flush() {}
	virtual uint8_t* map() = 0;
	virtual void unmap() {}

	size_t getSize() const { return dataSize; }
	const ResourceFormat& getFormat() const { return format; }
protected:
	size_t dataSize = 0;
	bool checkRange(size_t offset, size_t size) const { return offset+size <= dataSize; }
private:
	ResourceFormat format;
	ResourceAllocator* allocator = nullptr;
};

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_H_ */