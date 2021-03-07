/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <limits>

namespace Util {
	
//! @defgroup strings Strings

//! @ingroup strings
namespace StringUtils {

//! Does subject begin with @p find?  
UTILAPI bool beginsWith(const char * subject,const char * find);
UTILAPI bool beginsWith(std::istream & is,const std::string & find);

//! Create a random string out of digits, and lower and uppercase letters.
UTILAPI std::string createRandomString(uint64_t length);

//!	Escape quotes, newlines and backslashes.
UTILAPI std::string escape(const std::string & s);

/**
 * Convert the given string containing white space separated number representations and append the numbers to the given double-ended queue.
 *
 * @param in String which is taken as input of the conversion.
 * @param out Double-ended queue that is used to add the numbers.
 * @note  ****** Use toFloats(...) instead! *******
 */
UTILAPI void extractFloats(const std::string & in, std::deque<float> & out);

/**
 * Convert the given string containing white space separated number representations and append the numbers to the given double-ended queue.
 *
 * @param in String which is taken as input of the conversion.
 * @param out Double-ended queue that is used to add the numbers.
 */
UTILAPI void extractUnsignedLongs(const std::string & in, std::deque<unsigned long> & out);

/*! Read the next line.   */
UTILAPI std::string getLine(const char * subject);

template <typename Iterator_T>
std::string implode(const Iterator_T & begin,const Iterator_T & end,const std::string & delimiter ) {
	std::ostringstream s;
	auto it = begin;
	if(it!=end){
		s << *it;
		for(++it;it!=end;++it)
			s << delimiter << *it;
	}
	return s.str();
}


//! Move cursor to the next line. Return false if end of subject is reached. 
UTILAPI bool nextLine(const char * subject,int & cursor);

static const uint32_t INVALID_UNICODE_CODE_POINT = (std::numeric_limits<uint32_t>::max)();

/*! Reads the next UTF8 code point from the given @p string at the given @p pos.
	@return [UnicodeCodePoint, numberOfBytes]; 
		If the code point lies beyond the string, numberOfBytes is 0.
		If no valid code point could be read (e.g. formatting error), UnicodeCodePoint is INVALID_UNICODE_CODE_POINT. */
UTILAPI std::pair<uint32_t,uint8_t> readUTF8Codepoint(const std::string & str,const uint64_t pos);

/*! Read and return the content of a quoted string "foo bar". The cursor is placed after the string.
	If no quoted string is found at the beginning of the stream, the cursor is not moved.   */
UTILAPI std::string readQuotedString(const char * subject,int & cursor);


typedef std::pair<const std::string, std::string> keyValuePair;

//! Replace all(or maximal count) occurrences of find in subject by replace. 
UTILAPI std::string replaceAll(const std::string &subject,const std::string &find,const std::string &replace,int count=-1);
UTILAPI std::string replaceMultiple(const std::string &subject,int replaceCount,const std::string find[],const std::string replace[],int max=-1);
UTILAPI std::string replaceMultiple(const std::string &subject,const std::deque<keyValuePair > & findReplace,int max=-1);

/*! If subject[cursor] begins with @p search, the @p cursor is moved behind that text and true is returned.
	Otherwise, false is returned. */
UTILAPI bool stepText(const char * subject,int & cursor,const char * search);

//! Whitespace chars are skipped.
UTILAPI void stepWhitespaces(const char * subject,int & cursor);
UTILAPI void stepWhitespaces(std::istream & is);
UTILAPI void stepWhitespaces(std::istream & is,int & line);
UTILAPI void stepWhitespaces(char ** cursor);

/**
 * Convert the given string to a number.
 *
 * @param in String containing a number representation.
 * @return Number as type determined by template parameter.
 */
template<typename _T>
inline static _T toNumber(const std::string & in) {
	std::istringstream stream(in);
	_T out;
	stream >> out;
	return out;
}
UTILAPI bool toBool(const std::string & s);

//! e.g. "0 1 -4 6.0" -> [false, true, true, true] 
UTILAPI std::deque<bool> toBools(const std::string & s);
//! e.g. "0 1 -4 6.0" -> [0.0f,1.0f,-4.0f,6.0f]
UTILAPI std::vector<float> toFloats(const std::string & s);
//! e.g. "0 1 -4 6.0" -> [0, 1, -4, 6] 
UTILAPI std::vector<int> toInts(const std::string & s);

//! Strip all whitespaces from the beginning and ending of s.
UTILAPI std::string trim(const std::string & s);

UTILAPI std::string toFormattedString(float i);

template<typename Type>
std::string toString(Type var) {
	std::ostringstream s;
	s << var;
	return s.str();
}
template<> inline std::string toString<unsigned char>(unsigned char var) {
	return StringUtils::toString<unsigned int>(var);
}
template<> inline std::string toString<signed char>(signed char var) {
	return StringUtils::toString<int>(var);
}
template<> inline std::string toString<char>(char var) {
	return StringUtils::toString<int>(var);
}
template<> inline std::string toString<const std::string &>(const std::string & var) {
	return var;
}

UTILAPI std::u32string utf8_to_utf32(const std::string & str_u8);
UTILAPI std::string utf32_to_utf8(const std::u32string & str_u32);
UTILAPI std::string utf32_to_utf8(const uint32_t u32);

}
}

#endif // STRINGUTILS_H
