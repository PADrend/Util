/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Resource.h"
#include "../Devices/Device.h"
#include "../Macros.h"

namespace Util {
namespace {

uint64_t generateResourceId(Resource* resource) {
	static uint64_t counter = 0;
	return ++counter | (static_cast<uint64_t>(resource->getTypeId().getValue()) << 32);
}

} // _

//---------------

Resource::Resource() : id(generateResourceId(this)) {
}


//---------------

Resource::~Resource() {
	releaseAll();
}

//---------------

void Resource::releaseAll() {
	std::unordered_set<WeakPointer<Device>> tempOwners;
	tempOwners.swap(owners);
	for(auto& owner : tempOwners) {
		owner->releaseResource(this);
	}
}

//---------------

void Resource::removeOwner(WeakPointer<Device> device) {
	if(!device)
		return;
	owners.erase(device);
}

//---------------

void Resource::addOwner(WeakPointer<Device> device) {
	owners.emplace(device);
}

//---------------

bool Resource::isOwnedBy(WeakPointer<Device> device) const {
	return owners.find(device) != owners.end();
}

//---------------

} /* Util */