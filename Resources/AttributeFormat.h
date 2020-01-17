/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_RESOURCE_ATTRIBUTE_H_
#define UTIL_RESOURCES_RESOURCE_ATTRIBUTE_H_

#include "../StringIdentifier.h"
#include "../TypeConstant.h"
#include "../Utils.h"

#include <deque>

namespace Util {

/** AttributeFormat
 * @ingroup resources
 */
class AttributeFormat {
public:
	AttributeFormat();
	AttributeFormat(const StringIdentifier& _nameId, TypeConstant _dataType, uint32_t _components=1, bool _normalized=false, uint32_t _internalType=0, size_t _offset=0);

	bool operator==(const AttributeFormat& other) const;
	bool operator!=(const AttributeFormat& other) const { return !(*this==other); };
	bool operator<(const AttributeFormat& other) const;

	//! Returns the name of the attribute.
	std::string getName() const { return nameId.toString(); }
	//! Returns the name of the attribute as a @p StringIdentifier.
	StringIdentifier getNameId() const { return nameId; }
	//! Returns the base type of each component.
	TypeConstant getDataType() const { return dataType; }
	//! Returns the size (in bytes) of the entire attribute.
	size_t getDataSize() const { return dataSize; }
	//! Returns the number of components of the attribute.
	uint32_t getComponentCount() const { return components; }
	//! Specifies if the underlying type is automatically converted to/from float value in the range [-1.0,1.0] or [0.0,1.0]
	bool isNormalized() const { return normalized; }
	//! Returns the user defined internal type id of the attribute (e.g., for compressed data).
	uint32_t getInternalType() const { return internalType; }
	//! Returns the byte offset of the attribute within a @p ResourceFormat.
	size_t getOffset() const { return offset; }

	//! Returns @p true, if the attribute has at least one value
	bool isValid() const { return components != 0; }
	//! Returns a string representation of this attribute
	std::string toString() const;

	//! deprecated alias
	[[deprecated]] TypeConstant getValueType() const { return dataType; }
	//! deprecated alias
	[[deprecated]] uint16_t getBytesPerPixel() const { return dataSize; }
	//! deprecated alias
	[[deprecated]] uint32_t getNumValues() const { return components; }
	//! deprecated alias
	[[deprecated]] uint32_t getNumComponents() const { return components; }
	//! deprecated alias
	[[deprecated]] bool empty() const { return components == 0; }
private:
	friend class ResourceFormat;
	AttributeFormat(const StringIdentifier& _nameId, TypeConstant _dataType, uint16_t _dataSize, uint32_t _components, bool _normalized, uint32_t _internalType, size_t _offset);
	
	StringIdentifier nameId;
	TypeConstant dataType;
	uint16_t dataSize;
	size_t offset;
	uint32_t components;
	bool normalized;
	uint32_t internalType;
};

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_ATTRIBUTE_H_ */
