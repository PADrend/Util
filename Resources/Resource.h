/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef UTIL_RESOURCES_RESOURCE_H_
#define UTIL_RESOURCES_RESOURCE_H_

#include "../ReferenceCounter.h"
#include "ResourceFormat.h"

#include <vector>
#include <cstdint>

namespace Util {

class Resource : public Util::ReferenceCounter<Resource> {
public:
	explicit Resource(const ResourceFormat& format);
	virtual ~Resource() = default;

	void upload(const uint8_t* srcData, size_t size, size_t offset=0);

	template<typename T>
	void upload(const std::vector<T>& data, size_t offset = 0) {
		upload(reinterpret_cast<const uint8_t*>(data.data()), data.size() * sizeof(T), offset);
	}

	void download(uint8_t* tgtData, size_t size, size_t offset=0);

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

	size_t getDataSize() const { return dataSize; }
	const ResourceFormat& getFormat() const { return format; }

	bool hasDataChanged() const { return dataHasChanged; }
	void markAsChanged() { dataHasChanged = true; }
protected:
	const ResourceFormat format;
	size_t dataSize = 0;
	bool dataHasChanged = false;

	virtual void doUpload(const uint8_t* srcData, size_t size, size_t offset=0) = 0;
	virtual void doDownload(uint8_t* tgtData, size_t size, size_t offset=0) = 0;
};

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_H_ */