/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "StringUtils.h"
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring> /* for strlen, strncmp */
#include <random>
#include <sstream>
#include <cstdint>
#include <stdexcept>

namespace Util {
namespace StringUtils {

using std::ios;

//! (static)
std::string trim(const std::string & s) {
	const char * const whiteSpace = " \t\v\f\r\n";

	size_t startPos = s.find_first_not_of(whiteSpace);
	size_t endPos = s.find_last_not_of(whiteSpace);
	if ((startPos == std::string::npos) || (endPos == std::string::npos)) {
		return std::string();
	} else {
		return s.substr(startPos, endPos - startPos + 1);
	}
}
//! (static)
std::string replaceAll(const std::string &subject,const std::string &find,const std::string &replace,int count) {
	std::ostringstream s;

	size_t cursor=0;
	size_t len=subject.length();
	size_t fLen=find.length();
	//unsigned int pos= std::string::npos;
	int nr=0;
	while (cursor<len&& nr!=count) {
		size_t pos=subject.find(find,cursor);

		//std::cout << " found "<<search<<" at "<< pos<<"\n";
		if (pos == std::string::npos) {
			break;
		}

		if (pos>cursor) {
			s<<subject.substr(cursor,pos-cursor);
			cursor=pos;
		}
		s<<replace;
		cursor+=fLen;
		nr++;
	}

	if (cursor<len) {
		s<<subject.substr(cursor,len-cursor);
	}

	return s.str();
}
/**
 * Ersetzt alle Auftreten von Strings in find[] und ersetzt sie durch das Gegenst�ck in replace[]
 * @param   subject         Worin wird gesucht.
 *          replaceCount    Anzahl der find/replace-Paare
 *          find
 *          replace
 *          max             Wenn >=0, maximale Zahl der Ersetzungen
 */
std::string replaceMultiple(const std::string &subject,int replaceCount,const std::string find[],const std::string replace[],int max) {
	std::ostringstream s;

	unsigned int cursor=0;

	size_t  len=subject.length();
	auto findLen = new size_t[replaceCount];
	auto pos = new size_t[replaceCount];
	for (int i=0;i<replaceCount;i++) {
		// L�nge des Suchstrings
		findLen[i]=find[i].length();
		// Position der ersten Fundstelle
		pos[i]=subject.find(find[i],0);
	}
	int nr=0;
	while (cursor<len&& nr!=max) {
		// select next match
		size_t nextPos = std::string::npos;
		int nextIndex=-1;

		// F�r jedes find,replace-Paar:
		for (int i=0;i<replaceCount;i++) {
			// Nicht mehr vorhanden -> n�chste
			if (pos[i]== std::string::npos) continue;
			// Schon �ber Fundstelle hinweg gesprungen (bei �berlappenden Fundstellen) -> neu suchen
			if (pos[i]<cursor) {
				pos[i]=subject.find(find[i],cursor);
			}
			// Kandidat f�r n�chste Ersetzung?
			if (pos[i]<nextPos) {
				nextIndex=i;
				nextPos=pos[i];
			}
		}
		// Nichts mehr gefunden -> Ende
		if (nextPos== std::string::npos) break;

		// Einf�gen und weiter...
		s<<subject.substr(cursor,nextPos-cursor);
		s<<replace[nextIndex];
		cursor=nextPos+findLen[nextIndex];//findLen[nextIndex];

		nr++;
	}
	// Ende anf�gen
	if (cursor<len) {
		s<<subject.substr(cursor,len-cursor);
	}
	delete [] findLen;
	delete [] pos;
//

	return s.str();
}
/**
 * Ersetzt alle Auftreten von Strings in find[] und ersetzt sie durch das Gegenst�ck in replace[]
 * @param   subject         Worin wird gesucht.
 *          find
 *          replace
 *          max             Wenn >=0, maximale Zahl der Ersetzungen
 */
std::string replaceMultiple(const std::string &subject,const std::deque<keyValuePair> & findReplace,int max){
	std::ostringstream s;

	size_t replaceCount=findReplace.size();


	size_t cursor=0;

	size_t  len=subject.length();
	auto findLen = new size_t[replaceCount];
	auto pos = new size_t[replaceCount];

	unsigned int i=0;
	for(const auto & it : findReplace) {
		// L�nge des Suchstrings
		findLen[i]=it.first.length();
		// Position der ersten Fundstelle
		pos[i]=subject.find(it.first,0);
//		std::cout << "Replace "<<subject<<": "<<(*it).first<<" -> "<<(*it).second <<"\n";
		i++;
	}
	int nr=0;
	while (cursor<len&& nr!=max) {
		// select next match
		size_t nextPos= std::string::npos;
		unsigned int nextReplaceLength=0;
		auto nextReplace=findReplace.begin();

		// F�r jedes find,replace-Paar:
		auto f_it=findReplace.begin();
//		std::list<std::string>::const_iterator r_it=replace.begin();

		for (i=0;i<replaceCount;i++,++f_it) {
			// Nicht mehr vorhanden -> n�chste
			if (pos[i]== std::string::npos) continue;
			// Schon �ber Fundstelle hinweg gesprungen (bei �berlappenden Fundstellen) -> neu suchen
			if (pos[i]<cursor) {
				pos[i]=subject.find((*f_it).first,cursor);
			}
			// Kandidat f�r n�chste Ersetzung?
			if (pos[i]<nextPos) {
				nextReplace=f_it;
				nextPos=pos[i];
				nextReplaceLength=findLen[i];
			}

		}
		// Nichts mehr gefunden -> Ende
		if (nextPos== std::string::npos) break;

		// Einf�gen und weiter...
		s<<subject.substr(cursor,nextPos-cursor);
		s<<(*nextReplace).second;
		cursor=nextPos+nextReplaceLength;

		nr++;
	}
	// Ende anf�gen
	if (cursor<len) {
		s<<subject.substr(cursor,len-cursor);
	}
	delete [] findLen;
	delete [] pos;
//

	return s.str();
}

//! (static)
bool beginsWith(const char * subject,const char * find) {
	return strncmp(subject, find, strlen(find)) == 0;
}

//! (static)
bool beginsWith(std::istream & is,const std::string & find) {
	if(is.peek()!=find.at(0))
		return false;
	std::streampos startPos= is.tellg();

	for(const auto & findPos : find) {
		if( !is.good() || findPos != is.get() ){
			is.seekg(startPos);
			return false;
		}
	}
	is.seekg(startPos);
	return true;
}

//! (static)
bool nextLine(const char * subject,int & cursor) {
	while (subject[cursor]!=0) {
		cursor++;
		if (subject[cursor]=='\n') {
			cursor++;
			return subject[cursor] != 0;
		}
	}
	return false;
}

//! (static)
void stepWhitespaces(const char * subject, int & cursor) {
	while(isspace(subject[cursor]) && subject[cursor] != '\0') {
		++cursor;
	}
}

//! (static)
void stepWhitespaces(std::istream & is) {
	while (is.good() && isspace(is.peek()))
		is.get();
}

//! (static)
void stepWhitespaces(std::istream & is,int & line) {
	char c;
	while (is.good() && isspace(c=is.peek())) {
		if(c=='\n') ++line;
		is.get();
	}
}

void stepWhitespaces(char ** cursor) {
	while (isspace(**cursor))
		++(*cursor);
}

//! (static)
bool stepText(const char * subject,int & cursor,const char * search) {
	int _cursor = cursor;
	while (true) {
		char s=*search;
		if (s==0) {
			cursor=_cursor;
			return true;
		}
		if (subject[_cursor]!=s ||subject[_cursor]==0 )
			return false;
		_cursor++;
		search++;
	}
}

//! (static)
std::string getLine(const char * subject) {
	const char * end = subject;
	while (*end != '\0' && *end != '\n') {
		++end;
	}
	return std::string(subject, end - subject);
}
//! (static)
std::string readQuotedString(const char * subject,int & cursor) {
	char c=subject[cursor];
	if (c!='"' && c!='\'')
		return "";

	std::string s;
	char begin=c;
	cursor++;
	while (true){
		c=subject[cursor];

		// escape
		if (c=='\\'){
			cursor++;
			c=subject[cursor];
			if (c==0)
				break;// TODO: Warn?
			s+=c;
			cursor++;
			continue;
		}
		else if (c==0)
			break;// TODO: Warn?

		else if (c==begin){
			cursor++;
			break;
		}else{
			s+=c;
			cursor++;
		}
	}
	return s;
}

//! (static)
std::string toFormattedString(float f) {
	const bool neg = f<0;
	if(neg)
		f=-f;
	int exp=0;
	while (f>=1000.0){
		f/=1000.0;
		exp++;
	}
	while (f!=0.0&&f<0.1){
		f*=1000.0;
		exp--;
	}
	char c[20];
	if(neg)
		sprintf(c, "%1.3f", -f);
	else
		sprintf(c, "%1.3f", f);
	std::string s(c);
	s+=' ';

	switch (exp){
		case 0:
			s+=' ';
			break;// 1
		case 1:
			s+='K';
			break;//Kilo
		case 2:
			s+='M';
			break;//Mega
		case 3:
			s+='G';
			break;//Giga
		case 4:
			s+='T';
			break;//Tera
		case 5:
			s+='P';
			break;//Peta
		case 6:
			s+='E';
			break;//Exa
		case -1:
			s+='m';
			break;//milli
		case -2:
			s+='u';
			break;//micro
		case -3:
			s+='n';
			break;//nano
		case -4:
			s+='p';
			break;//pico
		case -5:
			s+='f';
			break;//femto
		case -6:
			s+='a';
			break;//atto
		default:
			if(exp<0)
				s+='E'+toString(3*exp);
			else
				s+="E+"+toString(3*exp);
			break;//unknown
	}
	return s;
}

//! (static)
std::vector<float> toFloats(const std::string & s){
	std::deque<float> values;
	extractFloats(s, values);
	return std::vector<float>(values.begin(), values.end());
}

//! (static) 
std::vector<int> toInts(const std::string & s) {
	std::vector<int> out;
	char * ptr = const_cast<char *> (s.c_str());
	char * ptrOld = nullptr;
	while (*ptr != '\0' && ptrOld != ptr) {
		ptrOld = ptr;
		out.emplace_back(strtol(ptr, &ptr, 10));
	}
	return out;
}

//! (static) 
std::deque<bool> toBools(const std::string & s) {
	std::deque<bool> bools;
	for(const auto & value : toInts(s)) 
		bools.push_back(value != 0);
	return bools;
}

bool toBool(const std::string & s){
	return s=="true" || s=="TRUE" || s=="1";
}

void extractFloats(const std::string & in, std::deque<float> & out) {
	char * ptr = const_cast<char *> (in.c_str());
	char * ptrOld = nullptr;
	while (*ptr != '\0' && ptrOld != ptr) {
		ptrOld = ptr;
		out.push_back(strtof(ptr, &ptr));
	}
}


void extractUnsignedLongs(const std::string & in, std::deque<unsigned long> & out) {
	char * ptr = const_cast<char *> (in.c_str());
	char * ptrOld = nullptr;
	while (*ptr != '\0' && ptrOld != ptr) {
		ptrOld = ptr;
		out.push_back(strtoul(ptr, &ptr, 10));
	}
}

//! (static)
std::string escape(const std::string & s) {
	std::deque<keyValuePair> replace;
	replace.emplace_back("\"", "\\\"");
	replace.emplace_back("\n", "\\n");
	replace.emplace_back("\\", "\\\\");
	return replaceMultiple(s, replace);
}

//! (static)
std::string createRandomString(size_t length){
	static const char * const chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 62 chars
	static const size_t size = 62; // = strlen(chars);
	static std::default_random_engine engine;
	std::uniform_int_distribution<size_t> distribution(0, size - 1);

	std::string s(length, '\0');
	for (uint_fast8_t i = 0; i < length; ++i) {
		s[i] = chars[distribution(engine)];
	}
	return s;
}


//! (static)
std::pair<uint32_t,uint8_t> readUTF8Codepoint(const std::string & str,const size_t pos){
	const uint8_t* cursor = reinterpret_cast<const uint8_t*>(str.c_str())+pos;
	const uint8_t* end = reinterpret_cast<const uint8_t*>(str.c_str())+str.length();
	
	uint32_t codePoint = INVALID_UNICODE_CODE_POINT;
	uint8_t byteLength = 0;
	
	if(cursor < end ){
		const uint8_t byte0 = *cursor;
		if(byte0<0x80){ // 1 byte (ascii)
			byteLength = 1;
			codePoint = static_cast<uint32_t>(byte0);
		}else if(byte0<0xC2){ 
			byteLength = 1;	// step over illegal value
		}else if(byte0<0xE0){ // 2 byte sequence
			if(cursor+1 < end){
				const uint8_t byte1 = *(cursor+1);
				if( (byte1&0xC0) == 0x80 ){
					byteLength = 2;
					codePoint = (static_cast<uint32_t>(byte0&0x1F) << 6) + (byte1&0x3F) ;
				}else{
					byteLength = 1; // step over illegal value
				}
			}
		}else if(byte0<0xF0){ // 3 byte sequence
			if(cursor+2 < end){
				const uint8_t byte1 = *(cursor+1);
				const uint8_t byte2 = *(cursor+2);
				if( (byte1&0xC0) == 0x80 && (byte2&0xC0) == 0x80 ){
					byteLength = 3;
					codePoint = (static_cast<uint32_t>(byte0&0x0F) << 12) + 
								(static_cast<uint32_t>(byte1&0x3F) << 6) + 
								(byte2&0x3F) ;
				}else{
					byteLength = 1; // step over illegal value
				}
			}
		}else if(byte0<0xF5){ // 4 byte sequence
			if(cursor+3 < end){
				const uint8_t byte1 = *(cursor+1);
				const uint8_t byte2 = *(cursor+2);
				const uint8_t byte3 = *(cursor+3);
				if( (byte1&0xC0) == 0x80 && (byte2&0xC0) == 0x80 && (byte3&0xC0) == 0x80 ){
					byteLength = 4;
					codePoint = (static_cast<uint32_t>(byte0&0x07) << 18) + 
								(static_cast<uint32_t>(byte1&0x3F) << 12) + 
								(static_cast<uint32_t>(byte2&0x3F) << 6) + 
								(byte3&0x3F);
				}else{
					byteLength = 1; // step over illegal value
				}
			}
		}
	}	
	return std::make_pair(codePoint,byteLength);
}


std::u32string utf8_to_utf32(const std::string & str_u8) {
	std::u32string utf32String;
	utf32String.reserve(str_u8.length()); // just a guess.
	size_t cursor = 0;
	while(true) {
		const auto codePoint = readUTF8Codepoint(str_u8, cursor);
		if(codePoint.second == 0) {
			break;
		}
		utf32String.push_back(codePoint.first);
		cursor += codePoint.second;
	}
	return utf32String;
}

/*
 * The code below does neither compile with g++ 4.7.1 nor g++ 4.8.1. libstdc++
 * lacks support for wstring_convert. Maybe it can be used in the future:
 * 
 * #include <codecvt>
 * #include <locale>
 *
 * std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
 * const std::string utf8String = converter.to_bytes(u32string);
 */

std::string utf32_to_utf8(const std::u32string & str_u32){
	std::string str_u8;
	str_u8.reserve(str_u32.length()); // just a guess.
	for(const uint32_t u32 : str_u32)
		str_u8.append(utf32_to_utf8(u32));
	return str_u8;
}

std::string utf32_to_utf8(const uint32_t u32){
	if(u32<=0x7F){
		return {	static_cast<char>(u32)	};								// 0XXXXXXX
	}else if(u32<=0x7FF){
		return {	static_cast<char>(0xC0 | ( (u32>>6) & 0x1F)),			// 110XXXXX
					static_cast<char>(0x80 | (u32&0x3F))	};				// 10XXXXXX
	}else if(u32<=0xFFFF){
		return {	static_cast<char>(0xE0 | ( (u32>>12) & 0x0F)),			// 1110XXXX
					static_cast<char>(0x80 | ( (u32>>6) & 0x3F)),			// 10XXXXXX
					static_cast<char>(0x80 | (u32&0x3F))	};				// 10XXXXXX
	}else if(u32<=0x13FFFF){
		return {	static_cast<char>(0xF0 | ( (u32>>18) & 0x07)),			// 11110XXX
					static_cast<char>(0x80 | ( (u32>>12) & 0x3F)),			// 10XXXXXX
					static_cast<char>(0x80 | ( (u32>>6) & 0x3F)),			// 10XXXXXX
					static_cast<char>(0x80 | (u32&0x3F))	};				// 10XXXXXX
	}else {
		throw std::invalid_argument("utf32_to_utf8: Invalid unicode codepoint."); // ??? this could also be silently ignored? don't know what is better...
	}
}



}
}

