/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef PROVIDES_TYPE_NAME_MACRO_H_INCLUDED
#define PROVIDES_TYPE_NAME_MACRO_H_INCLUDED

#include "StringIdentifier.h"

namespace Util {
#define PROVIDES_TYPE_NAME(_TYPE_NAME) \
public: \
	static const char * getClassName() { \
		return #_TYPE_NAME; \
	}\
	static const Util::StringIdentifier & getClassId(){ \
		static const Util::StringIdentifier _id = Util::StringIdentifier(getClassName());\
		return _id;\
	}\
	virtual const char * getTypeName() const { \
		return #_TYPE_NAME; \
	} \
	virtual const Util::StringIdentifier & getTypeId() const { \
		static const Util::StringIdentifier _id(getTypeName());\
		return _id;\
	} \
private:
// _TYPE_NAME::getClassName();

/// Non Virtual version of PROVIDES_TYPE_NAME
#define PROVIDES_TYPE_NAME_NV(_TYPE_NAME) \
public: \
	static const char * getClassName() { \
		return #_TYPE_NAME; \
	}\
	static const Util::StringIdentifier & getClassId(){ \
		static const Util::StringIdentifier _id(getClassName());\
		return _id;\
	}\
	const char * getTypeName() const { \
		return #_TYPE_NAME; \
	} \
	const Util::StringIdentifier & getTypeId() const { \
		static const Util::StringIdentifier _id(getTypeName());\
		return _id;\
	} \
private:

}
#endif // PROVIDES_TYPE_NAME_MACRO_H_INCLUDED
