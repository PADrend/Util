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
#ifndef UTIL_RESOURCES_RESOURCE_FORMAT_H_
#define UTIL_RESOURCES_RESOURCE_FORMAT_H_

#include "../StringIdentifier.h"
#include "../TypeConstant.h"

#include <deque>

namespace Util {

/** ResourceFormat
 * @ingroup resources
 */
class ResourceFormat {
public:
	class Attribute {
		public:
			//static const uint8_t TYPE_R11G11B10Float = 100; //! Special type
			//static const uint8_t TYPE_RGBA8UnormSrgb = 101; //! Special type
						
			Attribute();
			//Attribute(const StringIdentifier& _nameId, uint8_t _dataType, uint16_t _dataSize, uint16_t _offset=0);
			Attribute(const StringIdentifier& _nameId, TypeConstant _dataType, uint8_t _numValues, bool _normalized, uint16_t _offset=0);
			
			bool operator==(const Attribute& other) const;
			bool operator<(const Attribute& other) const;
			std::string toString() const;

			uint16_t getOffset() const { return offset; }
			uint16_t getDataSize() const { return dataSize; }
			uint8_t getNumValues() const { return numValues; }
			TypeConstant getDataType() const { return dataType; }
			bool isNormalized() const { return normalized; }
			StringIdentifier getNameId() const { return nameId; }
			std::string getName() const { return nameId.toString(); }
			bool empty() const { return numValues == 0; }
		private:
			friend class ResourceFormat;
			Attribute(const StringIdentifier& _nameId, TypeConstant _dataType, uint16_t _dataSize, uint8_t _numValues, bool _normalized, uint16_t _offset);
			
			StringIdentifier nameId;
			TypeConstant dataType;
			uint16_t dataSize;
			uint16_t offset;
			uint8_t numValues;
			bool normalized;
	};
	
	using AttributeContainer_t = std::deque<Attribute>;
	
	static const ResourceFormat BYTE_FORMAT;
	
	ResourceFormat(size_t _attributeAlignment=0) : attributeAlignment(_attributeAlignment) {}
	virtual ~ResourceFormat() = default;
	
	/*! Create and add a new attribute to the ResourceFormat.
		\return the new attribute
		\note the owner of the attribute is the ResourceFormat
		\note Before using this function, check a default method can be used instead (e.g. append appendVec3) */
	//const Attribute& appendAttribute(const StringIdentifier& nameId, uint8_t type, uint16_t dataSize);
	const Attribute& appendAttribute(const StringIdentifier& nameId, TypeConstant type, uint8_t numValues, bool normalized=false);
		
	//! Add an attribute with the given name and the given number of float values.
	const Attribute & appendFloat(const Util::StringIdentifier& nameId, uint8_t numValues, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, numValues, normalized);
	}

	//! Add an attribute with the given name and the given number of unsigned int values.
	const Attribute & appendUInt(const Util::StringIdentifier& nameId, uint8_t numValues) {
		return appendAttribute(nameId, TypeConstant::UINT32, numValues, false);
	}

	//! Add an attribute with the given name and the given number of int values.
	const Attribute & appendInt(const Util::StringIdentifier& nameId, uint8_t numValues) {
		return appendAttribute(nameId, TypeConstant::INT32, numValues, false);
	}
		
	/*! Get a reference to the attribute with the corresponding name.
		\return Always returns an attribute.
				If the attribute is not present in the vertex description, it is empty.
		\note The owner of the attribute is the ResourceFormat, so be careful if the
				ResourceFormat is deleted or reassigned.*/
	const Attribute& getAttribute(const StringIdentifier& nameId) const;
	const Attribute& getAttribute(const std::string& name) const {
		return getAttribute(StringIdentifier(name));
	}

	bool hasAttribute(const StringIdentifier& nameId) const;
	bool hasAttribute(const std::string& name) const {
		return hasAttribute(StringIdentifier(name));
	}

	/**
	 * Update an existing attribute of or append a new attribute to the ResourceFormat.
	 *
	 * @param attr Attribute that contains the new data.
	 * @note The offsets of all attributes may be recalculated and therefore old values may become invalid.
	 */
	void updateAttribute(const Attribute& attr);

	size_t getSize() const { return size; }
	size_t getNumAttributes() const { return attributes.size(); }
	const AttributeContainer_t& getAttributes() const { return attributes; }
	bool operator==(const ResourceFormat& other) const;
	bool operator!=(const ResourceFormat& other) const;
	bool operator<(const ResourceFormat& other) const;

	std::string toString() const;
private:
	AttributeContainer_t attributes;
	size_t size = 0;
	size_t attributeAlignment;
};

} /* Util */

#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_FORMAT_H_ */
