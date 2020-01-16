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

#include "ResourceFormat.h"

#include <sstream>

namespace Util {

//------------------


const AttributeFormat& ResourceFormat::appendAttribute(const StringIdentifier& nameId, TypeConstant type, uint32_t components, bool normalized, uint32_t internalType) {
	size_t offset = align(size, attributeAlignment);
	attributes.emplace_back(std::move(AttributeFormat(nameId, type, components, normalized, internalType, offset)));
	size = align(offset + attributes.back().dataSize, attributeAlignment);
	return attributes.back();
}

//------------------

const AttributeFormat& ResourceFormat::getAttribute(const StringIdentifier& nameId) const {
	static const AttributeFormat emptyAttribute;
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

const uint32_t ResourceFormat::getAttributeLocation(const StringIdentifier& nameId) const {
	for(uint16_t i=0; i<attributes.size(); ++i) {
		if(attributes[i].nameId == nameId) {
			return i;
		}
	}
	return attributes.size();
}

//------------------

void ResourceFormat::updateAttribute(const AttributeFormat& attr, bool recalculateOffsets) {
	// Find existing attribute
	for(auto it = attributes.begin(); it != attributes.end(); ++it) {
		AttributeFormat& currentAttr = *it;
		if(currentAttr.getNameId() == attr.getNameId()) {
			currentAttr = AttributeFormat(attr.nameId, attr.dataType, attr.dataSize, attr.components, attr.normalized, attr.internalType, recalculateOffsets ? currentAttr.offset : attr.offset);
			size = static_cast<std::size_t>(currentAttr.getOffset() + currentAttr.getDataSize());

			// Update the offsets.
			auto toUpdateIt = it;
			std::advance(toUpdateIt, 1);
			for(; toUpdateIt != attributes.end(); ++toUpdateIt) {
				AttributeFormat & toUpdateAttr = *toUpdateIt;
				if(recalculateOffsets) toUpdateAttr.offset = size;
				size += toUpdateAttr.getDataSize();
			}
			return;
		}
	}
	// AttributeFormat was not found.
	size_t offset = recalculateOffsets ? align(size, attributeAlignment) : attr.offset;
	attributes.emplace_back(std::move(AttributeFormat(attr.nameId, attr.dataType, attr.dataSize, attr.components, attr.normalized, attr.internalType, offset)));
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
