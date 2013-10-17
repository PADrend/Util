/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericAttributeSerialization.h"
#include "Factory/WrapperFactory.h"
#include "GenericAttribute.h"
#include "JSON_Parser.h"
#include "StringIdentifier.h"
#include "StringUtils.h"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <utility>

namespace Util {

GenericAttributeSerialization::serializer_factory_t & GenericAttributeSerialization::getSerializerFactory() {
	static serializer_factory_t factory;
	return factory;
}
GenericAttributeSerialization::unserializer_factory_t & GenericAttributeSerialization::getUnserializerFactory() {
	static unserializer_factory_t factory;
	return factory;
}

static const std::string GATypeNameBool = "bool";
static GenericAttributeSerialization::serializer_type_t serializeGABool(const GenericAttributeSerialization::serializer_parameter_t & attributeAndContext) {
	auto boolAttribute = dynamic_cast<const BoolAttribute *>(attributeAndContext.first);
	const std::string content = boolAttribute->get() ? "true" : "false";
	return std::make_pair(GATypeNameBool, content);
}
static BoolAttribute * unserializeGABool(const GenericAttributeSerialization::unserializer_parameter_t & contentAndContext) {
	if(contentAndContext.first == std::string("true")) {
		return new BoolAttribute(true);
	} else {
		return new BoolAttribute(false);
	}
}

template<typename> static const std::string getGATypeName();
template<> const std::string getGATypeName<double>()			{return "numberDouble";}
template<> const std::string getGATypeName<float>()			{return "numberFloat";}
template<> const std::string getGATypeName<int>()				{return "numberInt";}
template<> const std::string getGATypeName<unsigned int>()	{return "numberUInt";}
template<> const std::string getGATypeName<long>()			{return "numberLong";}
template<> const std::string getGATypeName<unsigned long>()	{return "numberULong";}
template<> const std::string getGATypeName<short>()			{return "numberShort";}
template<> const std::string getGATypeName<unsigned short>()	{return "numberUShort";}

template<typename NumberType>
static GenericAttributeSerialization::serializer_type_t serializeGANumber(const GenericAttributeSerialization::serializer_parameter_t & attributeAndContext) {
	auto numberAttribute = dynamic_cast<const _NumberAttribute<NumberType> *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << numberAttribute->get();
	return std::make_pair( getGATypeName<NumberType>() , stream.str());
}
template<typename NumberType>
static _NumberAttribute<NumberType> * unserializeGANumber(const GenericAttributeSerialization::unserializer_parameter_t & contentAndContext) {
	NumberType number;
	std::istringstream stream(contentAndContext.first);
	stream >> number;
	return new _NumberAttribute<NumberType>(number);
}

template<> const std::string getGATypeName<char>()			{return "numberByte";}
// Make sure that the character is converted to a number
template<>
GenericAttributeSerialization::serializer_type_t serializeGANumber<char>(const GenericAttributeSerialization::serializer_parameter_t & attributeAndContext) {
	auto numberAttribute = dynamic_cast<const _NumberAttribute<char> *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << static_cast<short>(numberAttribute->get());
	return std::make_pair(getGATypeName<char>(), stream.str());
}
template<>
_NumberAttribute<char> * unserializeGANumber<char>(const GenericAttributeSerialization::unserializer_parameter_t & contentAndContext) {
	short number;
	std::istringstream stream(contentAndContext.first);
	stream >> number;
	return new _NumberAttribute<char>(number);
}

template<> const std::string getGATypeName<unsigned char>()	{return "numberUByte";}
// Make sure that the unsigned character is converted to a number
template<>
GenericAttributeSerialization::serializer_type_t serializeGANumber<unsigned char>(const GenericAttributeSerialization::serializer_parameter_t & attributeAndContext) {
	auto numberAttribute = dynamic_cast<const _NumberAttribute<unsigned char> *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << static_cast<unsigned short>(numberAttribute->get());
	return std::make_pair(getGATypeName<unsigned char>(), stream.str());
}
template<>
_NumberAttribute<unsigned char> * unserializeGANumber<unsigned char>(const GenericAttributeSerialization::unserializer_parameter_t & contentAndContext) {
	unsigned short number;
	std::istringstream stream(contentAndContext.first);
	stream >> number;
	return new _NumberAttribute<unsigned char>(number);
}

template<> const std::string getGATypeName<std::string>()	{return "string";}
template<typename StringType>
GenericAttributeSerialization::serializer_type_t serializeGAString(const GenericAttributeSerialization::serializer_parameter_t & attributeAndContext) {
	auto stringAttribute = dynamic_cast<const _StringAttribute<StringType> *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << stringAttribute->get();
	return std::make_pair(getGATypeName<StringType>(), stream.str());
}
template<typename StringType>
StringAttribute * unserializeGAString(const GenericAttributeSerialization::unserializer_parameter_t & contentAndContext) {
	return new _StringAttribute<StringType>(contentAndContext.first);
}

static const std::string GATypeNameList = "list";
static GenericAttributeSerialization::serializer_type_t serializeGAList(const GenericAttributeSerialization::serializer_parameter_t & attributeAndContext) {
	auto listAttribute = dynamic_cast<const GenericAttributeList *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << '[';
	for(auto it = listAttribute->begin(); it != listAttribute->end(); ++it) {
		if(it != listAttribute->begin()) {
			stream << ',';
		}
		stream << '"' << StringUtils::escape(GenericAttributeSerialization::serialize(it->get())) << '"';
	}
	stream << ']';
	return std::make_pair(GATypeNameList, stream.str());
}
static GenericAttributeList * unserializeGAList(const GenericAttributeSerialization::unserializer_parameter_t & contentAndContext) {
	std::unique_ptr<GenericAttribute> JSONObject(JSON_Parser::parse(contentAndContext.first));
	GenericAttributeList * objectList = dynamic_cast<GenericAttributeList *>(JSONObject.get());
	if(objectList == nullptr) {
		throw std::logic_error("Invalid JSON representation: List expected");
	}
	auto list = new GenericAttributeList;
	for(auto & elem : *objectList) {
		list->push_back(GenericAttributeSerialization::unserialize(elem->toString()));
	}
	return list;
}

static const std::string GATypeNameMap = "map";
static GenericAttributeSerialization::serializer_type_t serializeGAMap(const GenericAttributeSerialization::serializer_parameter_t & attributeAndContext) {
	auto mapAttribute = dynamic_cast<const GenericAttributeMap *>(attributeAndContext.first);
	std::ostringstream stream;
	stream << '{';
	for(auto it = mapAttribute->begin(); it != mapAttribute->end(); ++it) {
		if(it != mapAttribute->begin()) {
			stream << ',';
		}
		stream << '"' << it->first.toString() << "\":\"" << StringUtils::escape(GenericAttributeSerialization::serialize(it->second.get())) << '"';
	}
	stream << '}';
	return std::make_pair(GATypeNameMap, stream.str());
}
static GenericAttributeMap * unserializeGAMap(const GenericAttributeSerialization::unserializer_parameter_t & contentAndContext) {
	std::unique_ptr<GenericAttribute> JSONObject(JSON_Parser::parse(contentAndContext.first));
	GenericAttributeMap * objectMap = dynamic_cast<GenericAttributeMap *>(JSONObject.get());
	if(objectMap == nullptr) {
		throw std::logic_error("Invalid JSON representation: Map expected");
	}
	auto map = new GenericAttributeMap;
	for(auto & elem : *objectMap) {
		map->setValue(elem.first, GenericAttributeSerialization::unserialize(elem.second->toString()));
	}
	return map;
}

bool GenericAttributeSerialization::init() {
	static int initializationCount = 0;
	if(initializationCount++ == 0) {
 		registerSerializer<BoolAttribute>(GATypeNameBool, serializeGABool, unserializeGABool);
		registerSerializer<_NumberAttribute<double>>(getGATypeName<double>(), serializeGANumber<double>, unserializeGANumber<double>);
		registerSerializer<_NumberAttribute<float>>(getGATypeName<float>(), serializeGANumber<float>, unserializeGANumber<float>);
		registerSerializer<_NumberAttribute<long>>(getGATypeName<long>(), serializeGANumber<long>, unserializeGANumber<long>);
		registerSerializer<_NumberAttribute<unsigned long>>(getGATypeName<unsigned long>(), serializeGANumber<unsigned long>, unserializeGANumber<unsigned long>);
		registerSerializer<_NumberAttribute<int>>(getGATypeName<int>(), serializeGANumber<int>, unserializeGANumber<int>);
		registerSerializer<_NumberAttribute<unsigned int>>(getGATypeName<unsigned int>(), serializeGANumber<unsigned int>, unserializeGANumber<unsigned int>);
		registerSerializer<_NumberAttribute<short>>(getGATypeName<short>(), serializeGANumber<short>, unserializeGANumber<short>);
		registerSerializer<_NumberAttribute<unsigned short>>(getGATypeName<unsigned short>(), serializeGANumber<unsigned short>, unserializeGANumber<unsigned short>);
 		registerSerializer<_NumberAttribute<char>>(getGATypeName<char>(), serializeGANumber<char>, unserializeGANumber<char>);
 		registerSerializer<_NumberAttribute<unsigned char>>(getGATypeName<unsigned char>(), serializeGANumber<unsigned char>, unserializeGANumber<unsigned char>);
 		registerSerializer<_StringAttribute<std::string>>(getGATypeName<std::string>(), serializeGAString<std::string>, unserializeGAString<std::string>);
 		registerSerializer<GenericAttributeList>(GATypeNameList, serializeGAList, unserializeGAList);
		registerSerializer<GenericAttributeMap>(GATypeNameMap, serializeGAMap, unserializeGAMap);
	}
	return true;
}

std::string GenericAttributeSerialization::serialize(const GenericAttribute * attribute) {
	return serialize(attribute, nullptr);
}

std::string GenericAttributeSerialization::serialize(const GenericAttribute * attribute,
													 const GenericAttributeMap * context) {
	const auto typeValuePair = getSerializerFactory().create(typeid(*attribute), std::make_pair(attribute, context));
	return std::string("{\"type\":\"") + typeValuePair.first + std::string("\",\"value\":\"") + StringUtils::escape(typeValuePair.second) + std::string("\"}");
}

GenericAttribute * GenericAttributeSerialization::unserialize(const std::string & representation) {
	return unserialize(representation, nullptr);
}

GenericAttribute * GenericAttributeSerialization::unserialize(const std::string & representation,
															  const GenericAttributeMap * context) {
	std::unique_ptr<GenericAttribute> JSONObject(JSON_Parser::parse(representation));
	GenericAttributeMap * objectMap = dynamic_cast<GenericAttributeMap *>(JSONObject.get());
	if(objectMap == nullptr) {
		throw std::logic_error("Invalid JSON representation: Map expected");
	}
	static const StringIdentifier TYPE("type");
	static const StringIdentifier VALUE("value");
	return getUnserializerFactory().create(objectMap->getString(TYPE), std::make_pair(objectMap->getString(VALUE), context));
}

}
