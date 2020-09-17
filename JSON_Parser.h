/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_JSON_PARSER_H
#define UTIL_JSON_PARSER_H

#include <string>

namespace Util{
class GenericAttribute;

/**
 * JSON parser for converting JSON data to GenericAttibutes.
 * @code
 * 	const std::string str = "{"
 * 							" \"1\":2 ,\n"
 * 							" \"foo\":\"bar\" ,"
 * 							" \"true\":false,"
 * 							" \"bar\":true,"
 * 							" \"a\":[1,02e10,3.0,-2.5,\"27\",\"a\\\"a\"] "
 * 							"}";
 * 	GenericAttribute * attr = JSON_Parser::parse(str);
 * 	if(attr != nullptr) {
 * 		std::cout << attr->toJSON() << std::endl;
 * 	}
 * @endcode
 * @ingroup util_helper
 */
class JSON_Parser {
	public:
		//! Try to convert a JSON formatted string into a GenericAttribute.
		UTILAPI static GenericAttribute * parse(const std::string & str);
};

}
#endif // UTIL_JSON_PARSER_H
