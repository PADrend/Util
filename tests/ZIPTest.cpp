/*
	This file is part of the Util library.
	Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "ZIPTest.h"
#include <Util/Concurrency/Concurrency.h>
#include <Util/Concurrency/Thread.h>
#include <Util/IO/FileName.h>
#include <Util/IO/FileUtils.h>
#include <Util/StringUtils.h>
#include <cppunit/TestAssert.h>
#include <cstdint>
#include <cstdlib>
#include <vector>
CPPUNIT_TEST_SUITE_REGISTRATION(ZIPTest);

using namespace Util::Concurrency;
using namespace CppUnit;

static const std::string zipArchive("zip://test.zip$");

static void * threadWriterFun(void * data) {
	const char thread = *reinterpret_cast<const char *>(data);
	for(uint32_t i = 0; i < 1000; ++i) {
		const std::string numString = Util::StringUtils::toString<uint32_t>(i);
		const Util::FileName testFile(zipArchive + "thread" + thread + numString + ".txt");
		const std::string testString(std::string("Test by Thread ") + thread + ".\n");
		Util::FileUtils::saveFile(testFile, std::vector<uint8_t>(testString.begin(), testString.end()));
	}
	return nullptr;
}

static void * threadReaderFun(void * /*arg*/) {
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
	return nullptr;
}

void ZIPTest::test() {
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
	
	std::unique_ptr<Thread> threadWriterA(createThread());
	std::unique_ptr<Thread> threadWriterB(createThread());
	
	std::unique_ptr<Thread> threadReaderA(createThread());
	
	std::unique_ptr<Thread> threadWriterC(createThread());
	std::unique_ptr<Thread> threadWriterD(createThread());
	
	std::unique_ptr<Thread> threadReaderB(createThread());
	std::unique_ptr<Thread> threadWriterE(createThread());
	
	std::unique_ptr<Thread> threadReaderC(createThread());

	char threadAChar = 'A';
	threadWriterA->start(threadWriterFun, reinterpret_cast<void *>(&threadAChar));
	char threadBChar = 'B';
	threadWriterB->start(threadWriterFun, reinterpret_cast<void *>(&threadBChar));
	
	threadReaderA->start(threadReaderFun, nullptr);
	
	char threadCChar = 'C';
	threadWriterC->start(threadWriterFun, reinterpret_cast<void *>(&threadCChar));
	char threadDChar = 'D';
	threadWriterD->start(threadWriterFun, reinterpret_cast<void *>(&threadDChar));
	
	threadReaderB->start(threadReaderFun, nullptr);
	
	char threadEChar = 'E';
	threadWriterE->start(threadWriterFun, reinterpret_cast<void *>(&threadEChar));
	
	threadReaderC->start(threadReaderFun, nullptr);

	threadWriterA->join();
	threadWriterB->join();
	
	threadReaderA->join();
	
	threadWriterC->join();
	threadWriterD->join();
	
	threadReaderB->join();
	
	threadWriterE->join();
	
	threadReaderC->join();

	threadWriterA.reset();
	threadWriterB.reset();
	
	threadReaderA.reset();
	
	threadWriterC.reset();
	threadWriterD.reset();
	
	threadReaderB.reset();
	
	threadWriterE.reset();
	
	threadReaderC.reset();
	
	Util::FileUtils::remove(zipFile);
}
