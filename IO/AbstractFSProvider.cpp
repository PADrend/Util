/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "AbstractFSProvider.h"
#include "FileUtils.h"
#include "FileName.h"
#include "../Macros.h"

namespace Util {

/*! (static) */
std::string AbstractFSProvider::getStatusMessage(const status_t status){
	switch(status){
		case OK:
			return "OK";
		case UNSUPPORTED:
			return "Operation not supported by file provider.";
		case FAILURE:
		default:
			return "FAILURE";
	}
}

/*! (ctor) */
AbstractFSProvider::AbstractFSProvider() : ReferenceCounter_t() {
}

/*! (dtor) */
AbstractFSProvider::~AbstractFSProvider(){
}

AbstractFSProvider::status_t AbstractFSProvider::removeRecursive(const FileName & name){
	if(isFile(name))
		return remove(name);
	if(isDir(name))
	{
		bool fail = false;
		status_t st;
		std::list<FileName> names;
		st = dir(name, names, FileUtils::DIR_DIRECTORIES|FileUtils::DIR_FILES|FileUtils::DIR_HIDDEN_FILES);
		if(st == UNSUPPORTED)
			return UNSUPPORTED;
		for(const auto & n : names) {
			st = removeRecursive(n);
			if(st == UNSUPPORTED)
				return UNSUPPORTED;
			if(st == FAILURE)
				fail = true;
		}
		st = remove(name);
		if(fail)
			return FAILURE;
		return st;
	}
	else
		return FAILURE;
}

}
