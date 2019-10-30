/*
	This file is part of the Util library.
	Copyright (C) 2007-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace Util {
//! @addtogroup util_helper
//! @{
namespace Utils {	

/**
 * Get the resident set size/working set size of the current process.
 *
 * @return Memory size in bytes or 0 if this function is not supported on this platform.
 * @see proc(5)
 * @see http://msdn.microsoft.com/en-us/library/ms684877(VS.85).aspx
 */
size_t getResidentSetMemorySize();

/**
 * Get the virtual memory size/pagefile space of the current process.
 *
 * @return Memory size in bytes or 0 if this function is not supported on this platform.
 * @see proc(5)
 * @see http://msdn.microsoft.com/en-us/library/ms684877(VS.85).aspx
 */
size_t getVirtualMemorySize();

/**
 * Get the dynamic memory allocation size of the current process.
 *
 * @return Memory size in bytes or 0 if this function is not supported on this platform.
 * @see mallinfo(3)
 */
size_t getAllocatedMemorySize();

/**
 * Output memory usage information to standard output.
 */
void outputProcessMemory();

/**
 * Get the number of bytes that were read by this process.
 *
 * @return Number of bytes or 0 if this function is not supported on this platform.
 * @see Documentation/filesystems/proc.txt
 * @see http://msdn.microsoft.com/en-us/library/ms683218(VS.85).aspx
 */
size_t getIOBytesRead();

/**
 * Get the number of bytes that were written by this process.
 *
 * @return Number of bytes or 0 if this function is not supported on this platform.
 * @see Documentation/filesystems/proc.txt
 * @see http://msdn.microsoft.com/en-us/library/ms683218(VS.85).aspx
 */
size_t getIOBytesWritten();

/**
 * Output IO usage information to standard output.
 */
void outputProcessIO();

/**
 * Put the thread to sleep for the specified number of milliseconds.
 *
 * @param ms Number of milliseconds.
 * @see CLOCK_NANOSLEEP(2)
 * @see http://msdn.microsoft.com/en-us/library/ms686298(VS.85).aspx
 */
void sleep(unsigned long int ms);

/**
 * Return the identifier of the calling process.
 *
 * @return Process identifier specific to the running operating system
 * or -1 if the function is not implemented for the running operating system.
 */
int32_t getProcessId();

/**
 * Return the path of the currently running executable.
 *
 * @return Absolute path of the executable or an empty string, if the path
 * could not be retrieved.
 */
std::string getExecutablePath();

/**
 * Return the overall CPU usage of the system.
 *
 * @param timespan Timespan in milliseconds to monitor the CPU usage for
 * @return Overall CPU usage from [0, 1] independent of the number of CPUs, or
 * -1.0 if the function is not implemented for the running operating system.
 * @note The function might block for at least @p timespan milliseconds.
 */
double getCPUUsage(unsigned long int timespan);

/**
 * Get a backtrace for the calling function.
 * 
 * @return Series of function calls of the program. The most recent function
 * call is at the beginning.
 * @note The function getBacktrace() itself may be part of the backtrace.
 * @note If the needed functionality is not available on your system, a warning
 * message will be emitted and an empty array will be returned.
 */
std::vector<std::string> getBacktrace();

/**
 * Return a string containing a time stamp. It contains the current date, the
 * current time, and a time zone identifier. For example, the string could be
 * "2013-03-21_16-16-45_CET".
 * 
 * @return String containing a time stamp
 */
std::string createTimeStamp();

}

// experimental
class DebugOutput{
	public:
		std::ostream * out;
		DebugOutput(std::ostream * s=nullptr):out(s){
		}
		void setStream(std::ostream *s){
			out=s;
		}
		template<typename _t>
		DebugOutput & operator<<(const _t & p){
			if(out) (*out)<<p;
			return *this;
		}

};

extern DebugOutput info;

void enableInfo();
void disableInfo();

uint32_t calcHash(const uint8_t * ptr,size_t size);

std::string md5(const std::string& str);

//    void useStdInfo();
//    void useInternalInfo();
//    std::string getInfo();
//    void clearInfo();

//! @}
}
#endif // UTILS_H_INCLUDED
