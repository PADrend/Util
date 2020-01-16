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
	AttributeFormat(const StringIdentifier& _nameId, TypeConstant _dataType, uint8_t _numValues=1, bool _normalized=false, uint32_t _internalType=0, uint16_t _offset=0);
	
	bool operator==(const AttributeFormat& other) const;
	bool operator!=(const AttributeFormat& other) const { return !(*this==other); };
	bool operator<(const AttributeFormat& other) const;
	std::string toString() const;

	uint16_t getOffset() const { return offset; }
	uint16_t getDataSize() const { return dataSize; }
	uint8_t getNumValues() const { return numValues; }
	TypeConstant getDataType() const { return dataType; }
	bool isNormalized() const { return normalized; }
	StringIdentifier getNameId() const { return nameId; }
	std::string getName() const { return nameId.toString(); }
	uint32_t getInternalType() const { return internalType; }
	bool empty() const { return numValues == 0; }
private:
	friend class ResourceFormat;
	AttributeFormat(const StringIdentifier& _nameId, TypeConstant _dataType, uint16_t _dataSize, uint8_t _numValues, bool _normalized, uint32_t _internalType, uint16_t _offset);
	
	StringIdentifier nameId;
	TypeConstant dataType;
	uint16_t dataSize;
	uint16_t offset;
	uint8_t numValues;
	bool normalized;
	uint32_t internalType;
};

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_ATTRIBUTE_H_ */
