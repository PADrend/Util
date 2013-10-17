/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "ProgressIndicator.h"
#include "Timer.h"
#include <iostream>

namespace Util {

ProgressIndicator::ProgressIndicator(std::string description, uint32_t totalSteps, double updateInterval) :
	textOutput(description), stepsTotal(totalSteps), stepsFinished(0), interval(updateInterval) {
	// print progress state the first time
	std::cout << textOutput << ":   0.0%";
	std::cout.flush();
}

void ProgressIndicator::increment() {
	static Timer timer;

	++stepsFinished;

	if (stepsFinished == stepsTotal) {
		// all steps have been executed
		std::cout << '\r' << textOutput << ": done  " << std::endl;
		return;
	}

	if (timer.getSeconds() >= interval) {
		timer.reset();
		std::cout << '\r' << textOutput << ": ";

		// save the stream format and set the temporary new format
		std::streamsize oldPrecision = std::cout.precision(1);
		std::streamsize oldWidth = std::cout.width(5);
		char oldFill = std::cout.fill(' ');
		const std::ios_base::fmtflags oldFlags = std::cout.flags();
		std::cout.setf(std::ios_base::right, std::ios_base::adjustfield);
		std::cout.setf(std::ios_base::dec, std::ios_base::basefield);
		std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);

		const double percentFinished = static_cast<double> (stepsFinished) / static_cast<double> (stepsTotal) * 100.0;
		std::cout << percentFinished;

		// restore the stream format
		std::cout.precision(oldPrecision);
		std::cout.width(oldWidth);
		std::cout.fill(oldFill);
		std::cout.flags(oldFlags);

		std::cout << "%";
		std::cout.flush();
	}
}

}
