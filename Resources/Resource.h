/*
	This file is part of the Util library.
	Copyright (C) 2020-2022 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef UTIL_RESOURCES_RESOURCE_H_
#define UTIL_RESOURCES_RESOURCE_H_

#include "../ReferenceCounter.h"
#include "../TypeNameMacro.h"

#include <unordered_set>
#include <memory>
#include <cstdint>

namespace Util {
class Device;

/**
 * @brief Basic representation of a resource on a device
 * 
 * A resource can be any resource stored on device (e.g., buffers, images, etc.).
 * It can be shared between devices where every device can have its own internal representation of the resource.
 * The resource does not have allocated memory by itself. This is managed on the devices the resource belong to.
 */
class Resource : public ReferenceCounter<Resource> {
	PROVIDES_TYPE_NAME(Resource)
protected:
	Resource(); // should only be created by a device
public:

	uint64_t getId() const { return id; }

	UTILAPI virtual ~Resource();

	UTILAPI void releaseAll();

private:
	friend class Device;
	const uint64_t id; // unique identifier
	std::unordered_set<WeakPointer<Device>> owners; // internal represenation for each device

	void removeOwner(WeakPointer<Device> device);

	void addOwner(WeakPointer<Device> device);

	bool isOwnedBy(WeakPointer<Device> device) const;
};

using ResourceHandle = Reference<Resource>;

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_H_ */