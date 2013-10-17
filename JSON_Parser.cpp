/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "JSON_Parser.h"
#include "GenericAttribute.h"
#include "Macros.h"

#include <cctype>
#include <sstream>
#include <vector>

namespace Util {

struct JSON_Token {
	enum token_t {
		T_UNDEFINED, T_NUMBER, T_STRING, T_TRUE, T_FALSE, T_NULL,
		T_MAP_OPEN, T_MAP_COLON, T_MAP_CLOSE,
		T_ARRAY_OPEN, T_ARRAY_CLOSE, T_DELIMITER
	} type;
	union {
		float floatValue;
		std::string * stringPtr;
	};
	JSON_Token(token_t _type) : type(_type), stringPtr(nullptr) {
	}
	JSON_Token(float number) : type(T_NUMBER), floatValue(number) {
	}
	JSON_Token(const std::string & s, token_t _type = T_STRING) : type(_type), stringPtr(new std::string(s)) {
	}
	JSON_Token(const JSON_Token &) = delete;
	JSON_Token(JSON_Token && other) : type(other.type) {
		if(other.type == T_NUMBER) {
			floatValue = other.floatValue;
		} else {
			stringPtr = other.stringPtr;
			other.stringPtr = nullptr;
		}
	}
	~JSON_Token() {
		if(type == T_STRING || type == T_UNDEFINED) {
			delete stringPtr;
		}
	}
	JSON_Token & operator=(const JSON_Token &) = delete;
	JSON_Token & operator=(JSON_Token && other) {
		// Handle self-assignment.
		if(this == &other) {
			return *this;
		}
		if(type == T_STRING || type == T_UNDEFINED) {
			delete stringPtr;
		}
		stringPtr = nullptr;
		type = other.type;
		if(other.type == T_NUMBER) {
			floatValue = other.floatValue;
		} else {
			stringPtr = other.stringPtr;
			other.stringPtr = nullptr;
		}
		return *this;
	}
	std::string getString() const {
		return (type == T_STRING && stringPtr != nullptr) ? std::string(*stringPtr) : "";
	}
};
typedef std::vector<JSON_Token> tokenArray_t;

/**
 * (internal) Convert a string into an array of tokens.
 * \note escaped unicode characters are not supported.
 */
static tokenArray_t tokenize(const std::string & input) {
	tokenArray_t tokens;
	std::istringstream iss (input);
	while (  iss.good() ) {
		char c = static_cast<char>(iss.peek());

		// skip spaces
		if (isspace(c)) {
			iss.get();
			continue;
		}
		// read constants (true,false,null)
		else if (isalpha(c)) {
			std::string s;
			while (iss.good() && isalnum(c)) {
				s+=c;
				iss.get();
				c = static_cast<char>(iss.peek());
			}
			if(s == "true") {
				tokens.push_back(JSON_Token(JSON_Token::T_TRUE));
			} else if(s == "false") {
				tokens.push_back(JSON_Token(JSON_Token::T_FALSE));
			} else if(s == "null") {
				tokens.push_back(JSON_Token(JSON_Token::T_NULL));
			} else {
				tokens.push_back(JSON_Token(s, JSON_Token::T_UNDEFINED));
			}
			continue;
		// read number
		} else if (isdigit(c) || c=='-') {
			float f;
			iss >> f;
			tokens.push_back(JSON_Token(f));
			continue;
		// read string
		} else if ( c=='"') {
			std::string s;
			iss.get();
			c=iss.peek();
			while (iss.good() && c!='"') {
				if (c=='\\') { // escape sequence
					iss.get();
					c = static_cast<char>(iss.peek());
					switch (c) {
						case 'b':	s+='\b';	break;
						case 'f':	s+='\f';	break;
						case 'n':	s+='\n';	break;
						case 'r':	s+='\r';	break;
						case 't':	s+='\t';	break;
						default:	s+=c;		break;
					}
					iss.get();
					c = static_cast<char>(iss.peek());
					continue;
				}
				s+=c;
				iss.get();
				c = static_cast<char>(iss.peek());
			}
			if (c!='"') {
				tokens.push_back(JSON_Token(s,JSON_Token::T_UNDEFINED));
				WARN("JSON_Parser: unclosed string '"+s+'\'');
				continue;
			}
			iss.get();
			tokens.push_back(JSON_Token(s));
			continue;
		}
		// read single char token
		iss.get();
		if(iss.fail()) {
			break;
		}

		switch (c) {
			case '[':	tokens.push_back(JSON_Token(JSON_Token::T_ARRAY_OPEN));     continue;
			case ']':	tokens.push_back(JSON_Token(JSON_Token::T_ARRAY_CLOSE));    continue;
			case '{':	tokens.push_back(JSON_Token(JSON_Token::T_MAP_OPEN));       continue;
			case '}':	tokens.push_back(JSON_Token(JSON_Token::T_MAP_CLOSE));      continue;
			case ':':	tokens.push_back(JSON_Token(JSON_Token::T_MAP_COLON));      continue;
			case ',':	tokens.push_back(JSON_Token(JSON_Token::T_DELIMITER));      continue;
			default:
				tokens.push_back(JSON_Token(std::string(1, c), JSON_Token::T_UNDEFINED));
				WARN("JSON_Parser: Unknown character.");
		}
	}
	return tokens;
}

//! (internal) Convert an array of tokens into a GenericAttribute.
static GenericAttribute * getGenericAttribute(const tokenArray_t & tokens, tokenArray_t::const_iterator & it) {
	if(it == tokens.end()) {
		return nullptr;
	}
	const JSON_Token & t = *it;
	++it;
	switch (t.type) {
		case JSON_Token::T_NUMBER:  return GenericAttribute::createNumber<float>(t.floatValue);
		case JSON_Token::T_STRING:  return GenericAttribute::createString(t.getString());
		case JSON_Token::T_TRUE:    return GenericAttribute::createBool(true);
		case JSON_Token::T_FALSE:   return GenericAttribute::createBool(false);
		case JSON_Token::T_NULL:    return GenericAttribute::createUndefined<void *>(nullptr);
		case JSON_Token::T_ARRAY_OPEN:  {
				auto l = new GenericAttributeList;
				while (true) {
					if (it==tokens.end()) {
						WARN("JSON_Parser: Unclosed array.");
						break;
					} else if (it->type==JSON_Token::T_ARRAY_CLOSE) {
						++it;
						break;
					}
					GenericAttribute * a=getGenericAttribute(tokens,it);
					if (a==nullptr) {
						WARN("JSON_Parser: Error in array (1).");
						break;
					}
					l->push_back(a);
					if (it==tokens.end() || it->type==JSON_Token::T_ARRAY_CLOSE) {
						continue;
					} else if (it->type==JSON_Token::T_DELIMITER) { // this allows [1,2,]
						++it;
						continue;
					} else {
						WARN("JSON_Parser: Error in array (2). Delimiter expected!");
						break;
					}
				}
				return l;
			}
		case JSON_Token::T_MAP_OPEN:  {
				auto m = new GenericAttributeMap;
				while (true) {
					if (it==tokens.end()) {
						WARN("JSON_Parser: Unclosed map.");
						break;
					} else if (it->type==JSON_Token::T_MAP_CLOSE) {
						++it;
						break;
					} else if ( it->type != JSON_Token::T_STRING) {
						WARN("JSON_Parser: Key in map is not a string.");
						break;
					}
					std::string key=it->getString();
					++it;
					if ( it==tokens.end() || it->type != JSON_Token::T_MAP_COLON) {
						WARN("JSON_Parser: Expected ':' between key and value-");
						break;
					}
					++it;
					GenericAttribute * value=getGenericAttribute(tokens,it);
					if (value==nullptr) {
						WARN("JSON_Parser: Error in map (1).");
						break;
					}

					m->setValue(key,value);
					if (it==tokens.end() || it->type==JSON_Token::T_MAP_CLOSE) {
						continue;
					} else if (it->type==JSON_Token::T_DELIMITER) { // this allows [1,2,]
						++it;
						continue;
					} else {
						WARN("JSON_Parser: Error in map (2). Delimiter expected!");
						break;
					}
				}
				return m;
			}
		case JSON_Token::T_UNDEFINED:
		case JSON_Token::T_MAP_COLON:
		case JSON_Token::T_MAP_CLOSE:
		case JSON_Token::T_ARRAY_CLOSE:
		case JSON_Token::T_DELIMITER:
			WARN("JSON_Parser: Read unexpected token '"+t.getString()+'\'');
			return nullptr;
		default: {
				WARN("JSON_Parser: Read undefined token '"+t.getString()+'\'');
				return nullptr;
			}

	}
}

GenericAttribute * JSON_Parser::parse(const std::string & str) {
	const tokenArray_t tokens = tokenize(str);
	auto it = tokens.begin();
	return getGenericAttribute(tokens, it);
}

}
