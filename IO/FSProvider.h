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
#define _FS_PROVIDER_H

#include <iosfwd>
#include <list>
#include <string>

#include "AbstractFSProvider.h"

namespace Util {

/*! Standard file system provider for accessing normal files with "file" protocol.
	E.g. "file://bla/foo.txt"

	[FSProvider] ---|> [AbstractFSProvider]
	@ingroup io
*/
class FSProvider : public AbstractFSProvider {
	public:
		UTILAPI static bool init();

		UTILAPI FSProvider();
		UTILAPI virtual ~FSProvider();

		// ---|> AbstractFSProvider
		UTILAPI status_t readFile(const FileName & file, std::vector<uint8_t> & data) override;
		UTILAPI status_t writeFile(const FileName &, const std::vector<uint8_t> & data, bool overwrite) override;

		UTILAPI std::unique_ptr<std::iostream> open(const FileName & filename) override;
		UTILAPI std::unique_ptr<std::istream> openForReading(const FileName & filename) override;
		UTILAPI std::unique_ptr<std::ostream> openForWriting(const FileName & filename) override;
		UTILAPI std::unique_ptr<std::ostream> openForAppending(const FileName & filename) override;

		UTILAPI status_t makeDir(const FileName &) override;
		UTILAPI status_t makeDirRecursive(const FileName &) override;
		UTILAPI status_t remove(const FileName &) override;

		UTILAPI status_t dir(const FileName &path, std::list<FileName> &result, uint8_t flags) override;
		UTILAPI bool isFile(const FileName &) override;
		UTILAPI bool isDir(const FileName &) override;
		UTILAPI uint64_t fileSize(const FileName & filename) override;
};
}
#endif	/* _FS_PROVIDER_H */
