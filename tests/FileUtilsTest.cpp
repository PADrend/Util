/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "FileUtilsTest.h"
#include <Util/IO/FileName.h>
#include <Util/IO/FileUtils.h>
#include <Util/IO/TemporaryDirectory.h>
#include <Util/StringUtils.h>
#include <Util/Utils.h>
#include <cppunit/TestAssert.h>
CPPUNIT_TEST_SUITE_REGISTRATION(FileUtilsTest);

using namespace CppUnit;

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
		std::unique_ptr<std::ostream> outs(FileUtils::openForWriting(filename));
		CPPUNIT_ASSERT(outs.get() != nullptr);
		*outs << s;
		CPPUNIT_ASSERT(outs->good());
	}

	{   // fileSize
		size_t s1=FileUtils::fileSize(filename);
		CPPUNIT_ASSERT_EQUAL(s.size(), s1);
	}

	{   // isFile
		bool b1=FileUtils::isFile(filename);
		CPPUNIT_ASSERT_EQUAL(true, b1);
		bool b2=FileUtils::isFile(FileName(filename.toString()+"foo"));
		CPPUNIT_ASSERT_EQUAL(false, b2);
	}

	{   // dir (very basic check)
		std::list<FileName> files;
		FileUtils::dir(directory, files, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
		CPPUNIT_ASSERT(files.size() == 1);
	}

	{ // openForReading
		std::unique_ptr<std::istream> in(FileUtils::openForReading(filename));
		CPPUNIT_ASSERT(in.get() != nullptr);

		std::string s2;
		while(in->good()){
			char c = static_cast<char> (in->get());
			if(in->good())
				s2+=c;
		}
		CPPUNIT_ASSERT_EQUAL(s, s2);
	}

	{ // openForAppending
		std::unique_ptr<std::ostream> out(FileUtils::openForAppending(filename));
		CPPUNIT_ASSERT(out.get() != nullptr);

		std::string app="Appended:Foo";
		*out << app;
		s+=app;
		out.reset();
		const std::string data=FileUtils::getFileContents(filename);
		CPPUNIT_ASSERT(!data.empty());
		CPPUNIT_ASSERT_EQUAL(s, data);
	}

	{ // open
		std::unique_ptr<std::iostream> io(FileUtils::open(filename));
		CPPUNIT_ASSERT(io.get() != nullptr);

		char c = static_cast<char> (io->get());
		io->seekp(0);
		*io << c << c << c;
		*io << ":-)";

		s.replace(0,6,"HHH:-)");
		io.reset();
		const std::string data=FileUtils::getFileContents(filename);
		CPPUNIT_ASSERT(!data.empty());
		CPPUNIT_ASSERT_EQUAL(s, data);
	}
	s+="\nTest2";

	{   // saveFile
		bool b1=FileUtils::saveFile(filename,std::vector<uint8_t>(s.begin(), s.end()),true);
		CPPUNIT_ASSERT_EQUAL(true, b1);
		bool b2=FileUtils::saveFile(filename,std::vector<uint8_t>(s.begin(), s.end()),false);
		CPPUNIT_ASSERT_EQUAL(false, b2);
	}

	{   // loadFile
		const std::vector<uint8_t> data=FileUtils::loadFile(filename);
		CPPUNIT_ASSERT(!data.empty());
		CPPUNIT_ASSERT_EQUAL(s, std::string(data.begin(), data.end()));
	}

	// File remove (dbfs does not support remove function)
	if(directory.getFSName() != "dbfs") {
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isFile(filename));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::remove(filename));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isFile(filename));
	}

	// Directory operations (dbfs does not support createDir, remove functions)
	if(directory.getFSName() != "dbfs") {
		const Util::FileName missingDir(directory.toString() + "missing/");
		const Util::FileName flatDir(directory.toString() + "directory/");
		const Util::FileName deepDir1(flatDir.toString() + "subdirectory/");
		const Util::FileName deepDir2(deepDir1.toString() + "subdirectory/");
		const Util::FileName deepDir3(deepDir2.toString() + "subdirectory/");

		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(missingDir));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(flatDir));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir1));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir2));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir3));

		CPPUNIT_ASSERT_EQUAL(true, FileUtils::createDir(flatDir));

		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(missingDir));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(flatDir));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir1));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir2));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir3));

		CPPUNIT_ASSERT_EQUAL(true, FileUtils::createDir(deepDir3));

		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(missingDir));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(flatDir));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(deepDir1));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(deepDir2));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(deepDir3));

		CPPUNIT_ASSERT_EQUAL(true, FileUtils::remove(deepDir3));

		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(missingDir));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(flatDir));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(deepDir1));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(deepDir2));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir3));

		CPPUNIT_ASSERT_EQUAL(true, FileUtils::remove(deepDir2));

		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(missingDir));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(flatDir));
		CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(deepDir1));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir2));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir3));

		CPPUNIT_ASSERT_EQUAL(true, FileUtils::remove(flatDir, true));

		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(missingDir));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(flatDir));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir1));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir2));
		CPPUNIT_ASSERT_EQUAL(false, FileUtils::isDir(deepDir3));
	}

	// dir with different parameters (dbfs lacks support for various functions)
	if(directory.getFSName() != "dbfs") {
		{
			std::list<FileName> files;
			FileUtils::dir(directory, files, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
			CPPUNIT_ASSERT(files.empty());
		}

		for(uint_fast16_t dirLevel0 = 0; dirLevel0 < 5; ++dirLevel0) {
			const FileName dirLevel0FileName(directory.toString() + Util::StringUtils::toString(dirLevel0) + '/');
			CPPUNIT_ASSERT_EQUAL(true, FileUtils::createDir(dirLevel0FileName));
			for(uint_fast16_t dirLevel1 = 0; dirLevel1 < 4; ++dirLevel1) {
				const FileName dirLevel1FileName(dirLevel0FileName.toString() + Util::StringUtils::toString(dirLevel1) + '/');
				CPPUNIT_ASSERT_EQUAL(true, FileUtils::createDir(dirLevel1FileName));
				for(uint_fast16_t dirLevel2 = 0; dirLevel2 < 3; ++dirLevel2) {
					const FileName dirLevel2FileName(dirLevel1FileName.toString() + Util::StringUtils::toString(dirLevel2) + '/');
					CPPUNIT_ASSERT_EQUAL(true, FileUtils::createDir(dirLevel2FileName));
					for(uint_fast16_t fileNumber = 0; fileNumber < 2; ++fileNumber) {
						CPPUNIT_ASSERT_EQUAL(true,
											 FileUtils::saveFile(FileName(dirLevel2FileName.toString() + Util::StringUtils::toString(fileNumber)),
																 std::vector<uint8_t>(2),
																 true));
					}
					CPPUNIT_ASSERT_EQUAL(true,
										 FileUtils::saveFile(FileName(dirLevel2FileName.toString() + ".hidden"),
															 std::vector<uint8_t>(2),
															 true));
				}
				CPPUNIT_ASSERT_EQUAL(true,
									 FileUtils::saveFile(FileName(dirLevel1FileName.toString() + "level1File"),
														 std::vector<uint8_t>(2),
														 true));
			}
		}

		// Recursive
		{
			std::list<FileName> allEntries;
			FileUtils::dir(directory, allEntries, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(285), allEntries.size());
		}
		{
			std::list<FileName> dirs;
			FileUtils::dir(directory, dirs, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_RECURSIVE);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(85), dirs.size());
		}
		{
			std::list<FileName> files;
			FileUtils::dir(directory, files, FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES | FileUtils::DIR_RECURSIVE);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(200), files.size());
		}
		{
			std::list<FileName> filesWithoutHidden;
			FileUtils::dir(directory, filesWithoutHidden, FileUtils::DIR_FILES | FileUtils::DIR_RECURSIVE);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(140), filesWithoutHidden.size());
		}
		// Non-recursive
		{
			std::list<FileName> topLevel;
			FileUtils::dir(directory, topLevel, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), topLevel.size());
		}
		{
			std::list<FileName> level0;
			FileUtils::dir(Util::FileName(directory.toString() + "4/"), level0, FileUtils::DIR_DIRECTORIES | FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), level0.size());
		}
		{
			const Util::FileName level1FileName(directory.toString() + "1/0/");
			std::list<FileName> level1Dirs;
			FileUtils::dir(level1FileName, level1Dirs, FileUtils::DIR_DIRECTORIES);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), level1Dirs.size());
			std::list<FileName> level1Files;
			FileUtils::dir(level1FileName, level1Files, FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), level1Files.size());
		}
		{
			const Util::FileName level2FileName(directory.toString() + "3/2/1/");
			std::list<FileName> level2Dirs;
			FileUtils::dir(level2FileName, level2Dirs, FileUtils::DIR_DIRECTORIES);
			CPPUNIT_ASSERT(level2Dirs.empty());
			std::list<FileName> level2Files;
			FileUtils::dir(level2FileName, level2Files, FileUtils::DIR_FILES | FileUtils::DIR_HIDDEN_FILES);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), level2Files.size());
			std::list<FileName> level2FilesWithoutHidden;
			FileUtils::dir(level2FileName, level2FilesWithoutHidden, FileUtils::DIR_FILES);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), level2FilesWithoutHidden.size());
		}

		// Benchmark for lots of isFile and isDir calls
		for(uint_fast16_t dirLevel0 = 0; dirLevel0 < 5; ++dirLevel0) {
			const FileName dirLevel0FileName(directory.toString() + Util::StringUtils::toString(dirLevel0) + '/');
			CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(dirLevel0FileName));
			for(uint_fast16_t dirLevel1 = 0; dirLevel1 < 4; ++dirLevel1) {
				const FileName dirLevel1FileName(dirLevel0FileName.toString() + Util::StringUtils::toString(dirLevel1) + '/');
				CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(dirLevel1FileName));
				for(uint_fast16_t dirLevel2 = 0; dirLevel2 < 3; ++dirLevel2) {
					const FileName dirLevel2FileName(dirLevel1FileName.toString() + Util::StringUtils::toString(dirLevel2) + '/');
					CPPUNIT_ASSERT_EQUAL(true, FileUtils::isDir(dirLevel2FileName));
					for(uint_fast16_t fileNumber = 0; fileNumber < 2; ++fileNumber) {
						CPPUNIT_ASSERT_EQUAL(true, FileUtils::isFile(FileName(dirLevel2FileName.toString() + Util::StringUtils::toString(fileNumber))));
					}
					CPPUNIT_ASSERT_EQUAL(true, FileUtils::isFile(FileName(dirLevel2FileName.toString() + ".hidden")));
				}
				CPPUNIT_ASSERT_EQUAL(true, FileUtils::isFile(FileName(dirLevel1FileName.toString() + "level1File")));
			}
		}
	}
}

#ifdef UTIL_HAVE_LIB_ARCHIVE
void FileUtilsTest::testARCHIVE() {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testARCHIVE");
	Util::FileName testDirectory = tempDir.getPath();
	testDirectory.setFSName("archive");
	testDirectory.setFile("test.tar.gz$");
	testFileSystem(testDirectory);
}
#endif

void FileUtilsTest::testFS() {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testFS");
	Util::FileName testDirectory = tempDir.getPath();
	testFileSystem(testDirectory);
}

#ifdef UTIL_HAVE_LIB_SQLITE
void FileUtilsTest::testDBFS() {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testDBFS");
	Util::FileName testDirectory = tempDir.getPath();
	testDirectory.setFSName("dbfs");
	testDirectory.setDir(testDirectory.getDir() + "test.dbfs$");
	testFileSystem(testDirectory);
}
#endif

#ifdef UTIL_HAVE_LIB_ZIP
void FileUtilsTest::testZIP() {
	Util::TemporaryDirectory tempDir("FileUtilsTest_testZIP");
	Util::FileName testDirectory = tempDir.getPath();
	testDirectory.setFSName("zip");
	testDirectory.setFile("test.zip$");
	testFileSystem(testDirectory);
}
#endif
