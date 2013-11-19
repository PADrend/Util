/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericAttribute.h"
#include <sstream>
#include <utility>

namespace Util {

// ----------------------------------

/**
 * [static] Factory
 */
StringAttribute * GenericAttribute::createString(GenericAttribute * a){
	if(a==nullptr)
		return new StringAttribute("");
	else
		return new StringAttribute(a->toString());
}

BoolAttribute * GenericAttribute::createBool(bool b){
	return new BoolAttribute(b);
}

// ------------------------------------------------------------------------

GenericAttributeMap::GenericAttributeMap() : m() {
}

GenericAttributeMap::~GenericAttributeMap() = default;

GenericAttributeMap * GenericAttributeMap::clone() const {
	auto mapClone = new GenericAttributeMap;
	for(const auto & element : m) {
		if(element.second) {
			mapClone->setValue(element.first, element.second->clone());
		}
	}
	return mapClone;
}

void GenericAttributeMap::setValue(const StringIdentifier & key, GenericAttribute * attr) {
	m[key].reset(attr);
}

bool GenericAttributeMap::unsetValue(const StringIdentifier & key) {
	return m.erase(key) > 0;
}


void GenericAttributeMap::setString(const StringIdentifier & key, const std::string & value) {
	setValue(key, GenericAttribute::createString(value));
}

GenericAttribute * GenericAttributeMap::getValue(const StringIdentifier & key) const {
	auto it = m.find(key);
	if(it == m.end()) {
		return nullptr;
	}
	return it->second.get();
}

bool GenericAttributeMap::contains(const StringIdentifier & stringId) const {
	return m.count(stringId) != 0;
}

std::string GenericAttributeMap::getString(const StringIdentifier & key, const std::string & defaultValue) const {
	GenericAttribute * attr = getValue(key);
	return attr ? attr->toString() : defaultValue;
}

std::string GenericAttributeMap::toJSON() const {
	std::ostringstream s;
	s << "{";
	bool first = true;
	for(const auto & element : m) {
		if(element.second) {
			if(first) {
				first = false;
			} else {
				s << ",";
			}
			s << "\"" << element.first.toString() << "\":" << element.second->toJSON();
		}
	}
	s << "}";
	return s.str();
}

std::string GenericAttributeMap::toString() const {
	std::ostringstream s;
	s << "Map {\n";
	for(const auto & element : m) {
		if(element.second) {
			s << "\t\"" << element.first.toString() << "\" : "
			  << StringUtils::replaceAll(StringUtils::trim(element.second->toString()), "\n", "\n\t") << "\n";
		}
	}
	s << "}\n";
	return s.str();
}

//! ---|> GenericAttribute
bool GenericAttributeMap::operator==(const GenericAttribute & other)const{
	const attr_t * otherMap = (&other)->toType<const attr_t>();
	if(otherMap==nullptr)
		return false;
	if(size()!=otherMap->size())
		return false;
	for(const auto & element : m) {
		const auto & key = element.first;
		const auto & value = element.second;
		const auto & otherValue = otherMap->getValue(key);
		
		if(value==nullptr && otherValue==nullptr && otherMap->contains(key))
			continue;
		
		if(otherValue==nullptr || (*value != *otherValue))
			return false;
	}
	return true;
}

// ------------------------------------------------------------------------------

GenericAttributeList::GenericAttributeList() : l() {
}

GenericAttributeList::~GenericAttributeList() = default;

GenericAttributeList * GenericAttributeList::clone() const {
	auto listClone = new GenericAttributeList;
	for(const auto & element : l) {
		if(element) {
			listClone->push_back(element->clone());
		}
	}
	return listClone;
}

std::string GenericAttributeList::toJSON() const {
	std::ostringstream s;
	s << "[";
	bool first = true;
	for(const auto & element : l) {
		if(element != nullptr) {
			if(first)
				first = false;
			else
				s << ",";
			s << element->toJSON();
		}
	}
	s << "]";
	return s.str();
}

std::string GenericAttributeList::toString() const {
	std::ostringstream s;
	s << "List [\n";
	for(const auto & element : l) {
		if(element != nullptr) {
			s << "\t" << StringUtils::replaceAll(StringUtils::trim(
					element->toString()), "\n", "\n\t") << "\n";
		}
	}
	s << "]\n";
	return s.str();
}

std::string GenericAttributeList::implode(const std::string & separator) const {
	std::ostringstream s;
	bool first = true;
	for(const auto & element : l) {
		if(element != nullptr) {
			if(first) {
				first = false;
			} else {
				s << separator;
			}
			s << element->toString();
		}
	}
	return s.str();
}

GenericAttribute * GenericAttributeList::at(int index) const {
	if(index < 0) {
		return nullptr;
	}
	for(const auto & element : l) {
		if(index-- == 0) {
			return element.get();
		}
	}
	return nullptr;
}

//! ---|> GenericAttribute
bool GenericAttributeList::operator==(const GenericAttribute & other)const{
	const attr_t * otherList = (&other)->toType<const attr_t>();
	if(otherList==nullptr)
		return false;
	if(size()!=otherList->size())
		return false;
	int i = 0;
	for(const auto & value : l) {
		const auto & otherValue = otherList->at(i++);
		if( (value==nullptr && otherValue!=nullptr) || (*value != *otherValue))
			return false;
	}
	return true;
}

}
