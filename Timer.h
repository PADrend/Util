/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_HRTIMER_H
#define UTIL_HRTIMER_H

#include <cstdint>

#if defined(_WIN32)
#include <windows.h>
#else
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#endif

namespace Util {
/**
 * @brief High resolution timer
 *
 * Class to measure time intervals.
 * The implementation tries to uses a system timer with high resolution.
 * The system timer that is used depends on the platform.
 * @note Usage example for measuring a single time interval:
 * @code
 * Timer timer;
 * timer.reset();
 * doSomething();
 * timer.stop();
 * std::cout << "doSomething() took " << timer.getSeconds() << " s." << std::endl;
 * @endcode
 * @note Usage example for measuring multiple time intervals:
 * @code
 * Timer timer;
 * timer.reset();
 * timer.stop();
 * [...]
 * timer.resume();
 * doPartA();
 * timer.stop();
 * [...]
 * timer.resume();
 * doPartB();
 * timer.stop();
 * std::cout << "doPartA() and doPartB() took " << timer.getSeconds() << " s." << std::endl;
 * @endcode
 */
class Timer {
	private:
		static Timer processTimer;
	public:
		//! Returns the seconds elapsed since program start.
		static double now();
		// -----

		Timer();
		/**
		 * Reset the timer to the current time.
		 * The timer will be running after the call.
		 */
		void reset();
		/**
		 * Stop the timer.
		 * The current time will be stored to be able to resume later on.
		 */
		void stop();
		/**
		 * Start the timer again.
		 * This will resume the duration from the point that @a stop() was called last.
		 */
		void resume();

		/**
		 * @return @c true iff the timer is currently running
		 */
		bool isRunning() const {
			return running;
		}

		/**
		 * Return the current time.
		 *
		 * @return Time in seconds
		 */
		double getSeconds() const {
			return getNanoseconds() * 1.0e-9;
		}
		/**
		 * Return the current time.
		 *
		 * @return Time in milliseconds (1 s = 1e3 ms)
		 */
		double getMilliseconds() const {
			return getNanoseconds() * 1.0e-6;
		}

		/**
		 * Return the current time.
		 *
		 * @return Time in microseconds (1 s = 1e6 µs)
		 */
		double getMicroseconds() const {
			return getNanoseconds() * 1.0e-3;
		}
		/**
		 * Return the current time.
		 *
		 * @return Time in nanoseconds (1 s = 1e9 ns)
		 */
		uint64_t getNanoseconds() const;

		bool operator==(const Timer & other) const {
			return (&other == this) || (other.running == running && other.getNanoseconds() == getNanoseconds());
		}

	private:


#if defined(_WIN32)
		typedef LARGE_INTEGER timer_t;
		static timer_t frequency;
		static bool initDone;
#elif (defined(__USE_POSIX199309) && defined(_POSIX_MONOTONIC_CLOCK)) || defined(ANDROID)
		typedef timespec timer_t;
#else
		typedef timeval timer_t;
#endif

		uint64_t startTime;
		uint64_t stopTime;

		timer_t lastReset;

		bool running;

		/**
		 * Return the elapsed time since the last reset of this timer.
		 * This function has a system-specific implementation.
		 *
		 * @return Time in nanoseconds (1 s = 1e9 ns)
		 */
		uint64_t getNanosecondsSinceReset() const {
			timer_t time;
			timer_t result;
			queryTime(&time);
#ifdef _WIN32
			result.QuadPart = time.QuadPart - lastReset.QuadPart;
			return static_cast<uint64_t>(static_cast<double>(result.QuadPart) / static_cast<double>(frequency.QuadPart) * 1e9);
#elif (defined(__USE_POSIX199309) && defined(_POSIX_MONOTONIC_CLOCK)) || defined(ANDROID)
			result.tv_sec = time.tv_sec - lastReset.tv_sec;
			result.tv_nsec = time.tv_nsec - lastReset.tv_nsec;
			if (result.tv_nsec < 0) {
				--result.tv_sec;
				result.tv_nsec += 1000000000L;
			}
			return static_cast<uint64_t>(result.tv_nsec) + 1000000000L * static_cast<uint64_t>(result.tv_sec);
#else
			timersub(&time, &lastReset, &result);
			return 1000L * static_cast<uint64_t>(result.tv_usec) + 1000000000L * static_cast<uint64_t>(result.tv_sec);
#endif
		}


		/**
		 * Query the current time using a system call.
		 * This function has a system-specific implementation.
		 *
		 * @param time Structure that will be filled with the time value.
		 */
		static void queryTime(timer_t * time) {
#if defined(_WIN32)
			QueryPerformanceCounter(time);
#elif (defined(__USE_POSIX199309) && defined(_POSIX_MONOTONIC_CLOCK)) || defined(ANDROID)
			clock_gettime(CLOCK_MONOTONIC, time);
#else
			gettimeofday(time, nullptr);
#endif
		}
};
}
#endif // UTIL_HRTIMER_H
