/*
	This file is part of the Util library.
	Copyright (C) 2012-2014 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_ARCHIVE

#include "ArchiveProvider.h"
#include "FileName.h"
#include "FileUtils.h"
#include "../Factory/Factory.h"
#include "../Macros.h"
#include "../StringUtils.h"

#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <archive.h>
#include <archive_entry.h>

#include "../LibRegistry.h"
static bool libNameInitailized = [](){	
	Util::LibRegistry::registerLibVersionString("libarchive",ARCHIVE_VERSION_STRING + std::string(" (www.libarchive.org)")); 
	return true;
}();

namespace Util {

static archive * openReadHandle(const std::string & archiveFileName) {
	// Open archive for reading only.
	archive * readHandle = archive_read_new();
	if (readHandle == nullptr) {
		throw std::runtime_error("Cannot create new archive handle for reading. " 
				+ StringUtils::toString(archive_errno(readHandle)) + " " + archive_error_string(readHandle));
	}
	if (archive_read_support_format_all(readHandle) != ARCHIVE_OK) {
		throw std::runtime_error("Cannot enable format support for archive reading. " 
				+ StringUtils::toString(archive_errno(readHandle)) + " " + archive_error_string(readHandle));
	}
	if (archive_read_support_filter_all(readHandle) != ARCHIVE_OK) {
		throw std::runtime_error("Cannot enable filter support for archive reading. " 
				+ StringUtils::toString(archive_errno(readHandle)) + " " + archive_error_string(readHandle));
	}
	if(archive_read_open_filename(readHandle, archiveFileName.c_str(), 10240) != ARCHIVE_OK) {
		throw std::runtime_error("Cannot open archive " + archiveFileName + " for reading. " 
				+ StringUtils::toString(archive_errno(readHandle)) + " " + archive_error_string(readHandle));
	}
	return readHandle;
}

static void closeReadHandle(archive * readHandle, const std::string & archiveFileName) {
	if (archive_read_close(readHandle) != ARCHIVE_OK) {
		throw std::runtime_error("Cannot close archive " + archiveFileName + " after reading. "
				+ StringUtils::toString(archive_errno(readHandle)) + " " + archive_error_string(readHandle));
	}
	archive_read_free(readHandle);
}

//! C++ wrapper for archive_entry
class ArchiveEntry {
	public:
		ArchiveEntry(archive_entry * originalEntry) : entry(archive_entry_clone(originalEntry)) {
		}
		ArchiveEntry(const ArchiveEntry &) = delete;
		ArchiveEntry(ArchiveEntry && other) : entry(other.entry) {
			other.entry = nullptr;
		}
		~ArchiveEntry() {
			if(entry != nullptr) {
				archive_entry_free(entry); 
			}
		}
		ArchiveEntry & operator=(const ArchiveEntry &) = delete;
		ArchiveEntry & operator=(ArchiveEntry && other) = delete;

		archive_entry * get() {
			return entry;
		}

		size_t getSize() const {
			return static_cast<size_t>(archive_entry_size(entry));
		}

		bool isFile() const {
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wold-style-cast)
			return archive_entry_filetype(entry) == AE_IFREG;
COMPILER_WARN_POP
		}

		bool isDir() const {
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wold-style-cast)
			return archive_entry_filetype(entry) == AE_IFDIR;
COMPILER_WARN_POP
		}

	private:
		archive_entry * entry;
};

//! C++ wrapper for archive
class Archive {
	public:
		Archive(std::string archivePath) : archiveFileName(std::move(archivePath)), entries() {
			// Open archive for reading only.
			if (!FileUtils::isFile(FileName(archiveFileName))) {
				throw std::runtime_error("Archive file " + archiveFileName + " does not exist");
			}
			archive * readHandle = openReadHandle(archiveFileName);

			archive_entry * entry;
			while (archive_read_next_header(readHandle, &entry) == ARCHIVE_OK) {
				entries.insert(std::make_pair(archive_entry_pathname(entry), ArchiveEntry(entry)));
			}
			
			closeReadHandle(readHandle, archiveFileName);
		}
		
		bool dir(const std::string & localDirectory,
										 std::list<FileName> & result,
										 const uint8_t flags) {
			for(const auto & pathEntryPair : entries) {
				const FileName entryFileName(pathEntryPair.first);

				// Determine if the entry is a file or a directory.
				if (pathEntryPair.second.isDir()) {
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

				FileName fileName;
				fileName.setFSName("archive");
				fileName.setDir(archiveFileName + '$' + entryFileName.getDir());
				fileName.setFile(entryFileName.getFile());
				result.push_back(fileName);
			}
			return true;
		}

		bool isFile(const std::string & file) {
			const auto it = entries.find(file);
			if(it == entries.end()) {
				return false;
			}
			return it->second.isFile();
		}

		bool isDir(const std::string & directory) {
			const auto it = entries.find(directory);
			if(it == entries.end()) {
				return false;
			}
			return it->second.isDir();
		}

		size_t fileSize(const std::string & file) {
			const auto it = entries.find(file);
			if(it == entries.end()) {
				return 0;
			}
			return it->second.getSize();
		}

	private:
		std::string archiveFileName;

		std::unordered_map<std::string, ArchiveEntry> entries;
};

class WritableArchive {
	public:
		WritableArchive(const std::string & archiveFileName) :
				readArchiveFileName(), readHandle(nullptr), 
				writeArchiveFileName(), writeHandle(nullptr) {
					 // Check if archive exists.
			if (!FileUtils::isFile(FileName(archiveFileName))) {
				openWriteHandle(archiveFileName);
			} else {
				readHandle = openReadHandle(archiveFileName);
				readArchiveFileName = archiveFileName;
			}
		}

		~WritableArchive() {
			assert(readArchiveFileName != writeArchiveFileName);
			assert(readHandle != nullptr || writeHandle != nullptr);
			const bool needCopy = (readHandle != nullptr) && (writeHandle != nullptr);
			const bool emptyArchive = (writeHandle != nullptr) && (archive_file_count(writeHandle) == 0);
			if (readHandle != nullptr) {
				closeReadHandle(readHandle, readArchiveFileName);
			}
			if (writeHandle != nullptr) {
				if (archive_write_close(writeHandle) != ARCHIVE_OK) {
					throw std::runtime_error("Cannot close archive " + writeArchiveFileName + " after writing. "
							+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
				}
				archive_write_free(writeHandle);
			}
			if (needCopy) {
				FileUtils::remove(FileName(readArchiveFileName));
				if(emptyArchive) {
					// If the resulting archive is empty, remove it.
					FileUtils::remove(FileName(writeArchiveFileName));
				} else {
					const auto result = std::rename(writeArchiveFileName.c_str(), readArchiveFileName.c_str());
					if (result != 0) {
						const auto error = errno;
						throw std::runtime_error("Cannot rename written archive " + writeArchiveFileName + " to "
								+ readArchiveFileName + ". " + strerror(error));
					}
				}
			}
		}

		AbstractFSProvider::status_t writeFile(const std::string & file, const std::vector<uint8_t> & data) {
			bool fileWritten = false;
			if (writeHandle == nullptr) {
				archive_entry * entry;
				while (archive_read_next_header(readHandle, &entry) == ARCHIVE_OK) {
					// Format can be read from readHandle only after calling archive_read_next_header for the first time.
					if (writeHandle == nullptr) {
						openWriteHandle(FileUtils::generateNewRandFilename(FileName(FileName(readArchiveFileName).getDir()), 
																		FileName(readArchiveFileName).getFile() + '.', 
																		"", 8).getPath(), readHandle);
					}
					if (file == archive_entry_pathname(entry)) {
						archive_entry_set_size(entry, static_cast<int64_t>(data.size()));
						archive_write_header(writeHandle, entry);
						archive_write_data(writeHandle, data.data(), data.size());
						assert(!fileWritten);
						fileWritten = true;
					} else {
						copyEntry(readHandle, writeHandle, entry);
					}
					archive_write_finish_entry(writeHandle);
				}
			}
			if (!fileWritten) {
				archive_entry * entry = archive_entry_new();
				archive_entry_set_pathname(entry, file.c_str());
				archive_entry_set_size(entry, static_cast<int64_t>(data.size()));
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wold-style-cast)
				archive_entry_set_filetype(entry, AE_IFREG);
COMPILER_WARN_POP
				archive_entry_set_perm(entry, 0644);
				archive_write_header(writeHandle, entry);
				archive_write_data(writeHandle, data.data(), data.size());
				archive_entry_free(entry);
			}
			return AbstractFSProvider::OK;
		}

		//! Because @codeisDir() == false@endcode is already checked below, assume that the directory does not exist.
		AbstractFSProvider::status_t makeDir(const FileName & directory) {
			if (writeHandle == nullptr) {
				// Copy archive
				archive_entry * entry;
				while (archive_read_next_header(readHandle, &entry) == ARCHIVE_OK) {
					// Format can be read from readHandle only after calling archive_read_next_header for the first time.
					if (writeHandle == nullptr) {
						openWriteHandle(FileUtils::generateNewRandFilename(FileName(FileName(readArchiveFileName).getDir()), 
																		FileName(readArchiveFileName).getFile() + '.', 
																		"", 8).getPath(), readHandle);
					}
					copyEntry(readHandle, writeHandle, entry);
					archive_write_finish_entry(writeHandle);
				}
			}

			archive_entry * entry = archive_entry_new();
			archive_entry_set_pathname(entry, directory.getPath().c_str());
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wold-style-cast)
			archive_entry_set_filetype(entry, AE_IFDIR);
COMPILER_WARN_POP
			archive_write_header(writeHandle, entry);
			archive_entry_free(entry);
			return AbstractFSProvider::OK;
		}

		//! Because @codeisDir() == true@endcode is already checked below, assume that the directory exists.
		AbstractFSProvider::status_t removeDir(const FileName & directory) {
			assert(writeHandle == nullptr);
			const std::string directoryPath(directory.getPath());
			archive_entry * entry;
			while (archive_read_next_header(readHandle, &entry) == ARCHIVE_OK) {
				// Format can be read from readHandle only after calling archive_read_next_header for the first time.
				if (writeHandle == nullptr) {
					openWriteHandle(FileUtils::generateNewRandFilename(FileName(FileName(readArchiveFileName).getDir()), 
																	   FileName(readArchiveFileName).getFile() + '.', 
																	   "", 8).getPath(), readHandle);
				}
				const std::string entryPath(archive_entry_pathname(entry));
				// Skip the directory to delete
				if (entryPath.compare(0, directoryPath.size(), directoryPath) != 0) {
					copyEntry(readHandle, writeHandle, entry);
					archive_write_finish_entry(writeHandle);
				}
			}
			return AbstractFSProvider::OK;
		}
		
		//! Because @codeisFile() == true@endcode is already checked below, assume that the file exists.
		AbstractFSProvider::status_t removeFile(const FileName & file) {
			// Copy archive and remove the file.
			assert(writeHandle == nullptr);
			archive_entry * entry;
			while (archive_read_next_header(readHandle, &entry) == ARCHIVE_OK) {
				// Format can be read from readHandle only after calling archive_read_next_header for the first time.
				if (writeHandle == nullptr) {
					openWriteHandle(FileUtils::generateNewRandFilename(FileName(FileName(readArchiveFileName).getDir()), 
																	   FileName(readArchiveFileName).getFile() + '.', 
																	   "", 8).getPath(), readHandle);
				}
				const FileName entryFileName(archive_entry_pathname(entry));
				// Skip the file to delete
				if (file != entryFileName) {
					copyEntry(readHandle, writeHandle, entry);
					archive_write_finish_entry(writeHandle);
				}
			}
			return AbstractFSProvider::OK;
		}
	private:
		std::string readArchiveFileName;
		archive * readHandle;
		std::string writeArchiveFileName;
		archive * writeHandle;

		WritableArchive(const WritableArchive &) = delete;
		WritableArchive(WritableArchive &&) = delete;
		WritableArchive & operator=(const WritableArchive &) = delete;
		WritableArchive & operator=(WritableArchive &&) = delete;

		void openWriteHandle(const std::string & archiveFileName) {
			// Open archive for writing only.
			writeHandle = archive_write_new();
			if (writeHandle == nullptr) {
				throw std::runtime_error("Cannot create new archive handle for writing");
			}

			int formatResult = ARCHIVE_OK;
			int filterResult = ARCHIVE_OK;
			const auto pointPos = archiveFileName.find_last_of('.');
			if(pointPos != std::string::npos) {
				const std::string extension = archiveFileName.substr(pointPos);
				if(extension == ".zip") {
					formatResult = archive_write_set_format_zip(writeHandle);
				} else {
					formatResult = archive_write_set_format_pax_restricted(writeHandle);
					if(extension == ".bz2") {
						filterResult = archive_write_add_filter_bzip2(writeHandle);
					} else if(extension == ".gz") {
						filterResult = archive_write_add_filter_gzip(writeHandle);
					} else if(extension == ".xz") {
						filterResult = archive_write_add_filter_xz(writeHandle);
					}
				}
			}
			if (formatResult != ARCHIVE_OK) {
				throw std::runtime_error("Cannot enable format support for archive writing. " 
						+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
			}
			if (filterResult != ARCHIVE_OK) {
				throw std::runtime_error("Cannot enable filter support for archive writing. " 
						+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
			}

			if(archive_write_open_filename(writeHandle, archiveFileName.c_str()) != ARCHIVE_OK) {
				throw std::runtime_error("Cannot open archive " + archiveFileName + " for writing. " 
						+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
			}

			writeArchiveFileName = archiveFileName;
		}

		//! Open a write handle that is used to copy data from another archive
		void openWriteHandle(const std::string & archiveFileName, archive * handle) {
			// Open archive for writing only.
			writeHandle = archive_write_new();
			if (writeHandle == nullptr) {
				throw std::runtime_error("Cannot create new archive handle for writing");
			}
			if (archive_write_set_format(writeHandle, archive_format(handle)) != ARCHIVE_OK) {
				throw std::runtime_error("Cannot enable format support for archive writing. " 
						+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
			}
			for (int i = 0; i < archive_filter_count(handle); ++i) {
				if (archive_write_add_filter(writeHandle, archive_filter_code(handle, i)) != ARCHIVE_OK) {
					throw std::runtime_error("Cannot enable filter support for archive writing. " 
							+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
				}
			}
			if(archive_write_open_filename(writeHandle, archiveFileName.c_str()) != ARCHIVE_OK) {
				throw std::runtime_error("Cannot open archive " + archiveFileName + " for writing. " 
						+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
			}

			writeArchiveFileName = archiveFileName;
		}

		static void copyEntry(archive * readHandle, archive * writeHandle, archive_entry * entry) {
			const size_t entrySize = static_cast<size_t>(archive_entry_size(entry));
			archive_write_header(writeHandle, entry);
			if (entrySize > 0) {
				std::vector<uint8_t> buffer(entrySize);
				const auto resultRead = archive_read_data(readHandle, buffer.data(), buffer.size());
				if (static_cast<size_t>(resultRead) != buffer.size()) {
					throw std::runtime_error("Error reading data. " 
							+ StringUtils::toString(archive_errno(readHandle)) + " " + archive_error_string(readHandle));
				}
				const auto resultWrite = archive_write_data(writeHandle, buffer.data(), buffer.size());
				if (static_cast<size_t>(resultWrite) != buffer.size()) {
					throw std::runtime_error("Error writing data. " 
							+ StringUtils::toString(archive_errno(writeHandle)) + " " + archive_error_string(writeHandle));
				}
			}
		}
};

bool ArchiveProvider::init() {
	static ArchiveProvider provider;
	return FileUtils::registerFSProvider("archive", PointerHolderCreator<ArchiveProvider>(&provider));
}

ArchiveProvider::ArchiveProvider() :
	AbstractFSProvider(), archiveMutex() {
}

ArchiveProvider::~ArchiveProvider() {
	flush();
}

AbstractFSProvider::status_t ArchiveProvider::readFile(const FileName & url, std::vector<uint8_t> & data) {
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);

	if(file.getFile().empty()) {
		return AbstractFSProvider::FAILURE;
	}

	bool success = false;

	std::lock_guard<std::mutex> lock(archiveMutex);
	archive * readHandle = openReadHandle(archiveFileName);

	archive_entry * entry;
	while (archive_read_next_header(readHandle, &entry) == ARCHIVE_OK) {
		const FileName entryFileName(archive_entry_pathname(entry));
		if(file.getPath() == entryFileName.getPath()) {
			const auto entrySize = archive_entry_size(entry);
			data.resize(static_cast<size_t>(entrySize));
			if(entrySize > 0) {
				const auto bytesRead = archive_read_data(readHandle, data.data(), data.size());
				if (bytesRead != entrySize) {
					WARN("Sizes differ during read.");
				}
			}
			success = true;
			break;
		}
	}
	closeReadHandle(readHandle, archiveFileName);
	if (!success) {
		WARN("Cannot find entry for file \"" + file.getPath() + "\"");
		return AbstractFSProvider::FAILURE;
	}
	return AbstractFSProvider::OK;
}

AbstractFSProvider::status_t ArchiveProvider::writeFile(const FileName & url,
														const std::vector<uint8_t> & data,
														bool overwrite) {
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);

	if (file.getFile().empty()) {
		return AbstractFSProvider::FAILURE;
	}

	if (!overwrite) {
		std::lock_guard<std::mutex> lock(archiveMutex);
		Archive * readHandle = getHandle(archiveFileName);
		if(readHandle->isFile(file.getPath())) {
			return AbstractFSProvider::FAILURE;
		}
	}

	std::lock_guard<std::mutex> lock(archiveMutex);
	openArchives.erase(archiveFileName);
	WritableArchive handle(archiveFileName);
	return handle.writeFile(file.getPath(), data);
}

AbstractFSProvider::status_t ArchiveProvider::dir(const FileName & url,
												  std::list<FileName> & result,
												  uint8_t flags) {
	std::string archiveFileName;
	FileName localPath;
	decomposeURL(url, archiveFileName, localPath);

	if(!FileUtils::isFile(FileName(archiveFileName))) {
		return AbstractFSProvider::OK;
	}

	std::lock_guard<std::mutex> lock(archiveMutex);
	Archive * handle = getHandle(archiveFileName);
	return handle->dir(localPath.getDir(), result, flags) ? AbstractFSProvider::OK : AbstractFSProvider::FAILURE;
}

bool ArchiveProvider::isFile(const FileName & url) {
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);

	if(!FileUtils::isFile(FileName(archiveFileName))) {
		return false;
	}

	std::lock_guard<std::mutex> lock(archiveMutex);
	Archive * handle = getHandle(archiveFileName);
	return handle->isFile(file.getPath());
}

bool ArchiveProvider::isDir(const FileName & url) {
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);

	if(!FileUtils::isFile(FileName(archiveFileName))) {
		return false;
	}

	std::lock_guard<std::mutex> lock(archiveMutex);
	Archive * handle = getHandle(archiveFileName);
	return handle->isDir(file.getPath());
}

size_t ArchiveProvider::fileSize(const FileName & url) {
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	
	std::lock_guard<std::mutex> lock(archiveMutex);
	Archive * handle = getHandle(archiveFileName);
	return handle->fileSize(file.getPath());
}

AbstractFSProvider::status_t ArchiveProvider::makeDir(const FileName & url) {
	if(isDir(url)) {
		return AbstractFSProvider::OK;
	}

	std::lock_guard<std::mutex> lock(archiveMutex);
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	openArchives.erase(archiveFileName);
	WritableArchive handle(archiveFileName);
	return handle.makeDir(file);
}

AbstractFSProvider::status_t ArchiveProvider::makeDirRecursive(const FileName & url) {
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

AbstractFSProvider::status_t ArchiveProvider::remove(const FileName & url) {
	std::string archiveFileName;
	FileName file;
	decomposeURL(url, archiveFileName, file);
	
	if(isDir(url)) {
		std::lock_guard<std::mutex> lock(archiveMutex);
		openArchives.erase(archiveFileName);
		WritableArchive handle(archiveFileName);
		return handle.removeDir(file);
	} else if(isFile(url)) {
		std::lock_guard<std::mutex> lock(archiveMutex);
		openArchives.erase(archiveFileName);
		WritableArchive handle(archiveFileName);
		return handle.removeFile(file);
	}
	return AbstractFSProvider::FAILURE;
}

void ArchiveProvider::flush() {
	std::lock_guard<std::mutex> lock(archiveMutex);
	openArchives.clear();
}

Archive * ArchiveProvider::getHandle(const std::string & archiveFileName) {
	const auto it = openArchives.find(archiveFileName);
	if(it == openArchives.end()) {
		auto handle = new Archive(archiveFileName);
		openArchives.insert(std::make_pair(archiveFileName, std::unique_ptr<Archive>(handle)));
		return handle;
	}
	return it->second.get();
}

void ArchiveProvider::decomposeURL(const FileName & url,
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

}

#endif /* UTIL_HAVE_LIB_ARCHIVE */
