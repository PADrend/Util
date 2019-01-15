/*
	This file is part of the Util library.
	Copyright (C) 2014 Claudius Jähn <claudius@uni-paderborn.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef FILE_LOCATOR_H_
#define FILE_LOCATOR_H_

#include "FileName.h"
#include <string>
#include <vector>

namespace Util {

/** 
 * A FileLocator locates a file that is placed relative to a search path.
 * It can be applied to locate resources using only relative paths.
 * @ingroup io
 */
class FileLocator{
	private:	
		std::vector<std::string> searchPaths;
	public:
		FileLocator() = default;
		FileLocator(const FileLocator&) = default;
		FileLocator(FileLocator&&) = default;
		~FileLocator() = default;
		
		FileLocator& operator=(const FileLocator&) = default;
		FileLocator& operator=(FileLocator&&) = default;

		void addSearchPath(std::string s)						{	searchPaths.emplace_back(std::move(s));	}
		const std::vector<std::string>& getSearchPaths()const	{	return searchPaths;	}
		void setSearchPaths(std::vector<std::string> p)			{	searchPaths = std::move(p);	}

		/*! If the file can be found in one of the the searchPaths, <true, searchPath/relFile> is returned;
			Otherwise, <false,relFile> is returned.	*/
		std::pair<bool,FileName> locateFile(const FileName& relFile) const; // -> <found?, fullPathName>
};

}

#endif /* FILE_LOCATOR_H_ */
