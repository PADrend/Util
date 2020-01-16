/*
	This file is part of the Util library.
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_STRUCTUREDACCESSOR_H_
#define UTIL_RESOURCES_STRUCTUREDACCESSOR_H_

#include "ResourceAccessor.h"

namespace Util {
	
/** StructuredAccessor
 * @ingroup resources
 */
template<typename T>
class StructuredAccessor : public ResourceAccessor {
public:
	using Type_t = T;
	
	StructuredAccessor(uint8_t* ptr, size_t size, ResourceFormat format) : ResourceAccessor(ptr, size, format) {}
	virtual ~StructuredAccessor() = default;
	
	Type_t read(size_t index) {
		Type_t value;
		readRaw(index, reinterpret_cast<uint8_t*>(&value));
		return value;
	}
	
	std::vector<Type_t> read(size_t index, size_t count) {
		std::vector<Type_t> values(count);
		readRaw(index, reinterpret_cast<uint8_t*>(values.data()), count);
		return values;
	}
		
	void write(size_t index, const Type_t& value) {
		writeRaw(index, reinterpret_cast<const uint8_t*>(&value));
	}
	
	void write(size_t index, const std::vector<Type_t>& values) {
		writeRaw(index, reinterpret_cast<const uint8_t*>(values.data()), values.size());
	}
};
	
} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_STRUCTUREDACCESSOR_H_ */
