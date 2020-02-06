/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_RESOURCEALLOCATOR_H_
#define UTIL_RESOURCES_RESOURCEALLOCATOR_H_

#include "../References.h"
#include "ResourceFormat.h"

#include <memory>

namespace Util {


class ResourceAllocator {
public:
	struct Allocation {
		const size_t offset;
		const size_t size;
		const uint64_t handle;
	};

	virtual Allocation allocate(size_t size) = 0;
	virtual Allocation allocate(const Util::ResourceFormat& format, size_t count) {
		return allocate(format.getSize()*count);
	}
	virtual void free(Allocation&& allocation) = 0;
};

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCEALLOCATOR_H_ */