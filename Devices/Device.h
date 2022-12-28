/*
This file is part of the Platform for Algorithm Development and Rendering (PADrend).
Web page: http://www.padrend.de/
Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
Copyright (C) 2014-2022 Sascha Brandt <sascha@brandt.graphics>

This library is subject to the terms of the Mozilla Public License, v. 2.0.
You should have received a copy of the MPL along with this library; see the 
file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_DEVICE_H_
#define UTIL_RESOURCES_DEVICE_H_

#include "../ReferenceCounter.h"
#include "../Resources/Resource.h"
#include "../TypeNameMacro.h"

#include <memory>
#include <unordered_map>

namespace Util {


/**
 * @brief Basic representation of a device.
 * 
 * A Device manages resources and commands that use/modify these resources.
 * This can be a physical device (e.g., GPU, CPU) or virtual device (e.g., remote device).
 * Resources can be shared/transfered between devices and resource usage can be synchronized between them.
 */
class Device : public ReferenceCounter<Device> {
	PROVIDES_TYPE_NAME(Device)
protected:
	UTILAPI Device();
public:
	UTILAPI virtual ~Device();

	//! Returns the unique id of the device
	uint64_t getId() const { return id; }

	//! shuts down the device and releases all resources
	UTILAPI virtual void shutdown();

	//! waits until all commands running on this device have finished executing.
	virtual void waitIdle() {}

	//! @name Resources
	// @{
	
	//! releases all resources owned by this device
	UTILAPI void releaseAllResources();

	//! removes ownership of the resource and releases its internal representation.
	UTILAPI void releaseResource(ResourceHandle resource);
	
	//! checks if the device owns the resource
	UTILAPI bool ownsResource(const ResourceHandle& resource) const;
protected:
	/**
	 * Internal device-specific representation of a resource. Each device should implement a representation that inherits from this.
	 */
	struct InternalResource {
		InternalResource(const ResourceHandle& _resource) : resource(_resource.get()), resourceId(_resource->id), typeId(_resource->getTypeId()) {}
		WeakPointer<Resource> resource; // weak reference to the resource.
		const uint64_t resourceId;
		const StringIdentifier typeId;
		// TODO: dirty/locking/sync mechanisms
	};

	//! releases the internal representation
	virtual void releaseResource(InternalResource* resource) = 0;
	//! takes ownership of the resource.
	UTILAPI void takeOwnership(ResourceHandle resource, std::unique_ptr<InternalResource>&& internal);
	//! returns the internal representation of the resource or nullptr if not available
	UTILAPI InternalResource* getInternalRepresentation(const ResourceHandle& resource) const;
private:
	//! stores resources with their unique id
	std::unordered_map<uint64_t, std::unique_ptr<InternalResource>> resources;
public:
	// @}

	//! @name Command execution
	// @{
	
	//! waits until all commands have been finished on this device.
	//virtual void wait() const = 0;

	// @}
private:
	//! unique device id
	const uint64_t id;
};

// device handle
using DeviceHandle = Reference<Device>;

} // Util

#endif // UTIL_RESOURCES_DEVICE_H_