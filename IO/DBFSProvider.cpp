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

#include "DBFSProvider.h"
#include "FileName.h"
#include "FileUtils.h"
#include "../Factory/Factory.h"
#include "../Macros.h"
#include "../Concurrency/Concurrency.h"
#include "../Concurrency/Mutex.h"
#include "../Concurrency/Lock.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sys/stat.h>
#include <sstream>
#include <vector>

#include "../LibRegistry.h"
static bool libNameInitailized = [](){	
	Util::LibRegistry::registerLibVersionString("sqlite","sqlite " SQLITE_VERSION " (www.sqlite.org)"); 
	return true;
}();

static const std::size_t MAX_DEFFERED_STATEMENTS = 500;
static const sqlite3_destructor_type SQLITE_TRANSIENT_CXX = reinterpret_cast<sqlite3_destructor_type> (-1);

namespace Util {

bool DBFSProvider::init() {
	static DBFSProvider provider;
	return FileUtils::registerFSProvider("dbfs", PointerHolderCreator<DBFSProvider>(&provider));
}

// -------------------------------

/*! (ctor) */
DBFSProvider::DBFSProvider() : AbstractFSProvider() {
}

/*! (dtor) */
DBFSProvider::~DBFSProvider(){
	flush();
}

/*! ---|> AbstractFSProvider    */
AbstractFSProvider::status_t DBFSProvider::makeDir(const FileName & path){
	if (isDir(path))
		return OK;
	//else if(isFile(path))
	//	return FAILURE;
	std::string dbFilename,folder,file;
	extractFileName(path, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename, true);
	if(dbh==nullptr)
		return FAILURE;

	if(dbh->makeDir(folder))
		return OK;
	else
		return FAILURE;
}

//! ---|> AbstractFSProvider
AbstractFSProvider::status_t DBFSProvider::makeDirRecursive(const FileName & name){
	if (isDir(name))
		return OK;
	std::string dbFilename,folder,file;
	extractFileName(name, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename, true);
	if(dbh==nullptr)
		return FAILURE;

	std::string s = name.toString();
	size_t pos = std::string::npos;
	if(s.back() == '/') {
		pos = s.rfind('/')-1;
	}
	FileName child(name.toString());
	makeDirRecursive(FileName(s.substr(0,s.rfind('/', pos))));
	return makeDir(name);
}

/*! ---|> AbstractFSProvider    */
AbstractFSProvider::status_t DBFSProvider::readFile(const FileName & filename, std::vector<uint8_t> & data){
	std::string dbFilename,folder,file;
	extractFileName(filename, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename);
	if(dbh==nullptr)
		return FAILURE;

	std::vector<uint8_t> d=dbh->readFile(folder,file);
	if(d.empty())
		return FAILURE;
	using std::swap;
	swap(data, d);
	return OK;
}

/*! ---|> AbstractFSProvider    */
AbstractFSProvider::status_t DBFSProvider::writeFile(const FileName & path, const std::vector<uint8_t> & data, bool overwrite){
	if(!overwrite && isFile(path))
		return FAILURE;

//std::cout << "!1";
	std::string dbFilename,folder,file;
	extractFileName(path, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename,true);
//std::cout << "!2";
	if(dbh==nullptr)
		return FAILURE;
//std::cout << "!3";

	if(dbh->saveFile(folder,file,data,overwrite))
		return OK;
	else
		return FAILURE;
}
//
///*! ---|> AbstractFSProvider    */
//bool DBFSProvider::exists(const FileName & filename){
//	std::string dbFilename,folder,file;
//	extractFileName(filename, dbFilename, folder, file);
//	DBHandle * dbh=getDBHandle(dbFilename);
//	size_t size=0;
//	if(dbh==nullptr || (!dbh->isFile(folder,file)) || !dbh->isFolder(folder) ) )
//		return false;
//	return type!=NO_ENTRY;
//}

/*! ---|> AbstractFSProvider    */
bool DBFSProvider::isFile(const FileName & filename){
	std::string dbFilename,folder,file;
	extractFileName(filename, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename);
	if(dbh==nullptr || !dbh->isFile(folder,file) )
		return false;
	return true;
}

/*! ---|> AbstractFSProvider    */
bool DBFSProvider::isDir(const FileName & filename){
	std::string dbFilename,folder,file;
	extractFileName(filename, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename);
	if(dbh==nullptr || dbh->getFolderId(folder)==NO_ENTRY )
		return false;
	return true;
}

/*! ---|> AbstractFSProvider    */
size_t DBFSProvider::fileSize(const FileName & filename){
	std::string dbFilename,folder,file;
	extractFileName(filename, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename);
	if(dbh==nullptr)
		return 0;
	return dbh->getSize(folder,file);
}

/*! ---|> AbstractFSProvider    */
AbstractFSProvider::status_t DBFSProvider::dir(const FileName &path, std::list<FileName> &result, uint8_t flags) {
	std::string dbFilename,folder,file;
	extractFileName(path, dbFilename, folder, file);
	DBHandle * dbh=getDBHandle(dbFilename);
	if( dbh==nullptr || !dbh->dir(folder,path.toString()+'/',result,flags))
		return FAILURE;
//	std::cout << " ### "<<result.size()<<"\n";
	return OK;
}

// --------------------------------------------------------


/**
 *
 */
DBFSProvider::DBHandle * DBFSProvider::createDB(const std::string & dbFilename){
	static const char * sql_createDB=
	"BEGIN TRANSACTION;	"

	"CREATE  TABLE Files (	folderId INTEGER NOT NULL , "
	"						name TEXT NOT NULL , "
	"						cTime DATETIME DEFAULT CURRENT_TIMESTAMP, "
	"						data BLOB, "
	"						PRIMARY KEY (folderId, name));	"
	"	"
	"CREATE  TABLE Folders (folderId INTEGER PRIMARY KEY AUTOINCREMENT , "
	"						parentId INTEGER NOT NULL , "
	"						name TEXT NOT NULL, "
	"						UNIQUE (folderId, parentId, name));	"
	"	"
	"DROP INDEX IF EXISTS 'filesInFolderIdx';	"
	"CREATE UNIQUE INDEX 'filesInFolderIdx' ON 'Files' (name ASC, folderId ASC);	"
	"DROP TRIGGER IF EXISTS deleteFolderTrigger;	"
	"CREATE TRIGGER deleteFolderTrigger AFTER DELETE ON Folders FOR EACH ROW  	"
	"	BEGIN	"
	"		DELETE FROM Files WHERE folderId = OLD.folderId;	"
	"	END;	"
	"	"
	"COMMIT;	";
	sqlite3*db=nullptr;
	int rc = sqlite3_open_v2(dbFilename.c_str(),&db, SQLITE_OPEN_READWRITE| SQLITE_OPEN_CREATE,nullptr);
	if( rc != SQLITE_OK){
		std::cerr << "Cannot create database: "<< sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return nullptr;
	}
	char *zErrMsg = nullptr;
	rc =sqlite3_exec(db,sql_createDB,nullptr,nullptr,&zErrMsg);
	if( rc!=SQLITE_OK ){
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return nullptr;
	}
	std::cout << "DBFSProvider: Container file created: "<< dbFilename<<"\n";
	return new DBFSProvider::DBHandle(db);
}

/**
 * (internal)
 */
DBFSProvider::DBHandle * DBFSProvider::getDBHandle(const std::string & dbFilename,bool createFile/* = false*/){
	DBFSProvider::DBHandle*dbHandle=nullptr;
	auto it=openHandles.find(dbFilename);
	if(it!=openHandles.end()){
		return it->second;
	}

	// file does not exist
	if(!FileUtils::isFile(FileName(dbFilename)) ){
		if(!createFile){
			return nullptr;
		}
		dbHandle=createDB(dbFilename);
		if(dbHandle==nullptr){
			return nullptr;
		}
		openHandles[dbFilename] = dbHandle;
		return dbHandle;
	}

	sqlite3 * db=nullptr;
	int rc = sqlite3_open_v2(dbFilename.c_str(),&db, SQLITE_OPEN_READWRITE,nullptr);
	if( rc != SQLITE_OK){
		std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return nullptr;
	}
	dbHandle = new DBHandle(db);
	openHandles[dbFilename] = dbHandle;

	return dbHandle;
}

/*! ---|> AbstractFSProvider    */
void DBFSProvider::flush(){
	for(auto & handle : openHandles) {
		delete handle.second;
	}
	openHandles.clear();
}

/*! "dbfs://test.dbfs$folder/test.txt" -> dbFilename:"test.db" folder:"folder/" file:"test.txt"	*/
void DBFSProvider::extractFileName(const FileName & filename,std::string & dbFilename,std::string & folder,std::string & file){
	file=filename.getFile();
	size_t splitPos=filename.getDir().find('$');
	if(splitPos==std::string::npos){
		dbFilename=filename.getDir();
		folder="";
		return;
	}
	dbFilename=filename.getDir().substr(0,splitPos);
	folder=filename.getDir().substr(splitPos+1);
//	std::cout << filename << "-> db:"<<dbFilename<<" fo:"<<folder<<" fi:"<<file<<"\n";
}


// ----------------------------------------------------------------------

/*! (ctor) DBFSProvider::DBHandle */
DBFSProvider::DBHandle::DBHandle(sqlite3 *_db):
		db(_db),
		getFolderId_stmt(nullptr),getFileData_stmt(nullptr),getFileSize_stmt(nullptr),
		isFile_stmt(nullptr),dirFiles_stmt(nullptr),dirFolders_stmt(nullptr), mutex(Concurrency::createMutex()){
}

/*! (dtor) DBFSProvider::DBHandle */
DBFSProvider::DBHandle::~DBHandle(){
	flush();

	auto lock = Concurrency::createLock(*mutex);
	sqlite3_finalize(getFolderId_stmt);
	sqlite3_finalize(getFileData_stmt);
	sqlite3_finalize(getFileSize_stmt);
	sqlite3_finalize(isFile_stmt);
	sqlite3_finalize(dirFiles_stmt);
	sqlite3_finalize(dirFolders_stmt);

	sqlite3_close(db);
}

/*! (static) Helper */
sqlite3_stmt * DBFSProvider::createStatement(sqlite3 * db,const char * sql){
	if(db==nullptr)
		return nullptr;
	sqlite3_stmt * stmt=nullptr;
	int rc=sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);
	if( rc!=SQLITE_OK ){
		std::cerr << "SQL error: " << sqlite3_errmsg(db) << "\nIn: " << sql << std::endl;
		return nullptr;
	}
	return stmt;
}

/*! (internal) DBFSProvider::DBHandle */
void DBFSProvider::DBHandle::storeStatement(int folderId,const std::string & file,sqlite3_stmt * stmt){
	std::ostringstream s;
	s<<folderId<<"/"<<file;
	size_t size = 0;
	{
		auto lock = Concurrency::createLock(*mutex);
		deferredFiles.insert(s.str());

		deferredStatements.push_back(stmt);
		size = deferredStatements.size();
	}
	if(size > MAX_DEFFERED_STATEMENTS)
		flush();
}

/*! (internal) DBFSProvider::DBHandle */
bool DBFSProvider::DBHandle::isPendingFile(int folderId,const std::string & file){
	std::ostringstream s;
	s<<folderId<<"/"<<file;
	return deferredFiles.count(s.str())!=0;
}

/*! DBFSProvider::DBHandle */
// \todo implement caching???
int DBFSProvider::DBHandle::getFolderId(const std::string & folder){
	if(folder==".")
		return 0;

	auto lock = Concurrency::createLock(*mutex);
	if(getFolderId_stmt==nullptr){
		getFolderId_stmt=createStatement(getDB(),"SELECT folderId FROM Folders WHERE name = :name AND parentId = :id ;");
		if(getFolderId_stmt==nullptr)
			return NO_ENTRY;
	}
	int id=0; // root
	size_t from=0;
	while(true){
		size_t to=folder.find('/',from);
		if(to==std::string::npos)
			break;
		std::string f=folder.substr(from,to-from);
		from=to+1;
		if(f.empty() || f == ".")
			continue;
		sqlite3_reset(getFolderId_stmt);
		sqlite3_bind_text(getFolderId_stmt,1,f.c_str(),-1,SQLITE_TRANSIENT_CXX); // :name
		sqlite3_bind_int(getFolderId_stmt,2,id); // :id
		int rc=sqlite3_step(getFolderId_stmt);
		if(rc!=SQLITE_ROW){
			std::cout << "folder not found: "<<f<<"\n";
			return NO_ENTRY;
		}else{
			id=sqlite3_column_int(getFolderId_stmt,0);
		}
	}
	return id;
}


/*! DBFSProvider::DBHandle */
bool DBFSProvider::DBHandle::saveFile(const std::string & folder,const std::string & file,const std::vector<uint8_t> & data,bool overwrite/*=true*/){
	int folderId = getFolderId(folder);
	if(folderId==NO_ENTRY){
		WARN("Folder does not exist: "+folder);
		return false;
	}
	if(!overwrite && isFile(folder,file)){
		return false;
	}
	sqlite3_stmt * stmt=createStatement(getDB(),"INSERT OR REPLACE INTO Files (folderId,name,data) VALUES (:folderId,:name,:data);");
	if(stmt==nullptr)
		return false;

//	std::cout << "Inserting file "<<file<<" into folder "<<folderId<<"\n";
	sqlite3_reset(stmt);
	sqlite3_bind_int(stmt,1,folderId);
	sqlite3_bind_text(stmt,2,file.c_str(),-1,SQLITE_TRANSIENT_CXX);
	sqlite3_bind_blob(stmt,3,data.data(),data.size(),SQLITE_TRANSIENT_CXX);

	storeStatement(folderId,file,stmt);
	return true;
}

/*! DBFSProvider::DBHandle */
std::vector<uint8_t> DBFSProvider::DBHandle::readFile(const std::string & folder,const std::string & file){
	int folderId = getFolderId(folder);
	if(folderId==NO_ENTRY){
		WARN("Folder does not exist: "+folder);
		return std::vector<uint8_t>();
	}
	if(isPendingFile(folderId,file)){
		flush();
	}
	auto lock = Concurrency::createLock(*mutex);
	if(getFileData_stmt==nullptr){
		getFileData_stmt=createStatement(getDB(),"SELECT data FROM Files WHERE folderId = :folderId AND name = :name ;");
		if(getFileData_stmt==nullptr)
			return std::vector<uint8_t>();
	}
	sqlite3_reset(getFileData_stmt);
	sqlite3_bind_int(getFileData_stmt,1,folderId);
	sqlite3_bind_text(getFileData_stmt,2,file.c_str(),-1,SQLITE_TRANSIENT_CXX);

	int rc=sqlite3_step(getFileData_stmt);
	if(rc!=SQLITE_ROW){
		WARN( "File not found: " + folder + '/' + file );
		return std::vector<uint8_t>();
	}
	const uint8_t * dataTmp = reinterpret_cast<const uint8_t *> (sqlite3_column_blob(getFileData_stmt, 0));  // sqlite should free this automatically
	const size_t size = static_cast<size_t>(sqlite3_column_bytes(getFileData_stmt, 0));

	return std::vector<uint8_t>(dataTmp, dataTmp + size);
}

/*! DBFSProvider::DBHandle */
size_t DBFSProvider::DBHandle::getSize(const std::string & folder,const std::string & file){
	int folderId = getFolderId(folder);
	if(folderId==NO_ENTRY){
		WARN("Folder does not exist: "+folder);
		return 0;
	}
	if(isPendingFile(folderId,file)){
		flush();
	}
	auto lock = Concurrency::createLock(*mutex);
	if(getFileSize_stmt==nullptr){
		getFileSize_stmt=createStatement(getDB(),"SELECT length(data) FROM Files WHERE folderId = :folderId AND name = :name ;");
		if(getFileSize_stmt==nullptr)
			return false;
	}
	sqlite3_reset(getFileSize_stmt);
	sqlite3_bind_int(getFileSize_stmt,1,folderId);
	sqlite3_bind_text(getFileSize_stmt,2,file.c_str(),-1,SQLITE_TRANSIENT_CXX);

	int rc=sqlite3_step(getFileSize_stmt);
	if(rc!=SQLITE_ROW){
		return false;
	}
	size_t size = static_cast<size_t> (sqlite3_column_int(getFileSize_stmt, 0));
	return size;
}

/*! DBFSProvider::DBHandle */
bool DBFSProvider::DBHandle::isFile(const std::string & folder,const std::string & file){
	int folderId = getFolderId(folder);
	if(folderId==NO_ENTRY){
		WARN("Folder does not exist: "+folder);
		return false;
	}
	if(isPendingFile(folderId,file)){
		return true;
	}
	auto lock = Concurrency::createLock(*mutex);
	if(isFile_stmt==nullptr){
		isFile_stmt=createStatement(getDB(),"SELECT 1 FROM Files WHERE folderId = :folderId AND name = :name ;");
		if(isFile_stmt==nullptr)
			return false;
	}
	sqlite3_reset(isFile_stmt);
	sqlite3_bind_int(isFile_stmt,1,folderId);
	sqlite3_bind_text(isFile_stmt,2,file.c_str(),-1,SQLITE_TRANSIENT_CXX);

	return (sqlite3_step(isFile_stmt) == SQLITE_ROW);
}

/*! DBFSProvider::DBHandle */
bool DBFSProvider::DBHandle::dir(const std::string & folder, const std::string & prefix, std::list<FileName> &result, uint8_t flags){
	flush();
	int folderId = getFolderId(folder);
	if(folderId==NO_ENTRY){
		WARN("Folder does not exist: "+folder);
		return false;
	}
	if(flags & FileUtils::DIR_RECURSIVE){
		WARN("TODO: DIR_RECURSIVE not implemented yet.\n");
	}

	auto lock = Concurrency::createLock(*mutex);
	if(flags & FileUtils::DIR_FILES){
		if(dirFiles_stmt==nullptr){
			dirFiles_stmt=createStatement(getDB(),"SELECT name FROM Files WHERE folderId = :folderId ;");
			if(dirFiles_stmt==nullptr)
				return false;
		}
		sqlite3_reset(dirFiles_stmt);
		sqlite3_bind_int(dirFiles_stmt,1,folderId);

		while(sqlite3_step(dirFiles_stmt) == SQLITE_ROW){
			const char * nameTmp = reinterpret_cast<const char *> (sqlite3_column_text(dirFiles_stmt, 0));  // sqlite should free this automatically
			size_t size = static_cast<size_t> (sqlite3_column_bytes(dirFiles_stmt, 0));

			result.emplace_back(prefix + std::string(nameTmp,size));
		}
	}
	if( (flags&FileUtils::DIR_DIRECTORIES) || (flags & FileUtils::DIR_RECURSIVE)){
		if(dirFolders_stmt==nullptr){
			dirFolders_stmt=createStatement(getDB(),"SELECT name FROM Folders WHERE parentId = :folderId;");
			if(dirFolders_stmt==nullptr)
				return false;
			while(sqlite3_step(dirFolders_stmt) == SQLITE_ROW){
				const char * nameTmp = reinterpret_cast<const char *> (sqlite3_column_text(dirFolders_stmt, 0));  // sqlite should free this automatically
				size_t size = static_cast<size_t> (sqlite3_column_bytes(dirFolders_stmt, 0));

				if((flags&FileUtils::DIR_DIRECTORIES)){
					result.push_back(FileName::createDirName(prefix+std::string(nameTmp,size)));
				}
//				if(flags & FileUtils::DIR_RECURSIVE){
//					WARN("TODO: DIR_RECURSIVE not implemented yet.\n");
//				}
			}
		}
	}

//	std::cout <<": "<<rc << " ";
	return true;
}

/*! DBFSProvider::DBHandle */
bool DBFSProvider::DBHandle::makeDir(const std::string & folder){
	flush();
	int folderId = getFolderId(folder);
	if(folderId==NO_ENTRY){
		int parentId = 0;
		std::string folderName = folder.substr(0,folder.length()-1);
		size_t splitPos=folderName.rfind('/');
		if(splitPos!=std::string::npos){
			parentId = getFolderId(folder.substr(0,splitPos));
			if(parentId==NO_ENTRY)
				return false;
			folderName = folder.substr(splitPos+1, folder.length()-splitPos);
		}

		sqlite3_stmt * stmt=createStatement(getDB(),"INSERT OR REPLACE INTO Folders (parentId,name) VALUES (:parentId,:name);");
		if(stmt==nullptr)
			return false;

		sqlite3_reset(stmt);
		sqlite3_bind_int(stmt,1,parentId);
		sqlite3_bind_text(stmt,2,folderName.c_str(),-1,SQLITE_TRANSIENT_CXX);

		storeStatement(parentId,folderName,stmt);
		flush();
		return true;
	}
	return true;
}

/*! DBFSProvider::DBHandle */
void DBFSProvider::DBHandle::flush(){
	auto lock = Concurrency::createLock(*mutex);
	if(deferredStatements.empty())
		return;
	std::cout << "DB FLUSH!\n";

	deferredFiles.clear();

	char *zErrMsg = nullptr;
	int rc =sqlite3_exec(db,"BEGIN TRANSACTION;",nullptr,nullptr,&zErrMsg);
	if( rc!=SQLITE_OK ){
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
	for(auto & stmt : deferredStatements) {
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	deferredStatements.clear();
	rc =sqlite3_exec(db,"END TRANSACTION;",nullptr,nullptr,&zErrMsg);
	if( rc!=SQLITE_OK ){
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}

}

}

#endif // UTIL_HAVE_LIB_SQLITE
