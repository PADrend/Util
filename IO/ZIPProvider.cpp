/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_ZIP

#include "ZIPProvider.h"
#include "FileName.h"
#include "FileUtils.h"
#include "../Concurrency/Concurrency.h"
#include "../Concurrency/Lock.h"
#include "../Concurrency/Mutex.h"
#include "../Factory/Factory.h"
#include "../Macros.h"

#include <sys/stat.h>
#include <dirent.h>
#include <cerrno>
#include <cstdint>
#include <memory>
#include <vector>

/*
 Copyright notice for libzip (http://nih.at/libzip/index.html):

 Copyright (C) 1999-2008 Dieter Baron and Thomas Klausner
 The authors can be contacted at <libzip@nih.at>

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 3. The names of the authors may not be used to endorse or promote
 products derived from this software without specific prior
 written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wshadow)
#include <zip.h>
COMPILER_WARN_POP

namespace Util {

bool ZIPProvider::init() {
	static ZIPProvider provider;
	return FileUtils::registerFSProvider("zip", PointerHolderCreator<ZIPProvider>(&provider));
}

ZIPProvider::ZIPProvider() :
	AbstractFSProvider(), openHandles(), handlesMutex(Concurrency::createMutex()) {
}

ZIPProvider::~ZIPProvider() {
	flush();
	delete handlesMutex;
}

AbstractFSProvider::status_t ZIPProvider::readFile(const FileName & url, std::vector<uint8_t> & data) {
	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);

	ZIPHandle * handle = getZIPHandle(archiveFileName);
	if (handle == nullptr) {
		return FAILURE;
	}

	// Make sure all data has been written.
	if (handle->isChanged()) {
		delete handle;
		openHandles.erase(archiveFileName);
		handle = getZIPHandle(archiveFileName);
	}

	return handle->readFile(file, data);
}

AbstractFSProvider::status_t ZIPProvider::writeFile(const FileName & url,
													const std::vector<uint8_t> & data,
													bool overwrite) {
	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	ZIPHandle * handle = getZIPHandle(archiveFileName, true);
	if (handle == nullptr) {
		return FAILURE;
	}
	return handle->writeFile(file, data, overwrite);
}

AbstractFSProvider::status_t ZIPProvider::dir(const FileName & url, std::list<
		FileName> & result, uint8_t flags) {
	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName localPath;
	decomposeURL(url, archiveFileName, localPath);
	ZIPHandle * handle = getZIPHandle(archiveFileName);
	if (handle == nullptr) {
		return FAILURE;
	}

	// Make sure all data has been written.
	if (handle->isChanged()) {
		delete handle;
		openHandles.erase(archiveFileName);
		handle = getZIPHandle(archiveFileName);
	}

	if (handle == nullptr) {
		return OK;
	}

	return handle->dir(localPath.getDir(), result, flags);
}

bool ZIPProvider::isFile(const FileName & url) {
	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	ZIPHandle * handle = getZIPHandle(archiveFileName);
	if (handle == nullptr) {
		return false;
	}

	// Make sure all data has been written.
	if (handle->isChanged()) {
		delete handle;
		openHandles.erase(archiveFileName);
		handle = getZIPHandle(archiveFileName);
	}

	if (handle == nullptr) {
		return false;
	}

	return handle->isFile(file);
}

bool ZIPProvider::isDir(const FileName & url) {
	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	ZIPHandle * handle = getZIPHandle(archiveFileName);
	if (handle == nullptr) {
		return false;
	}

	// Make sure all data has been written.
	if (handle->isChanged()) {
		delete handle;
		openHandles.erase(archiveFileName);
		handle = getZIPHandle(archiveFileName);
	}

	if (handle == nullptr) {
		return false;
	}

	return handle->isDir(file);
}

size_t ZIPProvider::fileSize(const FileName & url) {
	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	ZIPHandle * handle = getZIPHandle(archiveFileName);
	if (handle == nullptr) {
		return 0;
	}

	// Make sure all data has been written.
	if (handle->isChanged()) {
		delete handle;
		openHandles.erase(archiveFileName);
		handle = getZIPHandle(archiveFileName);
	}

	return handle->fileSize(file);
}

AbstractFSProvider::status_t ZIPProvider::makeDir(const FileName & url) {
	if(isDir(url)) {
		return AbstractFSProvider::OK;
	}

	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	ZIPHandle * handle = getZIPHandle(archiveFileName, true);
	if (handle == nullptr) {
		return FAILURE;
	}
	return handle->makeDir(file);
}

AbstractFSProvider::status_t ZIPProvider::makeDirRecursive(const FileName & url) {
	if(isDir(url)) {
		return AbstractFSProvider::OK;
	}

	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);

	const std::string path = file.getPath();
	// Split path into directory components
	size_t pos = 0;
	while(pos != path.size()) {
		pos = path.find('/', pos);
		if(pos == std::string::npos) {
			break;
		}

		const std::string subPath = path.substr(0, pos + 1);
		++pos;

		if(makeDir(FileName(archiveFileName + '$' + subPath)) != AbstractFSProvider::OK) {
			return AbstractFSProvider::FAILURE;
		}
	}
	return AbstractFSProvider::OK;
}

AbstractFSProvider::status_t ZIPProvider::remove(const FileName & url) {
	auto lock = Concurrency::createLock(*handlesMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);

	ZIPHandle * handle = getZIPHandle(archiveFileName);
	if (handle == nullptr) {
		return FAILURE;
	}

	// Make sure all data has been written.
	if (handle->isChanged()) {
		delete handle;
		openHandles.erase(archiveFileName);
		handle = getZIPHandle(archiveFileName);
	}

	return handle->removeDir(file);
}

void ZIPProvider::flush() {
	auto lock = Concurrency::createLock(*handlesMutex);
	for(auto & openHandle : openHandles) {
		delete openHandle.second;
	}
	openHandles.clear();
}

ZIPProvider::ZIPHandle * ZIPProvider::getZIPHandle(
													const std::string & archiveFileName,
													bool createFile) {
	ZIPHandle * handle = nullptr;

	// Check if archive is already opened.
	auto it = openHandles.find(archiveFileName);
	if (it != openHandles.end()) {
		return it->second;
	}

	int flags = ZIP_CHECKCONS;

	// Check if archive exists.
	if (!FileUtils::isFile(FileName(archiveFileName))) {
		if (!createFile) {
			return nullptr;
		} else {
			flags |= ZIP_CREATE;
		}
	}

	// Open archive.
	int error;
	zip * zipHandle = zip_open(archiveFileName.c_str(), flags, &error);

	if (zipHandle == nullptr) {
		char errorString[256];
		zip_error_to_str(errorString, 256, error, errno);
		WARN(errorString+std::string(" File: ")+archiveFileName);
		return nullptr;
	}
	FileName archiveRoot;
	archiveRoot.setFSName("zip");
	archiveRoot.setDir( archiveFileName+'$' );
	handle = new ZIPHandle(archiveRoot,zipHandle);
	openHandles[archiveFileName] = handle;
	return handle;
}

void ZIPProvider::decomposeURL(const FileName & url,
								std::string & archiveFileName, FileName & localPath) {
	const std::string path = url.getPath();
	const std::size_t splitPos = path.find('$');
	if (splitPos == std::string::npos) {
		archiveFileName = path;
		localPath = std::string("");
	} else {
		archiveFileName = path.substr(0, splitPos);
		// Strip off './' at the beginning.
		if(path.compare(splitPos + 1, 2, "./") == 0) {
			localPath = path.substr(splitPos + 3);
		} else if(path.compare(splitPos + 1, 1, "/") == 0) { // strip beginning '/'
			localPath = path.substr(splitPos + 2);
		} else {
			localPath = path.substr(splitPos + 1);
		}
	}
}

ZIPProvider::ZIPHandle::ZIPHandle(FileName _archiveRoot, zip * archive) :
	archiveRoot(std::move(_archiveRoot)), handle(archive), tempStore(), dataWritten(false) {
}

ZIPProvider::ZIPHandle::~ZIPHandle() {
	if (zip_close(handle) == -1) {
		WARN(zip_strerror(handle));
	}
}

AbstractFSProvider::status_t ZIPProvider::ZIPHandle::readFile(const FileName & file, std::vector<uint8_t> & data) {
	if (file.getFile().empty()) {
		return FAILURE;
	}

	const size_t size = fileSize(file);
	if (size == 0) {
		return FAILURE;
	}

	zip_file * fileHandle = zip_fopen(handle, file.getPath().c_str(), 0);
	if (fileHandle == nullptr) {
		WARN(zip_strerror(handle));
		return FAILURE;
	}

	data.resize(size);
	const int bytesRead = zip_fread(fileHandle, data.data(), data.size());
	if (bytesRead == -1) {
		WARN(zip_strerror(handle));
		zip_fclose(fileHandle);
		return FAILURE;
	}
	if (static_cast<size_t>(bytesRead) != size) {
		WARN("Sizes differ during read.");
		zip_fclose(fileHandle);
		return FAILURE;
	}

	if (zip_fclose(fileHandle) == -1) {
		WARN(zip_strerror(handle));
		return FAILURE;
	}

	return OK;
}

AbstractFSProvider::status_t ZIPProvider::ZIPHandle::writeFile(const FileName & file, const std::vector<uint8_t> & data, bool overwrite) {
	if (file.getFile().empty()) {
		return FAILURE;
	}

	bool replace = false;

	int index = zip_name_locate(handle, file.getPath().c_str(), 0);
	if (index != -1) {
		// File already exists.
		if (!overwrite) {
			return FAILURE;
		} else {
			replace = true;
		}
	}

	// Store data temporary because libzip writes data not until zip_close.
	tempStore.emplace_back(data);

	zip_source * source = zip_source_buffer(handle,
			tempStore.back().data(), static_cast<off_t>(tempStore.back().size()), 0);
	if (source == nullptr) {
		WARN(zip_strerror(handle));
		zip_source_free(source);
		tempStore.pop_back();
		return FAILURE;
	}

	int newIndex;
	if (replace) {
		newIndex = zip_replace(handle, index, source);
	} else {
		newIndex = zip_add(handle, file.getPath().c_str(), source);
	}
	if (newIndex == -1) {
		WARN(zip_strerror(handle));
		zip_source_free(source);
		tempStore.pop_back();
		return FAILURE;
	}
	dataWritten = true;
	return OK;
}

AbstractFSProvider::status_t ZIPProvider::ZIPHandle::dir(
															const std::string & localDirectory,
															std::list<FileName> & result,
															const uint8_t flags) {
	int num = zip_get_num_files(handle);
	if (num == -1) {
		WARN(zip_strerror(handle));
		return FAILURE;
	}
	struct zip_stat sb;
	zip_stat_init(&sb);

	// Iterate over indices.
	for (int i = 0; i < num; ++i) {
		if (zip_stat_index(handle, i, 0, &sb) == -1) {
			WARN(zip_strerror(handle));
			return FAILURE;
		}

		FileName entryFileName(sb.name);

		// Determine if the entry is a file or a directory.
		if (entryFileName.getFile().empty()) {
			if(!(flags & FileUtils::DIR_DIRECTORIES)) {
				continue;
			}

			if (!(flags & FileUtils::DIR_RECURSIVE)) {
				std::string entryDirectory = entryFileName.getDir();

				if(entryDirectory == localDirectory) {
					continue;
				}

				if(entryDirectory.back() == '/') {
					entryDirectory.resize(entryDirectory.size() - 1);
				}
				const auto slashPos = entryDirectory.find_last_of('/');
				if(slashPos != std::string::npos) {
					entryDirectory = entryDirectory.substr(0, slashPos + 1);
				} else {
					entryDirectory.clear();
				}
				// Compare the parent directory of the directory with the requested localDirectory.
				if(entryDirectory != localDirectory) {
					continue;
				}
			}
		} else {
			if(!(flags & FileUtils::DIR_FILES)) {
				continue;
			}

			// Compare the directory of the file with the requested localDirectory.
			if (!(flags & FileUtils::DIR_RECURSIVE) && entryFileName.getDir() != localDirectory) {
				continue;
			}
		}

		// Check for hidden files beginning with '.' (files only).
		if (entryFileName.getFile().front() == '.' && !(flags & FileUtils::DIR_HIDDEN_FILES)) {
			continue;
		}

		FileName f;
		f.setFSName(archiveRoot.getFSName());
		f.setDir(archiveRoot.getDir() + entryFileName.getDir());
		f.setFile(entryFileName.getFile());
		result.push_back(f);
	}
	return OK;
}

bool ZIPProvider::ZIPHandle::isFile(const FileName & file) {
	struct zip_stat sb;
	zip_stat_init(&sb);
	if (zip_stat(handle, file.getPath().c_str(), 0, &sb) == -1) {
		return false;
	}
	std::string entry(sb.name);
	return (entry.back() != '/' && sb.size != 0);
}

bool ZIPProvider::ZIPHandle::isDir(const FileName & directory) {
	if (directory.getDir().empty()) {
		// Always return true for the root archive directory.
		return true;
	}
	struct zip_stat sb;
	zip_stat_init(&sb);
	if (zip_stat(handle, directory.getDir().c_str(), 0, &sb) == -1) {
		return false;
	}
	std::string entry(sb.name);
	return (entry.back() == '/' && sb.size == 0);
}

size_t ZIPProvider::ZIPHandle::fileSize(const FileName & file) {
	struct zip_stat sb;
	zip_stat_init(&sb);
	if (zip_stat(handle, file.getPath().c_str(), 0, &sb) == -1) {
		WARN(zip_strerror(handle));
		return 0;
	}
	return static_cast<size_t>(sb.size);
}

AbstractFSProvider::status_t ZIPProvider::ZIPHandle::makeDir(const FileName & directory) {
	int index = zip_add_dir(handle, directory.getPath().c_str());
	if (index == -1) {
		WARN(zip_strerror(handle));
		return FAILURE;
	}
	dataWritten = true;
	return OK;
}

AbstractFSProvider::status_t ZIPProvider::ZIPHandle::removeDir(const FileName & directory) {
	int index = zip_name_locate(handle, directory.getPath().c_str(), 0);
	if (index == -1) {
		WARN(zip_strerror(handle));
		return FAILURE;
	}
	if (zip_delete(handle, index) == -1) {
		WARN(zip_strerror(handle));
		return FAILURE;
	}
	dataWritten = true;
	return OK;
}

}

#endif /* UTIL_HAVE_LIB_ZIP */
