/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef _FS_PROVIDER_H
#define	_FS_PROVIDER_H

#include <string>
#include <list>
#include <fstream>

#include "AbstractFSProvider.h"

namespace Util {

/*! Standard file system provider for accessing normal files with "file" protocol.
	E.g. "file://bla/foo.txt"

	[FSProvider] ---|> [AbstractFSProvider]
*/
class FSProvider : public AbstractFSProvider {
	public:
		static bool init();

		FSProvider();
		virtual ~FSProvider();

		// ---|> AbstractFSProvider
		status_t readFile(const FileName & file, std::vector<uint8_t> & data) override;
		status_t writeFile(const FileName &, const std::vector<uint8_t> & data, bool overwrite) override;

		std::unique_ptr<std::iostream> open(const FileName & filename) override;
		std::unique_ptr<std::istream> openForReading(const FileName & filename) override;
		std::unique_ptr<std::ostream> openForWriting(const FileName & filename) override;
		std::unique_ptr<std::ostream> openForAppending(const FileName & filename) override;

		status_t makeDir(const FileName &) override;
		status_t makeDirRecursive(const FileName &) override;
		status_t remove(const FileName &) override;

		status_t dir(const FileName &path, std::list<FileName> &result, uint8_t flags) override;
		bool isFile(const FileName &) override;
		bool isDir(const FileName &) override;
		size_t fileSize(const FileName & filename) override;
};
}
#endif	/* _FS_PROVIDER_H */
