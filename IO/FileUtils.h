/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef _FILEUTILS_H
#define _FILEUTILS_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace Util {
class AbstractFSProvider;
class FileName;

/*! Utility class with functions for file reading and writing.
	\note initFSProvider() is called by Util::init() 
	@ingroup io */
struct FileUtils {

// -------------------------------------------------------------------

/*! @name File System Provider */
// @{
	/*! Registers an additional file system provider for the given fsName.
		Files of the type "fsName://foo.bar" are than handled by that provider */
	UTILAPI static bool registerFSProvider(const std::string & fsName, std::function<AbstractFSProvider * ()> providerCreator);
// @}

// -------------------------------------------------------------------

/**
 * @name Streamed reading and writing
 * @note All streams created by these functions have to be freed by the caller.
 * Depending on the fsProvider, not deleting a stream may result in data loss.
 */
// @{
	//! Return an input and output stream (or nullptr on failure) for reading from and writing to a file.
	UTILAPI static std::unique_ptr<std::iostream> open(const FileName & fileName);
	//! Return an input stream (or nullptr on failure) for reading from a file.
	UTILAPI static std::unique_ptr<std::istream> openForReading(const FileName & fileName);
	/**
	 * Return an output stream (or nullptr on failure) for writing to a file.
	 * The file is truncated: Any current content is discarded, assuming a length of zero on opening.
	 */
	UTILAPI static std::unique_ptr<std::ostream> openForWriting(const FileName & fileName);
	/**
	 * Return an output stream (or nullptr on failure) for appending to a file.
	 * The stream's position indicator is set to the end of the stream before.
	 */
	UTILAPI static std::unique_ptr<std::ostream> openForAppending(const FileName & fileName);
// @}

// -------------------------------------------------------------------

/*! @name Loading and saving complete files */
// @{
	UTILAPI static std::vector<uint8_t> loadFile(const FileName & filename);
	UTILAPI static std::string getFileContents(const FileName & filename);
	UTILAPI static std::string getParsedFileContents(const FileName & filename);

	UTILAPI static bool saveFile(const FileName & filename,const std::vector<uint8_t> & data,bool overwrite=true);
// @}

// -------------------------------------------------------------------

/*! @name Information */
// @{
	UTILAPI static bool isFile(const FileName & filename);
	UTILAPI static bool isDir (const FileName & filename);
	UTILAPI static uint64_t fileSize(const FileName & filename);

	/**
	 * Search a file in different paths. The paths are checked absolute and relative to the originating file path.
	 *
	 * @param fileName File name to search.
	 * @param pathHints List of paths, which are searched for the file.
	 * @param newName New file name in the case the file was found.
	 * @return @c true if the file was found and @c false otherwise.
	 */
	UTILAPI static bool findFile(const FileName & fileName, const std::vector<std::string> & pathHints, FileName & newName);

	static const uint8_t DIR_FILES          = 1<<0; //!< flag for dir(...)
	static const uint8_t DIR_DIRECTORIES    = 1<<1; //!< flag for dir(...)
	static const uint8_t DIR_RECURSIVE      = 1<<2; //!< flag for dir(...)
	static const uint8_t DIR_HIDDEN_FILES   = 1<<3; //!< flag for dir(...)
	/*! Get files and directories in the given directory ( @p path ). The
		resulting filenames are inserted into @p results. The type of entries
		is determined by @p flags. DIR_RECURSIVE searches recursive in all subdirectories.
		\note Only the provider and directory part of @p path is taken; you can create an
			  appropriate Directory-FileName with FileName::createDirName(...) */
	UTILAPI static bool dir(const FileName & path, std::list<FileName> & result, uint8_t flags);

	/*! Try to make the given @p path relative to the given @p fixedPath.
		If the directory of @p path (or the directory to the container in path) begins with
		the directory of @p fixedPath (or the directory to the container in fixedPath), the directory entry
		is truncated accordingly.

		@return true iff the path could be changed
		@note Example:
		@verbatim
				fixedPath:  		"file://./data/scene/Szene_1.minsg"
				path:   			"file://./data/scene/../model/tree1.ply"
			dir of the path begins with the dir of the fixedPath ---> make it relative
				resulting path:   	"file://../model/tree1.ply"
		@endverbatim
		@note Example:
		@verbatim
				fixedPath:  		"file://./a/b/c/Szene_1.minsg"
				path:   			"zip://./a/b/c/d/Foo.zip$./tree1.ply"
			dir of the path begins with the dir of the fixedPath ---> make it relative
				resulting path:   	"zip://d/Foo.zip$./tree1.ply"
		@endverbatim
	*/
	UTILAPI static bool makeRelativeIfPossible(const FileName & fixedPath,FileName & path);
// @}

// -------------------------------------------------------------------

/*! @name Misc */
// @{

	UTILAPI static bool createDir(const FileName & name, bool recursive = true);
	UTILAPI static bool remove(const FileName & name, bool recursive = false);

	UTILAPI static FileName generateNewRandFilename(const FileName & dir,const std::string & prefix,const std::string & postfix,
										int randomSize=8);

	/**
	 * Copy the file with path @a source to the file with path @a dest.
	 * If the destination file already exists, it will be overwritten.
	 *
	 * @param source Path to source file
	 * @param dest Path to destination file
	 * @return @c true if successful, @c false otherwise
	 */
	UTILAPI static bool copyFile(const FileName & source, const FileName & dest);

	/**
	 * Make sure that any pending data will be written to its destination.
	 *
	 * @param path Path that will be used to determine the correct data to be flushed.
	 * It is enough if it contains a file system name, e.g. "zip://", "file://".
	 */
	UTILAPI static void flush(const FileName & path);
// @}

};
}

#endif	/* _FILEUTILS_2_H */
