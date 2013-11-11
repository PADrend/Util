/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "FSProvider.h"
#include "FileName.h"
#include "FileUtils.h"
#include "../Factory/Factory.h"
#include "../Macros.h"

#include <sys/stat.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <dirent.h>
#include <vector>
#if defined(__linux__) || defined(__unix__) || defined (__APPLE__)
#include <unistd.h>
#endif /* defined(__linux__) || defined(__unix__) || defined (__APPLE__) */

namespace Util {

bool FSProvider::init() {
	static FSProvider provider;
	return FileUtils::registerFSProvider("file", PointerHolderCreator<FSProvider>(&provider));
}

// -------------------------------

/*! (ctor) */
FSProvider::FSProvider() : AbstractFSProvider() {
}

/*! (dtor) */
FSProvider::~FSProvider(){
}

//! ---|> AbstractFSProvider
AbstractFSProvider::status_t FSProvider::dir(const FileName &path, std::list<FileName> &result, uint8_t flags) {
	std::string name = path.getDir();

	if ((flags & FileUtils::DIR_FILES) && isFile(FileName(name))) {
		result.emplace_back(name);
		return OK;
	}
	if(name.empty())
		name="./";

	DIR *myDir;
	myDir=opendir(name.c_str());
	if (!myDir) {
		WARN(std::string("FileUtils: could not open directory ") + name);
		return FAILURE;
	}
	FileName entryName;
#if !defined(_WIN32)
	dirent entryStorage;
#endif
	dirent * entry;
	do {
#if defined(_WIN32)
		entry = readdir(myDir);
#else
		const int success = readdir_r(myDir, &entryStorage, &entry);
		if(success != 0) {
			const int error = errno;
			WARN(std::string(strerror(error)));
			return FAILURE;
		}
#endif
		if(entry == nullptr) {
			break;
		}
		std::string subName = entry->d_name;
		if( subName[0] == '.' &&
				((flags & FileUtils::DIR_HIDDEN_FILES)==0 || subName == "." || subName == "..") )
			continue;

		entryName = path;
		entryName.setFile(subName);

		if ( (flags & FileUtils::DIR_FILES) && isFile(entryName) ) {
			result.push_back(entryName);
		}

		if (isDir(entryName)) {
			// Make sure a directory always ends with a slash.
			entryName = entryName.getDir() + subName + '/';
			if (flags & FileUtils::DIR_DIRECTORIES) {
				result.push_back(entryName);
			}
			if (flags & FileUtils::DIR_RECURSIVE) {
				dir(entryName, result, flags);
			}
		}
	} while(entry != nullptr);
	closedir(myDir);
	return OK;
}

//! ---|> AbstractFSProvider
std::unique_ptr<std::iostream> FSProvider::open(const FileName & filename) {
	std::unique_ptr<std::iostream> stream(new std::fstream(filename.getPath().c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary));
	if(!stream->good()) {
		return nullptr;
	}
	return std::move(stream);
}

//! ---|> AbstractFSProvider
std::unique_ptr<std::istream> FSProvider::openForReading(const FileName & filename) {
	std::unique_ptr<std::istream> stream(new std::ifstream(filename.getPath().c_str(), std::ios_base::in | std::ios_base::binary));
	if(!stream->good()) {
		return nullptr;
	}
	return std::move(stream);
}

//! ---|> AbstractFSProvider
std::unique_ptr<std::ostream> FSProvider::openForWriting(const FileName & filename) {
	std::unique_ptr<std::ostream> stream(new std::ofstream(filename.getPath().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc));
	if(!stream->good()) {
		return nullptr;
	}
	return std::move(stream);
}

//! ---|> AbstractFSProvider
std::unique_ptr<std::ostream> FSProvider::openForAppending(const FileName & filename) {
	std::unique_ptr<std::ostream> stream(new std::ofstream(filename.getPath().c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app));
	if(!stream->good()) {
		return nullptr;
	}
	return std::move(stream);
}

//! ---|> AbstractFSProvider
AbstractFSProvider::status_t FSProvider::readFile(const FileName & filename, std::vector<uint8_t> & data){
	std::ifstream inputFile( filename.getPath().c_str(), std::ios::in | std::ios::binary);
	if ( inputFile.fail())
		return FAILURE;

	inputFile.seekg( 0, std::ios::end );
	size_t size = inputFile.tellg();
	inputFile.seekg( 0, std::ios::beg );

	data.resize(size);
	inputFile.read(reinterpret_cast<char *>(data.data()), size);
	inputFile.close();

	return OK;
}

//! ---|> AbstractFSProvider
AbstractFSProvider::status_t FSProvider::writeFile(const FileName & filename, const std::vector<uint8_t> & data, bool overwrite){
	if(!overwrite && isFile(filename))
		return FAILURE;

	std::ofstream outputFile( filename.getPath().c_str(), std::ios::out | std::ios::binary);
	if ( outputFile.fail())
		return FAILURE;

	outputFile.write(reinterpret_cast<const char *>(data.data()), data.size());
	outputFile.close();
	return OK;
}

//! ---|> AbstractFSProvider
bool FSProvider::isFile(const FileName & filename){
	struct stat statInfo;
	return (stat(filename.getPath().c_str(), &statInfo) == 0 && S_ISREG(statInfo.st_mode));
}

//! ---|> AbstractFSProvider
bool FSProvider::isDir(const FileName & filename){
	std::string s = filename.getPath();
	if(s.back() == '/') {
		// FIXME: Replace by s.pop_back() when supported.
		s.resize(s.length() - 1);
	}
	struct stat statInfo;
	return (stat(s.c_str(), &statInfo) == 0 && S_ISDIR(statInfo.st_mode));
}

//! ---|> AbstractFSProvider
AbstractFSProvider::status_t FSProvider::remove(const FileName & name){
	bool success = false;
	if(isFile(name)) {
		success = (std::remove(name.getPath().c_str()) == 0);
	} else if(isDir(name)) {
		success = (rmdir(name.getPath().c_str()) == 0);
	}
	if(!success) {
		const int error = errno;
		WARN(std::string(strerror(error)));
		return FAILURE;
	}
	return OK;
}

//! ---|> AbstractFSProvider
AbstractFSProvider::status_t FSProvider::makeDir(const FileName & name){
	if (isDir(name))
		return OK;
	else if(isFile(name))
		return FAILURE;
#if defined(__linux__) || defined(__unix__) || defined (__APPLE__)
	if(mkdir(name.getPath().c_str(), 0770) == 0)
		return OK;
	else
		return FAILURE;
#elif defined(_WIN32) || defined(_WIN64)
	if(mkdir(name.getPath().c_str())==0)
		return OK;
	else
		return FAILURE;
#else
	WARN("makeDir not implemented for your platform.");
	return FAILURE;
#endif
}

//! ---|> AbstractFSProvider
AbstractFSProvider::status_t FSProvider::makeDirRecursive(const FileName & name){
	if (isDir(name))
		return OK;
	else if(isFile(name))
		return FAILURE;

	std::string s = name.getPath();
	size_t pos = std::string::npos;
	if(s.back() == '/') {
		pos = s.rfind('/')-1;
	}
	makeDirRecursive(FileName(s.substr(0,s.rfind('/', pos))));
	return makeDir(name);
}

//! ---|> AbstractFSProvider
size_t FSProvider::fileSize(const FileName & filename){
	std::ifstream inputFile(filename.getPath().c_str(), std::ios::in | std::ios::binary);
	if ( inputFile.fail())
		return 0;

	inputFile.seekg( 0, std::ios::end);
	size_t size = inputFile.tellg();
	inputFile.close();
	return size;
}

}
