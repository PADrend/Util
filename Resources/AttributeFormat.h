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

#include "../Graphics/PixelFormat.h"
#include "../StringIdentifier.h"
#include "../TypeConstant.h"
#include "../Utils.h"

#include <deque>

namespace Util {

/** AttributeFormat
 * @ingroup resources
 */
struct AttributeFormat {
	AttributeFormat() = default;

	AttributeFormat(const StringIdentifier& _nameId, BaseType _baseType, uint16_t _dataSize, uint8_t _components, bool _normalized, bool _sRGB, uint8_t _internalType, uint64_t _offset) :
			nameId(_nameId), baseType(_baseType), dataSize(_dataSize),
			offset(_offset), components(_components), normalized(_normalized), sRGB(_sRGB), internalType(_internalType) {}

	AttributeFormat(const StringIdentifier& _nameId, BaseType _baseType, uint8_t _components = 1, bool _normalized = false, bool _sRGB = false, uint8_t _internalType = 0, uint64_t _offset = 0) :
			nameId(_nameId), baseType(_baseType), dataSize(getBlockSizeBytes(Util::getPixelFormat(_baseType, _components, _normalized, _sRGB, _internalType))),
			offset(_offset), components(_components), normalized(_normalized), sRGB(_sRGB), internalType(_internalType) {}

	AttributeFormat(BaseType _baseType, uint8_t _components = 1, bool _normalized = false, bool _sRGB = false, uint8_t _internalType = 0, uint64_t _offset = 0) :
			AttributeFormat(StringIdentifier(), _baseType, _components, _normalized, _sRGB, _internalType, _offset) {}

	AttributeFormat(const StringIdentifier& _nameId, PixelFormat format, uint64_t _offset = 0) :
			AttributeFormat(_nameId, getBaseType(format), getChannelCount(format), isNormalized(format), isSRGB(format), getInternalTypeId(format), _offset) {}

	AttributeFormat(PixelFormat format, uint64_t _offset = 0) :
			AttributeFormat(StringIdentifier(), format, _offset) {}

	//! The name of the attribute.
	StringIdentifier nameId;
	//! The base type of each component.
	BaseType baseType = BaseType::UInt8;
	//! The size (in bytes) of the entire attribute. For compressed formats, this is the block size.
	uint16_t dataSize = 1;
	//! The number of components of the attribute.
	uint8_t components = 1;
	//! Specifies if the underlying type is automatically converted to/from float value in the range [-1.0,1.0] or [0.0,1.0]
	bool normalized = false;
	//! Specifies if the underlying type should be interpreted as sRGB color values.
	bool sRGB = false;
	//! The user defined internal type id of the attribute (e.g., for compressed data).
	uint8_t internalType = 0;
	//! The byte offset of the attribute within a @p ResourceFormat.
	uint64_t offset = 0;

	bool operator==(const AttributeFormat& o) const {
		return nameId == o.nameId && baseType == o.baseType && dataSize == o.dataSize && offset == o.offset && components == o.components && normalized == o.normalized && sRGB == o.sRGB && internalType == o.internalType;
	}
	bool operator!=(const AttributeFormat& other) const { return !(*this == other); };
	bool operator<(const AttributeFormat& other) const {
		if (offset != other.offset) {
			return offset < other.offset;
		} else if (dataSize != other.dataSize) {
			return dataSize < other.dataSize;
		} else if (baseType != other.baseType) {
			return baseType < other.baseType;
		} else if (components != other.components) {
			return components < other.components;
		} else if (normalized != other.normalized) {
			return normalized < other.normalized;
		} else if (sRGB != other.sRGB) {
			return sRGB < other.sRGB;
		} else if (internalType != other.internalType) {
			return internalType < other.internalType;
		} else if (nameId != other.nameId) {
			return nameId < other.nameId;
		} else
			return false;
	}

	constexpr PixelFormat getPixelFormat() const { return Util::getPixelFormat(baseType, components, normalized, sRGB, internalType); }

	//! Returns @p true, if the attribute has at least one value
	constexpr bool isValid() const { return components != 0; }

	//! Returns a string representation of this attribute
	UTILAPI std::string toString() const;
};

} // namespace Util

template<> struct std::hash<Util::AttributeFormat> {
	std::uint64_t operator()(const Util::AttributeFormat& format) const {
		std::uint64_t result = format.nameId.getValue();
		Util::hash_combine(result, format.baseType);
		Util::hash_combine(result, format.dataSize);
		Util::hash_combine(result, format.components);
		Util::hash_combine(result, format.normalized);
		Util::hash_combine(result, format.sRGB);
		Util::hash_combine(result, format.internalType);
		Util::hash_combine(result, format.offset);
		return result;
	}
};

#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_ATTRIBUTE_H_ */
