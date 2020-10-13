/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_ZIP

#ifndef ZIP_PROVIDER_H_
#define	ZIP_PROVIDER_H_

#include "AbstractFSProvider.h"
#include "FileName.h"
#include <mutex>

// Forward declaration.
struct zip;

namespace Util {

/**
 * File system provider for access to ZIP files using the
 * "zip://path/to/archive$file_in_archive" URL scheme.
 *
 * @author Benjamin Eikel
 * @date 2010-03-11
 * @ingroup io
 */
class ZIPProvider : public AbstractFSProvider {
	public:
		UTILAPI static bool init();

		UTILAPI ZIPProvider();
		UTILAPI virtual ~ZIPProvider();

		UTILAPI status_t readFile(const FileName & url, std::vector<uint8_t> & data) override;
		UTILAPI status_t writeFile(const FileName & url, const std::vector<uint8_t> & data, bool overwrite) override;

		UTILAPI status_t dir(const FileName & url, std::list<FileName> & result, uint8_t flags) override;
		UTILAPI bool isFile(const FileName & url) override;
		UTILAPI bool isDir(const FileName & url) override;
		UTILAPI size_t fileSize(const FileName & url) override;

		UTILAPI status_t makeDir(const FileName & url) override;
		UTILAPI status_t makeDirRecursive(const FileName & url) override;
		UTILAPI status_t remove(const FileName & url) override;

		UTILAPI void flush() override;

	private:
		ZIPProvider(const ZIPProvider &) = delete;
		ZIPProvider(ZIPProvider &&) = delete;
		ZIPProvider & operator=(const ZIPProvider &) = delete;
		ZIPProvider & operator=(ZIPProvider &&) = delete;

		/*! Internal representation of an opened ZIP archive. */
		class ZIPHandle {
			public:
				UTILAPI explicit ZIPHandle(FileName _archiveRoot, zip * archive);
				UTILAPI ~ZIPHandle();

			public:
				UTILAPI status_t readFile(const FileName & file, std::vector<uint8_t> & data);
				UTILAPI status_t writeFile(const FileName & file, const std::vector<uint8_t> & data, bool overwrite);

				UTILAPI status_t dir(const std::string & directory, std::list<FileName> & result, const uint8_t flags);
				UTILAPI bool isFile(const FileName & file);
				UTILAPI bool isDir(const FileName & directory);
				UTILAPI size_t fileSize(const FileName & file);

				UTILAPI status_t makeDir(const FileName & directory);
				UTILAPI status_t removeDir(const FileName & directory);

				bool isChanged() const {
					return dataWritten;
				}
			private:
				ZIPHandle(const ZIPHandle &) = delete;
				ZIPHandle(ZIPHandle &&) = delete;
				ZIPHandle & operator=(const ZIPHandle &) = delete;
				ZIPHandle & operator=(ZIPHandle &&) = delete;

				const FileName archiveRoot;
				zip * handle;

				std::list<std::vector<uint8_t>> tempStore;

				bool dataWritten;
		};

		std::map<std::string, ZIPHandle *> openHandles;
		std::mutex handlesMutex;

		UTILAPI ZIPHandle * getZIPHandle(const std::string & archiveFileName, bool createFile = false);
		UTILAPI void decomposeURL(const FileName & url, std::string & archiveFileName, FileName & localPath);
};
}
#endif	/* ZIP_PROVIDER_H_ */

#endif /* UTIL_HAVE_LIB_ZIP */
