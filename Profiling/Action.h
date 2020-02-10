/*
	This file is part of the MinSG library extension Profiling.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_PROFILING_ACTION_H_
#define UTIL_PROFILING_ACTION_H_

#include "../StringIdentifier.h"
#include "../GenericAttribute.h"

namespace Util {
namespace Profiling {

//! Forward declaration of Action
typedef Util::GenericAttributeMap Action;

static const Util::StringIdentifier ATTR_description("description");
static const Util::StringIdentifier ATTR_memoryBegin("memoryBegin");
static const Util::StringIdentifier ATTR_memoryEnd("memoryEnd");
static const Util::StringIdentifier ATTR_timeBegin("timeBegin");
static const Util::StringIdentifier ATTR_timeEnd("timeEnd");

}
}

#endif /* UTIL_PROFILING_ACTION_H_ */
