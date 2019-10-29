/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ResourceFormat.h"

#include <sstream>

namespace Util {
	
static size_t align(size_t offset, size_t alignment) {
  return alignment > 1 ? (offset + (alignment - offset % alignment) % alignment) : offset;
}

static ResourceFormat createByteFormat() {
	StringIdentifier byteId("byte");
	ResourceFormat format;
	format.appendAttribute(byteId, TypeConstant::UINT8, 1, false);
	return format;
}

const ResourceFormat ResourceFormat::BYTE_FORMAT = createByteFormat();

//------------------

ResourceFormat::Attribute::Attribute() : nameId(0), dataType(TypeConstant::UINT8), dataSize(0), offset(0), numValues(0), normalized(false) { }

//------------------

//ResourceFormat::Attribute::Attribute(const StringIdentifier& _nameId, uint8_t _dataType, uint16_t _dataSize, uint16_t _offset) :
//	nameId(_nameId), dataType(_dataType), dataSize(_dataSize),  offset(_offset), numValues(1), normalized(false) { }

//------------------

ResourceFormat::Attribute::Attribute(const StringIdentifier& _nameId, TypeConstant _dataType, uint8_t _numValues, bool _normalized, uint16_t _offset) :
	nameId(_nameId), dataType(_dataType), dataSize(getNumBytes(_dataType)*_numValues), 
	offset(_offset), numValues(_numValues), normalized(_normalized) { }

//------------------

ResourceFormat::Attribute::Attribute(const StringIdentifier& _nameId, TypeConstant _dataType, uint16_t _dataSize, uint8_t _numValues, bool _normalized, uint16_t _offset) :
	nameId(_nameId), dataType(_dataType), dataSize(_dataSize), 
	offset(_offset), numValues(_numValues), normalized(_normalized) { }

//------------------

bool ResourceFormat::Attribute::operator==(const Attribute& o) const {
	return nameId == o.nameId && dataType == o.dataType && dataSize == o.dataSize && offset == o.offset && numValues == o.numValues && normalized == o.normalized;
}

//------------------

bool ResourceFormat::Attribute::operator<(const Attribute& other) const {
	if(offset!=other.offset) {
		return offset<other.offset;
	} else if(numValues!=other.numValues) {
		return numValues<other.numValues;
	} else if(dataSize!=other.dataSize) {
		return dataSize<other.dataSize;
	} else if(dataType!=other.dataType) {
		return dataType<other.dataType;
	} else if(nameId!=other.nameId) {
		return nameId<other.nameId;
	} else if(normalized!=other.normalized) {
		return normalized<other.normalized;
	} else return false;
}

//------------------

std::string ResourceFormat::Attribute::toString() const {
	std::ostringstream s;
	s << nameId.toString() << " (" << offset << "): ";
	//if(dataType <= static_cast<uint8_t>(TypeConstant::DOUBLE)) {
		s << static_cast<unsigned int>(numValues) << " " << getTypeString(static_cast<TypeConstant>(dataType));
	//} else if(dataType == TYPE_R11G11B10Float) {
	//	s << "R11G11B10Float";
	//} else if(dataType == TYPE_RGBA8UnormSrgb) {
	//	s << "RGBA8UnormSrgb";
	//} else {
	//	s << "type " << static_cast<uint32_t>(dataType) << ", " << dataSize << " byte";
	//}
	
	if(normalized)
		s << " (normalized)";	
	return s.str();
}

//------------------

//const ResourceFormat::Attribute& ResourceFormat::appendAttribute(const StringIdentifier& nameId, uint8_t type, uint16_t dataSize) {
//	size_t offset = align(size, attributeAlignment);
//	attributes.emplace_back(nameId, type, dataSize, offset);
//	size = align(offset + attributes.back().dataSize, attributeAlignment);
//	return attributes.back();
//}

//------------------

const ResourceFormat::Attribute& ResourceFormat::appendAttribute(const StringIdentifier& nameId, TypeConstant type, uint8_t numValues, bool normalized) {
	size_t offset = align(size, attributeAlignment);
	attributes.emplace_back(nameId, type, numValues, normalized, offset);
	size = align(offset + attributes.back().dataSize, attributeAlignment);
	return attributes.back();
}

//------------------

const ResourceFormat::Attribute& ResourceFormat::getAttribute(const StringIdentifier& nameId) const {
	static const Attribute emptyAttribute;
	for(const auto & attr : attributes) {
		if(attr.getNameId() == nameId) {
			return attr;
		}
	}
	return emptyAttribute;
}

//------------------

bool ResourceFormat::hasAttribute(const StringIdentifier& nameId) const {
	for(const auto & attr : attributes) {
		if(attr.nameId == nameId) {
			return true;
		}
	}
	return false;
}

//------------------

const uint16_t ResourceFormat::getAttributeLocation(const StringIdentifier& nameId) const {
	for(uint16_t i=0; i<attributes.size(); ++i) {
		if(attributes[i].nameId == nameId) {
			return i;
		}
	}
	return attributes.size();
}

//------------------

void ResourceFormat::updateAttribute(const Attribute& attr) {
	for(auto it = attributes.begin(); it != attributes.end(); ++it) {
		Attribute& currentAttr = *it;
		if(currentAttr.getNameId() == attr.getNameId()) {
			currentAttr = Attribute(attr.nameId, attr.dataType, attr.dataSize, attr.numValues, attr.normalized, currentAttr.offset);
			// Update the offsets.
			size = static_cast<std::size_t>(currentAttr.getOffset() + currentAttr.getDataSize());

			auto toUpdateIt = it;
			std::advance(toUpdateIt, 1);
			for(; toUpdateIt != attributes.end(); ++toUpdateIt) {
				Attribute & toUpdateAttr = *toUpdateIt;
				toUpdateAttr.offset = size;
				size += toUpdateAttr.getDataSize();
			}
			return;
		}
	}
	// Attribute was not found.
	size_t offset = align(size, attributeAlignment);
	attributes.emplace_back(Attribute(attr.nameId, attr.dataType, attr.dataSize, attr.numValues, attr.normalized, offset));
	size = align(offset + attributes.back().dataSize, attributeAlignment);
}

//------------------

bool ResourceFormat::operator==(const ResourceFormat& other) const {
	return size == other.size && attributeAlignment == other.attributeAlignment && attributes == other.attributes;
}

//------------------

bool ResourceFormat::operator!=(const ResourceFormat& other) const {
	return size != other.size || attributeAlignment != other.attributeAlignment || attributes != other.attributes;
}

//------------------

bool ResourceFormat::operator<(const ResourceFormat& other) const {
	if(size != other.size) {
		return size < other.size;
	} else if(attributeAlignment != other.attributeAlignment) {
		return attributeAlignment < other.attributeAlignment;
	} else if(attributes.size() != other.attributes.size()) {
		return attributes.size() < other.attributes.size();
	}
	auto it1=getAttributes().begin();
	for(auto it2=other.getAttributes().begin() ; it2!=other.getAttributes().end(); ++it1,++it2) {
		if( !((*it1)==(*it2)) ) {
			return (*it1)<(*it2);
		}
	}
	return false;
}

//------------------

std::string ResourceFormat::toString() const {
	std::ostringstream s;
	s << "(ResourceFormat " << size << " byte";
	for(const auto & attr : getAttributes()) {
		s << ", " << attr.toString();
	}
	s<< ")";
	return s.str();
}

//------------------

} /* Util */
