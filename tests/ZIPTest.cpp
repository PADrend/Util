/*
	This file is part of the Util library.
	Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "ZIPTest.h"
#include "IO/FileName.h"
#include "IO/FileUtils.h"
#include "StringUtils.h"
#include <cppunit/TestAssert.h>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <vector>
CPPUNIT_TEST_SUITE_REGISTRATION(ZIPTest);

using namespace CppUnit;

#ifdef UTIL_HAVE_LIB_ZIP
static const std::string zipArchive("zip://test.zip$");

static void threadWriterFun(const char thread) {
	for(uint32_t i = 0; i < 1000; ++i) {
		const std::string numString = Util::StringUtils::toString<uint32_t>(i);
		const Util::FileName testFile(zipArchive + "thread" + thread + numString + ".txt");
		const std::string testString(std::string("Test by Thread ") + thread + ".\n");
		Util::FileUtils::saveFile(testFile, std::vector<uint8_t>(testString.begin(), testString.end()));
	}
}

static void threadReaderFun() {
	for(uint32_t i = 0; i < 1000; ++i) {
		const std::string numString = Util::StringUtils::toString<uint32_t>(i);
		for(char thread = 'E'; thread >= 'A'; --thread) {
			const Util::FileName testFile(zipArchive + "thread" + thread + numString + ".txt");
			while(!Util::FileUtils::isFile(testFile)) {
				// Wait for file to be written.
			}
			// Check if the file has been written correctly.
			const std::vector<uint8_t> data = Util::FileUtils::loadFile(testFile);
			CPPUNIT_ASSERT(!data.empty());
			const std::string testString(std::string("Test by Thread ") + thread + ".\n");
			CPPUNIT_ASSERT(data == std::vector<uint8_t>(testString.begin(), testString.end()));
		}
	}
}
#endif

void ZIPTest::test() {
#ifdef UTIL_HAVE_LIB_ZIP
	{
		const std::string one = Util::StringUtils::toString<uint32_t>(1);
		const std::string two = Util::StringUtils::toString<uint32_t>(2);
		std::string twelve = one + two;
	}
	
	// Test threaded writing to and reading from a ZIP archive.
	static const Util::FileName zipFile("file://test.zip");
	
	if(Util::FileUtils::isFile(zipFile)) {
		Util::FileUtils::remove(zipFile);
	}

	std::thread threadWriterA(threadWriterFun, 'A');
	std::thread threadWriterB(threadWriterFun, 'B');
	
	std::thread threadReaderA(threadReaderFun);
	
	std::thread threadWriterC(threadWriterFun, 'C');
	std::thread threadWriterD(threadWriterFun, 'D');
	
	std::thread threadReaderB(threadReaderFun);
	
	std::thread threadWriterE(threadWriterFun, 'E');
	
	std::thread threadReaderC(threadReaderFun);

	threadWriterA.join();
	threadWriterB.join();
	
	threadReaderA.join();
	
	threadWriterC.join();
	threadWriterD.join();
	
	threadReaderB.join();
	
	threadWriterE.join();
	
	threadReaderC.join();

	Util::FileUtils::remove(zipFile);
#else
	// No support for ZIP, nothing to test.
#endif
}
