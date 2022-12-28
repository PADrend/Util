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
#include "Device.h"
#include "../Macros.h"

namespace Util {
namespace {

uint64_t generateDeviceId(Device* device) {
	static uint64_t counter = 0;
	return ++counter | (static_cast<uint64_t>(device->getTypeId().getValue()) << 32);
}

} // _

//---------------

Device::Device() : id(generateDeviceId(this)) {

}

//---------------

Device::~Device() = default;

//---------------

void Device::shutdown() {
	waitIdle();
	releaseAllResources();
}

//---------------

void Device::releaseAllResources() {
	for(auto& entry : resources) {
		if(entry.second && entry.second.get()) {
			entry.second->resource->removeOwner(this);
			releaseResource(entry.second.get());
		}
	}
	resources.clear();
}

//---------------

void Device::releaseResource(ResourceHandle resource) {
	if(!resource)
		return;
	
	resource->removeOwner(this);
	if(auto it = resources.find(resource->id); it != resources.end()) {
		releaseResource(it->second.get());	
		resources.erase(it);
	}
}

//---------------

bool Device::ownsResource(const ResourceHandle& resource) const {
	return resource.isNotNull() ? (resources.find(resource->id) != resources.end()) : false;
}

//---------------

void Device::takeOwnership(ResourceHandle resource, std::unique_ptr<InternalResource>&& internal) {
	if(!resource)
		return;
	WARN_AND_RETURN_IF(ownsResource(resource), "Device " + std::to_string(id) + " already owns resource " + std::to_string(resource->id),);

}

//---------------

Device::InternalResource* Device::getInternalRepresentation(const ResourceHandle& resource) const {
	if(!resource)
		return nullptr;
	if(auto it = resources.find(resource->id); it != resources.end()) {
		return it->second.get();
	}
	return nullptr;
}

//---------------

} // Util

