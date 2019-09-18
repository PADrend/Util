/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "IO/FileName.h"
#include "IO/FileUtils.h"
#include "IO/TemporaryDirectory.h"
#include "StringUtils.h"
#include "Utils.h"
#include <catch2/catch.hpp>

/**
 * @param directory Path to a temporary directory. The directory has to be
 * empty. To test different file system providers, the directory may be
 * located in a virtual file system (e.g. inside a zip archive).
 */
static void testFileSystem(const Util::FileName & directory) {
	using namespace Util;

	FileName filename(directory.toString() + "test.txt");

	std::string s("Hello world, foo bar. It is now " + Utils::createTimeStamp() + '\n');

	{   // openForWriting
		auto outs = FileUtils::openForWriting(filename);
		REQUIRE(outs);
		*outs << s;
		REQUIRE(outs->good());
	}

	{   // fileSize
		size_t s1=FileUtils::fileSize(filename);
		REQUIRE(s.size() == s1);
	}

	{   // isFile
		bool b1=FileUtils::isFile(filename);
		REQUIRE(b1);
		bool b2=FileUtils::isFile(FileName(filename.toString()+"foo"));
		REQUIRE_FALSE(b2);
	}

	{   // dir (very basic check)
		std::list<FileName> files;
		FileUtils::dir(directory, files, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
		REQUIRE(files.size() == 1);
	}

	{ // openForReading
		auto in = FileUtils::openForReading(filename);
		REQUIRE(in);

		std::string s2;
		while(in->good()){
			char c = static_cast<char> (in->get());
			if(in->good())
				s2+=c;
		}
		REQUIRE(s == s2);
	}

	{ // openForAppending
		auto out = FileUtils::openForAppending(filename);
		REQUIRE(out);

		std::string app="Appended:Foo";
		*out << app;
		s+=app;
		out.reset();
		const std::string data=FileUtils::getFileContents(filename);
		REQUIRE(!data.empty());
		REQUIRE(s == data);
	}

	{ // open
		auto io = FileUtils::open(filename);
		REQUIRE(io);

		char c = static_cast<char> (io->get());
		io->seekp(0);
		*io << c << c << c;
		*io << ":-)";

		s.replace(0,6,"HHH:-)");
		io.reset();
		const std::string data=FileUtils::getFileContents(filename);
		REQUIRE(!data.empty());
		REQUIRE(s == data);
	}
	s+="\nTest2";

	{   // saveFile
		bool b1=FileUtils::saveFile(filename,std::vector<uint8_t>(s.begin(), s.end()),true);
		REQUIRE(b1);
		bool b2=FileUtils::saveFile(filename,std::vector<uint8_t>(s.begin(), s.end()),false);
		REQUIRE_FALSE(b2);
	}

	{   // loadFile
		const std::vector<uint8_t> data=FileUtils::loadFile(filename);
		REQUIRE(!data.empty());
		REQUIRE(s == std::string(data.begin(), data.end()));
	}

	// File remove (dbfs does not support remove function)
	if(directory.getFSName() != "dbfs") {
		{
			REQUIRE(FileUtils::isFile(filename));
			REQUIRE(FileUtils::remove(filename));
			REQUIRE_FALSE(FileUtils::isFile(filename));
		}
	}

	// Directory operations (dbfs does not support createDir, remove functions)
	if(directory.getFSName() != "dbfs") {
		{
			const Util::FileName missingDir(directory.toString() + "missing/");
			const Util::FileName flatDir(directory.toString() + "directory/");
			const Util::FileName deepDir1(flatDir.toString() + "subdirectory/");
			const Util::FileName deepDir2(deepDir1.toString() + "subdirectory/");
			const Util::FileName deepDir3(deepDir2.toString() + "subdirectory/");

			REQUIRE_FALSE(FileUtils::isDir(missingDir));
			REQUIRE_FALSE(FileUtils::isDir(flatDir));
			REQUIRE_FALSE(FileUtils::isDir(deepDir1));
			REQUIRE_FALSE(FileUtils::isDir(deepDir2));
			REQUIRE_FALSE(FileUtils::isDir(deepDir3));

			REQUIRE(FileUtils::createDir(flatDir));

			REQUIRE_FALSE(FileUtils::isDir(missingDir));
			REQUIRE(FileUtils::isDir(flatDir));
			REQUIRE_FALSE(FileUtils::isDir(deepDir1));
			REQUIRE_FALSE(FileUtils::isDir(deepDir2));
			REQUIRE_FALSE(FileUtils::isDir(deepDir3));

			REQUIRE(FileUtils::createDir(deepDir3));

			REQUIRE_FALSE(FileUtils::isDir(missingDir));
			REQUIRE(FileUtils::isDir(flatDir));
			REQUIRE(FileUtils::isDir(deepDir1));
			REQUIRE(FileUtils::isDir(deepDir2));
			REQUIRE(FileUtils::isDir(deepDir3));

			REQUIRE(FileUtils::remove(deepDir3));

			REQUIRE_FALSE(FileUtils::isDir(missingDir));
			REQUIRE(FileUtils::isDir(flatDir));
			REQUIRE(FileUtils::isDir(deepDir1));
			REQUIRE(FileUtils::isDir(deepDir2));
			REQUIRE_FALSE(FileUtils::isDir(deepDir3));

			REQUIRE(FileUtils::remove(deepDir2));

			REQUIRE_FALSE(FileUtils::isDir(missingDir));
			REQUIRE(FileUtils::isDir(flatDir));
			REQUIRE(FileUtils::isDir(deepDir1));
			REQUIRE_FALSE(FileUtils::isDir(deepDir2));
			REQUIRE_FALSE(FileUtils::isDir(deepDir3));

			REQUIRE(FileUtils::remove(flatDir, true));

			REQUIRE_FALSE(FileUtils::isDir(missingDir));
			REQUIRE_FALSE(FileUtils::isDir(flatDir));
			REQUIRE_FALSE(FileUtils::isDir(deepDir1));
			REQUIRE_FALSE(FileUtils::isDir(deepDir2));
			REQUIRE_FALSE(FileUtils::isDir(deepDir3));
		}
	}

	// dir with different parameters (dbfs lacks support for various functions)
	if(directory.getFSName() != "dbfs") {
		{
			{
				std::list<FileName> files;
				FileUtils::dir(directory, files, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
				REQUIRE(files.empty());
			}

			for(uint_fast16_t dirLevel0 = 0; dirLevel0 < 5; ++dirLevel0) {
				const FileName dirLevel0FileName(directory.toString() + Util::StringUtils::toString(dirLevel0) + '/');
				REQUIRE(FileUtils::createDir(dirLevel0FileName));
				for(uint_fast16_t dirLevel1 = 0; dirLevel1 < 4; ++dirLevel1) {
					const FileName dirLevel1FileName(dirLevel0FileName.toString() + Util::StringUtils::toString(dirLevel1) + '/');
					REQUIRE(FileUtils::createDir(dirLevel1FileName));
					for(uint_fast16_t dirLevel2 = 0; dirLevel2 < 3; ++dirLevel2) {
						const FileName dirLevel2FileName(dirLevel1FileName.toString() + Util::StringUtils::toString(dirLevel2) + '/');
						REQUIRE(FileUtils::createDir(dirLevel2FileName));
						for(uint_fast16_t fileNumber = 0; fileNumber < 2; ++fileNumber) {
							REQUIRE(FileUtils::saveFile(FileName(dirLevel2FileName.toString() + Util::StringUtils::toString(fileNumber)),
																	 std::vector<uint8_t>(2),
																	 true));
						}
						REQUIRE(FileUtils::saveFile(FileName(dirLevel2FileName.toString() + ".hidden"),
																 std::vector<uint8_t>(2),
																 true));
					}
					REQUIRE(FileUtils::saveFile(FileName(dirLevel1FileName.toString() + "level1File"),
															 std::vector<uint8_t>(2),
															 true));
				}
			}

			// Recursive
			{
				std::list<FileName> allEntries;
				FileUtils::dir(directory, allEntries, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
				REQUIRE(static_cast<size_t>(285) == allEntries.size());
			}
			{
				std::list<FileName> dirs;
				FileUtils::dir(directory, dirs, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_RECURSIVE);
				REQUIRE(static_cast<size_t>(85) == dirs.size());
			}
			{
				std::list<FileName> files;
				FileUtils::dir(directory, files, FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
				REQUIRE(static_cast<size_t>(200) == files.size());
			}
			{
				std::list<FileName> filesWithoutHidden;
				FileUtils::dir(directory, filesWithoutHidden, FileUtils::DIR_FILES | FileUtils::DIR_RECURSIVE);
				REQUIRE(static_cast<size_t>(140) == filesWithoutHidden.size());
			}
			// Non-recursive
			{
				std::list<FileName> topLevel;
				FileUtils::dir(directory, topLevel, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
				REQUIRE(static_cast<size_t>(5) == topLevel.size());
			}
			{
				std::list<FileName> level0;
				FileUtils::dir(Util::FileName(directory.toString() + "4/"), level0, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
				REQUIRE(static_cast<size_t>(4) == level0.size());
			}
			{
				const Util::FileName level1FileName(directory.toString() + "1/0/");
				std::list<FileName> level1Dirs;
				FileUtils::dir(level1FileName, level1Dirs, FileUtils::DIR_DIRECTORIES);
				REQUIRE(static_cast<size_t>(3) == level1Dirs.size());
				std::list<FileName> level1Files;
				FileUtils::dir(level1FileName, level1Files, FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
				REQUIRE(static_cast<size_t>(1) == level1Files.size());
			}
			{
				const Util::FileName level2FileName(directory.toString() + "3/2/1/");
				std::list<FileName> level2Dirs;
				FileUtils::dir(level2FileName, level2Dirs, FileUtils::DIR_DIRECTORIES);
				REQUIRE(level2Dirs.empty());
				std::list<FileName> level2Files;
				FileUtils::dir(level2FileName, level2Files, FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
				REQUIRE(static_cast<size_t>(3) == level2Files.size());
				std::list<FileName> level2FilesWithoutHidden;
				FileUtils::dir(level2FileName, level2FilesWithoutHidden, FileUtils::DIR_FILES);
				REQUIRE(static_cast<size_t>(2) == level2FilesWithoutHidden.size());
			}

			// Benchmark for lots of isFile and isDir calls
			for(uint_fast16_t dirLevel0 = 0; dirLevel0 < 5; ++dirLevel0) {
				const FileName dirLevel0FileName(directory.toString() + Util::StringUtils::toString(dirLevel0) + '/');
				REQUIRE(FileUtils::isDir(dirLevel0FileName));
				for(uint_fast16_t dirLevel1 = 0; dirLevel1 < 4; ++dirLevel1) {
					const FileName dirLevel1FileName(dirLevel0FileName.toString() + Util::StringUtils::toString(dirLevel1) + '/');
					REQUIRE(FileUtils::isDir(dirLevel1FileName));
					for(uint_fast16_t dirLevel2 = 0; dirLevel2 < 3; ++dirLevel2) {
						const FileName dirLevel2FileName(dirLevel1FileName.toString() + Util::StringUtils::toString(dirLevel2) + '/');
						REQUIRE(FileUtils::isDir(dirLevel2FileName));
						for(uint_fast16_t fileNumber = 0; fileNumber < 2; ++fileNumber) {
							REQUIRE(FileUtils::isFile(FileName(dirLevel2FileName.toString() + Util::StringUtils::toString(fileNumber))));
						}
						REQUIRE(FileUtils::isFile(FileName(dirLevel2FileName.toString() + ".hidden")));
					}
					REQUIRE(FileUtils::isFile(FileName(dirLevel1FileName.toString() + "level1File")));
				}
			}
		}
	}
}

#ifdef UTIL_HAVE_LIB_ARCHIVE
TEST_CASE("FileUtilsTest_testARCHIVE", "[FileUtilsTest]") {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testARCHIVE");
	Util::FileName testDirectory = tempDir.getPath();
	testDirectory.setFSName("archive");
	testDirectory.setFile("test.tar.gz$");
	testFileSystem(testDirectory);
}
#endif

TEST_CASE("FileUtilsTest_testFS", "[FileUtilsTest]") {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testFS");
	Util::FileName testDirectory = tempDir.getPath();
	testFileSystem(testDirectory);
}

#ifdef UTIL_HAVE_LIB_SQLITE
TEST_CASE("FileUtilsTest_testDBFS", "[FileUtilsTest]") {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testDBFS");
	Util::FileName testDirectory = tempDir.getPath();
	testDirectory.setFSName("dbfs");
	testDirectory.setDir(testDirectory.getDir() + "test.dbfs$");
	testFileSystem(testDirectory);
}
#endif

#ifdef UTIL_HAVE_LIB_ZIP
TEST_CASE("FileUtilsTest_testZIP", "[FileUtilsTest]") {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testZIP");
	Util::FileName testDirectory = tempDir.getPath();
	testDirectory.setFSName("zip");
	testDirectory.setFile("test.zip$");
	testFileSystem(testDirectory);
}
#endif
