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

#include "AttributeFormat.h"

#include <sstream>

namespace Util {

//------------------

AttributeFormat::AttributeFormat() : nameId(0), dataType(TypeConstant::UINT8), dataSize(0), offset(0), components(0), normalized(false), internalType(0) { }

//------------------

AttributeFormat::AttributeFormat(const StringIdentifier& _nameId, TypeConstant _dataType, uint32_t _components, bool _normalized, uint32_t _internalType, size_t _offset) :
	nameId(_nameId), dataType(_dataType), dataSize(getNumBytes(_dataType)*_components), 
	offset(_offset), components(_components), normalized(_normalized), internalType(_internalType) { }

//------------------

AttributeFormat::AttributeFormat(const StringIdentifier& _nameId, TypeConstant _dataType, uint16_t _dataSize, uint32_t _components, bool _normalized, uint32_t _internalType, size_t _offset) :
	nameId(_nameId), dataType(_dataType), dataSize(_dataSize), 
	offset(_offset), components(_components), normalized(_normalized), internalType(_internalType) { }

//------------------

bool AttributeFormat::operator==(const AttributeFormat& o) const {
	return nameId == o.nameId && dataType == o.dataType && dataSize == o.dataSize && offset == o.offset && components == o.components && normalized == o.normalized && internalType == o.internalType;
}

//------------------

bool AttributeFormat::operator<(const AttributeFormat& other) const {
	if(offset!=other.offset) {
		return offset<other.offset;
	} else if(components!=other.components) {
		return components<other.components;
	} else if(dataSize!=other.dataSize) {
		return dataSize<other.dataSize;
	} else if(dataType!=other.dataType) {
		return dataType<other.dataType;
	} else if(nameId!=other.nameId) {
		return nameId<other.nameId;
	} else if(normalized!=other.normalized) {
		return normalized<other.normalized;
	} else if(internalType!=other.internalType) {
		return internalType<other.internalType;
	} else return false;
}

//------------------

std::string AttributeFormat::toString() const {
	std::ostringstream s;
	s << nameId.toString() << " (" << offset << "): ";	
	s << static_cast<unsigned int>(components) << " " << getTypeString(dataType);	
	if(normalized)
		s << " (normalized)";	
	if(internalType>0)
		s << " (internalType: " << internalType << ")";	
	return s.str();
}

//------------------

} /* Util */
