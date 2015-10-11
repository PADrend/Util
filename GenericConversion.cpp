/*
	This file is part of the Util library.
	Copyright (C) 2013-2015 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericConversion.h"
#include "Generic.h"
#include "GenericAttribute.h"
#include "JSON_Parser.h"
#include "StringIdentifier.h"
#include "StringUtils.h"
#include <istream>
#include <ostream>
#include <memory>

namespace Util {
namespace GenericConversion {

typedef std::vector<Generic> GenericArray;
typedef std::unordered_map<StringIdentifier, Generic> GenericMap;

static Generic fromGenericAttribute(const GenericAttribute * attr) {
	{
		auto boolAttr = dynamic_cast<const BoolAttribute *>(attr);
		if(boolAttr != nullptr) {
			return Generic(boolAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<double> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<float> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<long> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<unsigned long> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<int> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<unsigned int> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<short> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<unsigned short> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<char> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto numberAttr = dynamic_cast<const _NumberAttribute<unsigned char> *>(attr);
		if(numberAttr != nullptr) {
			return Generic(numberAttr->get());
		}
	}
	{
		auto stringAttr = dynamic_cast<const _StringAttribute<std::string> *>(attr);
		if(stringAttr != nullptr) {
			return Generic(stringAttr->get());
		}
	}
	{
		auto listAttr = dynamic_cast<const GenericAttributeList *>(attr);
		if(listAttr != nullptr) {
			GenericArray genericArray;
			genericArray.reserve(listAttr->size());
			for(const auto & element : *listAttr) {
				genericArray.emplace_back(fromGenericAttribute(element.get()));
			}
			return Generic(std::move(genericArray));
		}
	}
	{
		auto mapAttr = dynamic_cast<const GenericAttributeMap *>(attr);
		if(mapAttr != nullptr) {
			GenericMap genericMap;
			genericMap.reserve(mapAttr->size());
			for(const auto & element : *mapAttr) {
				genericMap.emplace(element.first, fromGenericAttribute(element.second.get()));
			}
			return Generic(std::move(genericMap));
		}
	}
	return Generic();
}

Generic fromJSON(std::istream & in) {
	// To prevent duplicate code, use the existing JSON parser.
	// Maybe, this can be replaced by a better suited parser in the future.
	
	in.seekg(0, std::ios::end);
	std::string data(static_cast<std::size_t>(in.tellg()), '\0');
	in.seekg(0, std::ios::beg);
	in.read(const_cast<char *>(data.data()), static_cast<std::streamsize>(data.size()));

	JSON_Parser parser;
	std::unique_ptr<GenericAttribute> attr(parser.parse(data));

	return fromGenericAttribute(attr.get());
}

void toJSON(const Generic & data, std::ostream & out) {
	{
		auto boolData = data.get<bool>();
		if(boolData != nullptr) {
			out << (*boolData ? "true" : "false");
			return;
		}
	}
	{
		auto numberData = data.get<double>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<float>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<long>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<unsigned long>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<int>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<unsigned int>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<short>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<unsigned short>();
		if(numberData != nullptr) {
			out << *numberData;
			return;
		}
	}
	{
		auto numberData = data.get<char>();
		if(numberData != nullptr) {
			out << static_cast<short>(*numberData);
			return;
		}
	}
	{
		auto numberData = data.get<unsigned char>();
		if(numberData != nullptr) {
			out << static_cast<unsigned short>(*numberData);
			return;
		}
	}
	{
		auto stringData = data.get<std::string>();
		if(stringData != nullptr) {
			out << '"' << Util::StringUtils::escape(*stringData) << '"';
			return;
		}
	}
	{
		auto arrayData = data.get<GenericArray>();
		if(arrayData != nullptr) {
			out << '[';
			bool first = true;
			for(const auto & element : *arrayData) {
				if(first) {
					first = false;
				} else {
					out << ",";
				}
				toJSON(element, out);
			}
			out << ']';
			return;
		}
	}
	{
		auto mapData = data.get<GenericMap>();
		if(mapData != nullptr) {
			out << '{';
			bool first = true;
			for(const auto & element : *mapData) {
				if(first) {
					first = false;
				} else {
					out << ",";
				}
				out << '"' << Util::StringUtils::escape(element.first.toString()) << "\":";
				toJSON(element.second, out);
			}
			out << '}';
			return;
		}
	}
}

}
}
