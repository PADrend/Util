/*
	This file is part of the Util library.
	Copyright (C) 2011 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "TimerTest.h"
#include "Timer.h"
#include "Utils.h"
CPPUNIT_TEST_SUITE_REGISTRATION(TimerTest);

void TimerTest::test() {
	Util::Timer timer;
	Util::Timer resumeTimer;
	resumeTimer.stop();
	uint64_t nsSum = 0;

	for (uint_fast8_t run = 0; run < 100; ++run) {
		timer.reset();
		resumeTimer.resume();
		Util::Utils::sleep(100ul);
		timer.stop();
		resumeTimer.stop();
		const uint64_t ns = timer.getNanoseconds();
		CPPUNIT_ASSERT(ns > 99000000 && ns < 103000000);
		const double us = timer.getMicroseconds();
		CPPUNIT_ASSERT_DOUBLES_EQUAL(us, 100000.0, 3000.0);
		const double ms = timer.getMilliseconds();
		CPPUNIT_ASSERT_DOUBLES_EQUAL(ms, 100.0, 3.0);
		const double s = timer.getSeconds();
		CPPUNIT_ASSERT_DOUBLES_EQUAL(s, 0.1, 0.003);

		nsSum += ns;
	}

	uint64_t nsDiff;
	const uint64_t nsResume = resumeTimer.getNanoseconds();
	if (nsSum > nsResume) {
		nsDiff = nsSum - nsResume;
	} else {
		nsDiff = nsResume - nsSum;
	}
	CPPUNIT_ASSERT(nsDiff < 20000);
}
