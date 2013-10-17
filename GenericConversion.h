/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_GENERICCONVERSION_H
#define UTIL_GENERICCONVERSION_H

#include <iosfwd>

namespace Util {
class Generic;
/**
 * @brief Conversion to and from Generic
 * 
 * Conversion functions to convert from other formats to Generic and from
 * Generic to other formats.
 */
namespace GenericConversion {

/**
 * Read JavaScript Object Notation (JSON) from the given stream and convert the
 * data to Generic.
 * 
 * @param in Stream containing JSON data
 * @return Generic representation of the JSON data
 */
Generic fromJSON(std::istream & in);

/**
 * Convert the given data to JavaScript Object Notation (JSON) and write it to
 * the given stream.
 * 
 * @param data Generic representation of the data
 * @param out Stream to which the JSON data will be written
 */
void toJSON(const Generic & data, std::ostream & out);

}
}

#endif /* UTIL_GENERICCONVERSION_H */
