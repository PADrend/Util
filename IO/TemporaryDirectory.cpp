/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "TemporaryDirectory.h"
#include "FileName.h"
#include "FileUtils.h"
#include "../Macros.h"
#include "../StringUtils.h"
#include "../Utils.h"
#include <stdexcept>
#include <cstdint>
#include <string>

namespace Util {

static FileName getTemporaryDirectoryName(uint32_t counter, const std::string & fragment) {
	FileName globalTmpDir;
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	std::string tmpDirString = std::getenv("TEMP");
	if(tmpDirString.empty()) {
		tmpDirString = std::getenv("TMP");
	}
	if(tmpDirString.empty()) {
		WARN("Using \"./tmp\" as temporary directory.");
		tmpDirString = "./tmp";
	}
	globalTmpDir = tmpDirString;
#elif defined(__linux) || defined(__linux__) || defined(linux) || defined (__APPLE__)
	globalTmpDir = "/tmp";
#else
	WARN("Using \"./tmp\" as temporary directory.");
	globalTmpDir = "./tmp";
#endif
	if(!FileUtils::isDir(globalTmpDir)) {
		throw std::runtime_error("Temporary directory could not be determined.");
	}

	const std::string pid = StringUtils::toString(Utils::getProcessId());
	const std::string num = StringUtils::toString(counter);

	if(fragment.empty()) {
		return FileName(globalTmpDir.toString() + '/' + pid + '_' + num + '/');
	} else {
		return FileName(globalTmpDir.toString() + '/' + pid + '_' + num + '_' + fragment + '/');
	}
}

uint32_t TemporaryDirectory::counter = 0;

TemporaryDirectory::TemporaryDirectory(const std::string & fragment) : path(getTemporaryDirectoryName(counter++, fragment)) {
	try {
		FileUtils::createDir(path, true);
	} catch(...) {
		WARN("Exception during directory creation.");
	}
}

TemporaryDirectory::~TemporaryDirectory() {
	try {
		FileUtils::remove(path, true);
	} catch(...) {
		WARN("Exception during directory deletion.");
	}
}

}
