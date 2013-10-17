/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_SQLITE

#ifndef _DBFS_PROVIDER_H
#define	_DBFS_PROVIDER_H

#include <string>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <sqlite3.h>

#include "AbstractFSProvider.h"

namespace Util {

/*! Standard file system provider for accessing normal files with "file" protocol.
	E.g. "file://bla/foo.txt"

	[DBFSProvider] ---|> [AbstractFSProvider]
*/
class DBFSProvider : public AbstractFSProvider {
	public:
		static bool init();

		// ----
		// static Helper
		static sqlite3_stmt * createStatement(sqlite3 * db,const char * sql);

		static const int NO_ENTRY = -1;
//		static const int TYPE_FILE = 1;
//		static const int TYPE_FOLDER = 0;

		/*! Internal representation of a database connection */
		class DBHandle{
			std::list<sqlite3_stmt *> deferredStatements;
			std::set<std::string> deferredFiles;

			sqlite3 * db;
			sqlite3_stmt * getFolderId_stmt;
			sqlite3_stmt * getFileData_stmt;

			sqlite3_stmt * getFileSize_stmt;
			sqlite3_stmt * isFile_stmt;
			sqlite3_stmt * dirFiles_stmt;
			sqlite3_stmt * dirFolders_stmt;

		public:
			explicit DBHandle(sqlite3 *_db);
			~DBHandle();
			inline sqlite3 * getDB()const{	return db;	}

		public:
			bool saveFile(const std::string & folder,const std::string & file,const std::vector<uint8_t> & data,bool overwrite=true);
			std::vector<uint8_t> readFile(const std::string & folder,const std::string & file);
			size_t getSize(const std::string & folder,const std::string & file);
			bool dir(const std::string & folder, const std::string & prefix, std::list<FileName> &result, uint8_t flags);
			int getFolderId(const std::string & folder);
			bool isFile(const std::string & folder,const std::string & file);

			void flush();
		protected:
			bool createFile(int folderId,const std::string & file);
			bool updateData(int fileId,const std::vector<uint8_t> & data);
			std::vector<uint8_t> getData(int fileId);

			void storeStatement(int folderId,const std::string & file,sqlite3_stmt * stmt);
			bool isPendingFile(int intFolderId,const std::string & file);

		};

		DBFSProvider();
		virtual ~DBFSProvider();

		status_t readFile(const FileName & file, std::vector<uint8_t> & data) override;
		status_t writeFile(const FileName &, const std::vector<uint8_t> & data, bool overwrite) override;

		bool isFile(const FileName &) override;
		bool isDir(const FileName &) override;
		size_t fileSize(const FileName & filename) override;
		status_t dir(const FileName &path, std::list<FileName> &result, uint8_t flags) override;

		void flush() override;

		// -----------------------------------------
	private:
		std::map<std::string, DBHandle*> openHandles;

		DBHandle * createDB(const std::string & dbFilename);
		DBHandle * getDBHandle(const std::string & dbFilename, bool createFile = false);
		void extractFileName(const FileName & filename,std::string & dbFilename,std::string & folder,std::string & file);
};
}
#endif	/* _DBFS_PROVIDER_H */
#endif	/* UTIL_HAVE_LIB_SQLITE */
