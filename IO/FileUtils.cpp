/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "FileUtils.h"

#include "AbstractFSProvider.h"
#include "FileName.h"
#include "../Factory/Factory.h"
#include "../Macros.h"
#include "../References.h"
#include "../StringUtils.h"
#include "../Utils.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <utility>
#include <vector>

namespace Util {

typedef Factory<AbstractFSProvider *, std::string, std::function<AbstractFSProvider * ()>, FallbackPolicies::NULLFallback> provider_factory_t;

//! Local singleton function for factory creating providers
static provider_factory_t & getProviderFactory() {
	static provider_factory_t factory;
	return factory;
}

bool FileUtils::registerFSProvider(const std::string & fsName, std::function<AbstractFSProvider * ()> providerCreator) {
	return getProviderFactory().registerType(fsName, providerCreator);
}

static AbstractFSProvider * getFSProvider(const FileName & path) {
	AbstractFSProvider * provider = getProviderFactory().create(path.getFSName());
	if(provider == nullptr) {
		throw std::runtime_error(std::string("No file system provider for \"") + path + "\".");
	}
	return provider;
}

bool FileUtils::createDir(const FileName & name, bool recursive) {
	if(recursive)
		return AbstractFSProvider::OK == getFSProvider(name)->makeDirRecursive(name);
	else
		return AbstractFSProvider::OK == getFSProvider(name)->makeDir(name);
}

bool FileUtils::remove(const FileName & name, bool recursive){
	if(recursive)
		return AbstractFSProvider::OK == getFSProvider(name)->removeRecursive(name);
	else
		return AbstractFSProvider::OK == getFSProvider(name)->remove(name);
}

//! (static)
void FileUtils::flush(const FileName & path) {
	getFSProvider(path)->flush();
}

//! (static)
std::unique_ptr<std::iostream> FileUtils::open(const FileName & fileName){
	AbstractFSProvider * provider = getFSProvider(fileName);
	std::unique_ptr<std::iostream> stream(provider->open(fileName));
	if(stream) {
		return stream;
	}
	// Alternative path: Try to read the file, and build a stream from it that will write the file on destruction.
	class InOutStream : public std::stringstream {
		private:
			AbstractFSProvider * provider;
			FileName fileName;

		public:
			InOutStream(AbstractFSProvider * _provider, FileName _fileName, const std::string & initialContent) :
					std::stringstream(initialContent, std::ios_base::in | std::ios_base::out | std::ios_base::binary), provider(_provider), fileName(std::move(_fileName)) {
			}
			virtual ~InOutStream() {
				const std::string stringData = str();
				if(!stringData.empty()) {
					const std::vector<uint8_t> data(stringData.begin(), stringData.end());
					provider->writeFile(fileName, data, true);
				}
			}
	};

	// Check if file reading is supported. Write support is not checked here.
	std::vector<uint8_t> data;
	if(provider->readFile(fileName, data) != AbstractFSProvider::OK) {
		return nullptr;
	}

	return std::unique_ptr<std::iostream>(new InOutStream(provider, fileName, std::string(data.begin(), data.end())));
}

//! (static)
std::unique_ptr<std::istream> FileUtils::openForReading(const FileName & fileName) {
	AbstractFSProvider * provider = getFSProvider(fileName);
	auto stream = provider->openForReading(fileName);
	if(stream) {
		return stream;
	}

	// Check if file reading is supported.
	std::vector<uint8_t> data;
	if(provider->readFile(fileName, data) != AbstractFSProvider::OK) {
		return nullptr;
	}

	return std::unique_ptr<std::istream>(new std::istringstream(std::string(data.begin(), data.end()), 
																std::ios_base::in | std::ios_base::binary));
}

//! (static)
std::unique_ptr<std::ostream> FileUtils::openForWriting(const FileName & fileName){
	AbstractFSProvider * provider = getFSProvider(fileName);
	auto stream = provider->openForWriting(fileName);
	if(stream) {
		return stream;
	}
	// Alternative path: Try to build a stream that will write the file on destruction.
	class OutStream : public std::ostringstream {
		private:
			AbstractFSProvider * provider;
			FileName fileName;

		public:
			OutStream(AbstractFSProvider * _provider, FileName _fileName) :
					std::ostringstream(std::ios_base::out | std::ios_base::binary | std::ios_base::trunc), provider(_provider), fileName(std::move(_fileName)) {
			}
			virtual ~OutStream() {
				const std::string stringData = str();
				if(!stringData.empty()) {
					const std::vector<uint8_t> data(stringData.begin(), stringData.end());
					provider->writeFile(fileName, data, true);
				}
			}
	};

	// Check if file writing is supported. If a stream would have been opened, the file would have been truncated too.
	if(provider->writeFile(fileName, std::vector<uint8_t>(), true) != AbstractFSProvider::OK) {
		return nullptr;
	}

	return std::unique_ptr<std::ostream>(new OutStream(provider, fileName));
}

//! (static)
std::unique_ptr<std::ostream> FileUtils::openForAppending(const FileName & fileName) {
	AbstractFSProvider * provider = getFSProvider(fileName);
	auto stream = provider->openForAppending(fileName);
	if(stream) {
		return stream;
	}
	// Alternative path: Try to build a stream that will write the file on destruction.
	class AppendStream : public std::ostringstream {
		private:
			AbstractFSProvider * provider;
			FileName fileName;

		public:
			AppendStream(AbstractFSProvider * _provider, FileName _fileName, const std::string & initialContent) :
					std::ostringstream(initialContent, std::ios_base::out | std::ios_base::binary | std::ios_base::app), provider(_provider), fileName(std::move(_fileName)) {
			}
			virtual ~AppendStream() {
				const std::string stringData = str();
				if(!stringData.empty()) {
					const std::vector<uint8_t> data(stringData.begin(), stringData.end());
					provider->writeFile(fileName, data, true);
				}
			}
	};

	// Check if file reading is supported. Write support is not checked here.
	std::vector<uint8_t> data;
	if(provider->readFile(fileName, data) != AbstractFSProvider::OK) {
		return nullptr;
	}

	return std::unique_ptr<std::ostream>(new AppendStream(provider, fileName, std::string(data.begin(), data.end())));
}

// ----------------------------------------------
// ---- Loading and saving complete files

//! (static)
std::vector<uint8_t> FileUtils::loadFile(const FileName & filename){
	AbstractFSProvider * p = getFSProvider(filename);

	std::vector<uint8_t> binData;
	AbstractFSProvider::status_t status=p->readFile(filename, binData);
	if(status == AbstractFSProvider::OK)
		return binData;
	WARN( std::string("Could not load file: ")+filename+" ("+AbstractFSProvider::getStatusMessage(status)+')');
	return std::vector<uint8_t>();
}

//! (static)
std::string FileUtils::getFileContents(const FileName & filename) {
	const std::vector<uint8_t> data = loadFile(filename);
	if(data.empty()) {
		return std::string();
	}
	return std::string(data.begin(), data.end());
}

//! (static)
std::string FileUtils::getParsedFileContents(const FileName & filename){
	std::string content=getFileContents(filename);
	if(content.empty())
		return content;

	std::ostringstream s;

	size_t lastPos=0;
	size_t pos=0;

	static const std::string startMarker="/*{{";
	static const std::string endMarker="}}*/";
	std::vector<std::string> pathHints;

	/*{{ include "foo.h" }}*/
	while( (pos=content.find(startMarker,lastPos))!=std::string::npos ){
		s<<content.substr(lastPos,pos-lastPos);

		size_t endPos=content.find(endMarker,pos);
		if(endPos==std::string::npos){
			WARN(std::string("getParsedFileContents: Unclosed meta block /*{{ }}*/ in ")+filename.toString());
			break;
		}
		std::string metaBlock=content.substr(pos+4,endPos-pos-4);
		int cursor=0;
		StringUtils::stepWhitespaces(metaBlock.c_str(),cursor);
		if(StringUtils::stepText(metaBlock.c_str(),cursor,"include")){
			if(pathHints.empty()){
				pathHints.push_back(filename.getDir());
				pathHints.push_back("");
			}
			StringUtils::stepWhitespaces(metaBlock.c_str(),cursor);
			FileName includeFile=FileName(StringUtils::readQuotedString(metaBlock.c_str(),cursor));
			FileName includeFile2;
			findFile(includeFile, pathHints, includeFile2);
			s<<getParsedFileContents(includeFile2);
		}else if(StringUtils::stepText(metaBlock.c_str(),cursor,"comment")){
			// ignore ..
		}else{
			WARN(std::string("getParsedFileContents: Unknown meta block command: '")+metaBlock+"' in "+filename.toString());
		}
//		std::cout <<"'"<< metaBlock<<"'\n";
		lastPos=endPos + 4; // jump after meta block
	}
	s<<content.substr(lastPos);
	return s.str();
}

//! (static)
bool FileUtils::saveFile(const FileName & filename,const std::vector<uint8_t> & data,bool overwrite/*=true*/){
	AbstractFSProvider * p = getFSProvider(filename);

	AbstractFSProvider::status_t status=p->writeFile(filename,data,overwrite);
	if(status == AbstractFSProvider::OK)
		return true;
//    WARN( std::string("Could not save file: ")+filename+" ("+AbstractFSProvider::getStatusMessage(status)+")" );
	return false;
}

size_t FileUtils::fileSize(const FileName & filename){
	return getFSProvider(filename)->fileSize(filename);
}

bool FileUtils::isFile(const FileName & filename){
	return getFSProvider(filename)->isFile(filename);
}

bool FileUtils::isDir(const FileName & filename){
	return getFSProvider(filename)->isDir(filename);
}

bool FileUtils::dir(const FileName & path, std::list<FileName> & result, uint8_t flags){
	return getFSProvider(path)->dir(path,result,flags) == AbstractFSProvider::OK;
}

bool FileUtils::findFile(const FileName & fileName, const std::vector<std::string> & pathHints, FileName & newName) {
	FileName modifiedFileName(fileName);
	const std::string & fileDir = fileName.getDir();
	// Check if we are supposed to find a file inside an archive.
	const size_t dollarPos = fileDir.find('$');
	if(dollarPos != std::string::npos) {
		const std::string archiveName(fileDir, 0, dollarPos);
		// Try to find the archive first.
		FileName newArchiveName;
		bool success = findFile(FileName(archiveName), pathHints, newArchiveName);
		if(!success) {
			return false;
		}
		// Replace archive part of fileName by new archive file path.
		modifiedFileName.setDir(newArchiveName.getPath() + fileDir.substr(dollarPos));
	}
	newName = modifiedFileName;
	if (isFile(newName)) {
		return true;
	}
	for(auto hint : pathHints) {
		newName = modifiedFileName;

		const size_t pos = hint.find("://");
		if(pos != std::string::npos) {
			newName.setFSName(hint.substr(0, pos));
			hint = hint.substr(pos + 3);
		}

		newName.setDir(hint + modifiedFileName.getDir());
		if (isFile(newName)) {
			info << "\"" << fileName.toString() << "\" found as \"" << newName.toString() << "\".\n";
			return true;
		}
		newName.setDir(hint);
		if (isFile(newName)) {
			info << "\"" << fileName.toString() << "\" found as \"" << newName.toString() << "\".\n";
			return true;
		}
	}
	return false;
}

bool FileUtils::makeRelativeIfPossible(const FileName & fixedPath,FileName & path){
	std::string fixedDir( fixedPath.getDir() );

	// Only the "real"-file system part of an archive should be touched
	const size_t dollarPos = fixedDir.find('$');
	if(dollarPos != std::string::npos) {
		FileName tmp( fixedDir.substr(0,dollarPos) );
		fixedDir = tmp.getDir();
	}
	if( path.getDir().length()>=fixedDir.length() && path.getDir().compare(0,fixedDir.length(),fixedDir)==0 ){
		path.setDir( path.getDir().substr(fixedDir.length()) );
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------

//! (static)
FileName FileUtils::generateNewRandFilename(const FileName & dir,const std::string & prefix,const std::string & postfix,int randomSize){
	FileName fileName = dir;
	int i = 0;
	static std::default_random_engine engine;
	std::uniform_int_distribution<char> distribution(97, 122);
	do {
		if(i++ > 100){
			++randomSize;
			i=0;
		}
		// Create random file name.
		std::string name(prefix);
		for (unsigned char j = 0; j < randomSize; ++j) {
			// Characters from [a-z].
			const char random = distribution(engine);
			name += random;
		}
		name += postfix;
		fileName.setFile(name);
	} while (isFile(fileName));
	return fileName;
}

// ---------------------------------------------------------------------------

//! (static)
bool FileUtils::copyFile(const FileName & source, const FileName & dest) {
	AbstractFSProvider * inputProvider = getFSProvider(source);
	AbstractFSProvider * outputProvider = getFSProvider(dest);

	auto inputStream = inputProvider->openForReading(source);
	if (!inputStream) {
		WARN("Unable to get a stream for reading.");
		return false;
	}
	auto outputStream = outputProvider->openForWriting(dest);
	if (!outputStream) {
		WARN("Unable to get a stream for writing.");
		return false;
	}
	// Use a fixed block of 8 MiBytes here to guarantee a fixed memory footprint for large files.
	const std::streamsize bufferSize = 8 * 1024 * 1024;
	auto buffer = new char[bufferSize];
	while (inputStream->good()) {
		if (!outputStream->good()) {
			delete[] buffer;
			WARN("Failure writing to stream.");
			return false;
		}
		inputStream->read(buffer, bufferSize);
		const std::streamsize charsRead = inputStream->gcount();
		outputStream->write(buffer, charsRead);
	}
	delete[] buffer;
	return true;
}

}
