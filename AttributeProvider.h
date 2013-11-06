/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef ATTRIBUTEPROVIDER_H
#define ATTRIBUTEPROVIDER_H

#include "GenericAttribute.h"
#include "StringIdentifier.h"
#include <memory>

namespace Util {

/**
 * Mixin class to provide a GenericAttributeMap including access functions for classes inheriting it.
 * The map itself is created when the first attribute is added;
 * so that the minimal memory overhead is of the size of a single pointer.
 */
class AttributeProvider {
	private:
		std::unique_ptr<GenericAttributeMap> _attributes;
		inline GenericAttributeMap * _createAndGetAttributes() {
			if(!_attributes)
				_attributes.reset(new GenericAttributeMap);
			return _attributes.get();
		}
	public:
		AttributeProvider() {
		}
		AttributeProvider(const AttributeProvider & other) {
			copyAttributesFrom(other);
		}
		AttributeProvider(AttributeProvider &&) = default;
		AttributeProvider& operator=(const AttributeProvider & other) {
			if(&other != this)
				copyAttributesFrom(other);
			return *this;
		}
		AttributeProvider& operator=(AttributeProvider &&) = default;
		~AttributeProvider() = default;

		void copyAttributesFrom(const AttributeProvider & other) {
			if (other.hasAttributes())
				_attributes.reset(other.getAttributes()->clone());
		}

		GenericAttribute * getAttribute(const StringIdentifier & key) const {
			return _attributes ? _attributes->getValue(key) : nullptr;
		}
		template<class AttrType>
		AttrType * getAttribute(const StringIdentifier & key) const {
			return _attributes ? dynamic_cast<AttrType*>(_attributes->getValue(key)) : nullptr;
		}
		GenericAttributeMap * getAttributes() const {
			return _attributes.get();
		}
		bool hasAttributes() const {
			return _attributes.get() != nullptr;
		}
		bool isAttributeSet(const StringIdentifier & key) const {
			return _attributes && _attributes->contains(key);
		}
		void removeAttributes() {
			_attributes.reset();
		}
		void setAttributes(GenericAttributeMap * newAttributes) {
			_attributes.reset(newAttributes);
		}
		void setAttribute(const StringIdentifier & key, GenericAttribute * value) {
			if(value)
				_createAndGetAttributes()->setValue(key, value);
			else 
				unsetAttribute(key);
		}
		bool unsetAttribute(const StringIdentifier &  key) {
			return _attributes && _attributes->unsetValue(key);
		}

};
}

#endif // ATTRIBUTEPROVIDER_H
