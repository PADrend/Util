/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef PROGRESSINDICATOR_H_
#define PROGRESSINDICATOR_H_

#include <string>
#include <cstdint>

namespace Util {

/**
 * Class for printing progress state to console
 *
 * @author Paul Justus, Benjamin Eikel
 * @date 2010-06-08
 * @ingroup util_helper
 */
class ProgressIndicator {
		std::string textOutput; /*!< Text that precedes the progress in the output */
		uint32_t stepsTotal; /*!< Total number of steps */
		uint32_t stepsFinished; /*!< Number of steps that have been finished */
		double interval; /*!< Interval between consecutive outputs in seconds */

	public:
		/**
		 * Create a ProgressIndicator.
		 *
		 * @param description Description of the progress that is output together with the progress state
		 * @param totalSteps Total number of steps during the progress
		 * @param updateInterval Duration in seconds indicates between output of progress state
		 */
		UTILAPI ProgressIndicator(std::string description, uint32_t totalSteps, double updateInterval);

		/**
		 * Increment the number of finished steps by one.
		 * Print the progress state if the elapsed time since the last update is larger than the defined interval.
		 */
		UTILAPI void increment();

		bool operator==(const ProgressIndicator & other) const {
			return stepsTotal == other.stepsTotal && stepsFinished == other.stepsFinished && interval == other.interval && textOutput == other.textOutput;
		}
};

}

#endif // PROGRESSINDICATOR_H_
