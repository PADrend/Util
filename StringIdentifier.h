/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef STRINGIDENTIFIER_H_
#define STRINGIDENTIFIER_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Util {


/**
 * String identifiers map a string to a number.
 * This mapping is guaranteed to be unique during one execution of the program.
 *
 * @author Claudius Jähn, Benjamin Eikel
 */
class StringIdentifier {
		uint32_t value;
	public:

		StringIdentifier() : value(0) 							{}
		explicit StringIdentifier( uint32_t _id) : value(_id)	{}
		/*implicit*/ StringIdentifier( const std::string & str) : value(calcId(str)) {}

		uint32_t getValue()const								{	return value;	}
		std::string toString()const;

		StringIdentifier & operator=(const std::string & str){
			value = calcId(str);
			return *this;
		}
		bool empty()const										{	return value==0;	}

		bool operator==(const StringIdentifier & other)const	{	return value == other.value;	}
		bool operator!=(const StringIdentifier & other)const	{	return value != other.value;	}
		bool operator<(const StringIdentifier & other)const		{	return value < other.value;	}

	private:
		static uint32_t calcId(const std::string & s);
		static uint32_t calcHash(const std::string & s);
};

}

namespace std{
template <> struct hash<Util::StringIdentifier> {
	size_t operator()(const Util::StringIdentifier & id) const noexcept {	return id.getValue();	}
};
}
#endif /* STRINGIDENTIFIER_H_ */
