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
#include <limits>

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

//-------------

template<typename T>
inline T align(T offset, T alignment) {
	return alignment > 1 ? (offset + (alignment - offset % alignment) % alignment) : offset;
}

//-------------

template<typename T>
inline T clamp(T value, T min, T max) { return std::min(max, std::max(min, value)); }

//-------------

template<typename T>
inline double normalizeUnsigned(T value) { return static_cast<double>(value)/std::numeric_limits<T>::max(); }
template<>
inline double normalizeUnsigned(float value) { return clamp(value, 0.0f, 1.0f); }
template<>
inline double normalizeUnsigned(double value) { return clamp(value, 0.0, 1.0); }

//-------------

template<typename T>
inline T unnormalizeUnsigned(double value) { return static_cast<T>(clamp(value, 0.0, 1.0) * std::numeric_limits<T>::max()); }
template<>
inline float unnormalizeUnsigned(double value) { return clamp(value, 0.0, 1.0); }
template<>
inline double unnormalizeUnsigned(double value) { return clamp(value, 0.0, 1.0); }

//-------------

template<typename T>
inline double normalizeSigned(T value) { return std::max(static_cast<double>(value)/std::numeric_limits<T>::max(), -1.0); }
template<>
inline double normalizeSigned(float value) { return clamp(value, -1.0f, 1.0f); }
template<>
inline double normalizeSigned(double value) { return clamp(value, -1.0, 1.0); }

//-------------

template<typename T>
inline T unnormalizeSigned(double value) { return static_cast<T>(clamp(value, -1.0, 1.0) * std::numeric_limits<T>::max()); }
template<>
inline float unnormalizeSigned(double value) { return clamp(value, -1.0, 1.0); }
template<>
inline double unnormalizeSigned(double value) { return clamp(value, -1.0, 1.0); }

//-----------------------------------------------------------------------------------
// Hashing

uint32_t calcHash(const uint8_t * ptr,size_t size);

std::string md5(const std::string& str);

template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

inline void hash_param(size_t &seed) {}

template <typename T>
inline void hash_param(size_t &seed, const T &value) {
	hash_combine(seed, value);
}

template <typename T, typename... Args>
inline void hash_param(size_t& seed, const T& first_arg, const Args& ...args) {
	hash_combine(seed, first_arg);
	hash_param(seed, args...);
}

// FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
// str should be a null terminated string literal, value should be left out 
// e.g hash32("example")
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/
constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

inline constexpr uint32_t hash32(const char* const str, const uint32_t value = val_32_const) noexcept {
	return (str[0] == '\0') ? value : hash32(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
}

inline constexpr uint64_t hash64(const char* const str, const uint64_t value = val_64_const) noexcept {
	return (str[0] == '\0') ? value : hash64(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
}

//    void useStdInfo();
//    void useInternalInfo();
//    std::string getInfo();
//    void clearInfo();

//! @}
}
#endif // UTILS_H_INCLUDED
