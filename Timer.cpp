/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Timer.h"
#include "Macros.h"

namespace Util {

/*!	(static) */
Timer Timer::processTimer;

/*!	(static) */
#ifdef _WIN32
bool Timer::initDone = false;
Timer::timer_t Timer::frequency;
#endif

/*!	(static) */
double Timer::now() {
	return processTimer.getSeconds();
}

// ---------------------------------------


Timer::Timer() :
	startTime(0), stopTime(0), running(true) {
#ifdef _WIN32
	if(!Timer::initDone) {
		if(!QueryPerformanceFrequency(&Timer::frequency)) {
			WARN("QueryPerformanceFrequency failed, timer will not work properly!");
		}
		Timer::initDone = true;
	}
#endif
	queryTime(&lastReset);
}

void Timer::reset() {
	running = true;
	queryTime(&lastReset);
	startTime = 0;
	stopTime = 0;
}

void Timer::stop() {
	if (!running) {
		return;
	}
	running = false;
	stopTime = getNanosecondsSinceReset();
}

void Timer::resume() {
	if (running) {
		return;
	}
	running = true;
	startTime += getNanosecondsSinceReset() - stopTime;
}

uint64_t Timer::getNanoseconds() const {
	if (running) {
		return getNanosecondsSinceReset() - startTime;
	} else {
		return stopTime - startTime;
	}
}

}
