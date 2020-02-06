/*
	This file is part of the Util library.
	Copyright (C) 2007-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>

	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Utils.h"

#include "Macros.h"
#include "MicroXML.h"
#include "StringUtils.h"

#include <iomanip>
#include <iostream>
#include <cstdio>
#if defined(_WIN32) || defined(_WIN64)
#ifndef WINVER
#define WINVER 0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#endif
#include <algorithm>
#include <array>
#include <cstdint>
#include <ctime>
#include <deque>
#include <numeric>
#include <sstream>
#include <utility>
#include <cstring>

#ifdef UTIL_HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef UTIL_HAVE_MALLOC_H
#include <malloc.h>
#endif

namespace Util {
namespace Utils {

size_t getResidentSetMemorySize() {
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)); // Needs psapi-library!
	return pmc.WorkingSetSize;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	try {
		size_t sizeNumPages, residentNumPages;
		const long pageSize = sysconf(_SC_PAGESIZE);
		std::ifstream fs("/proc/self/statm");
		fs >> std::skipws >> sizeNumPages >> residentNumPages;
		return residentNumPages * pageSize;
	} catch (...) {
		return 0;
	}
#else
	return 0;
#endif
}

size_t getVirtualMemorySize() {
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)); // Needs psapi-library!
	return pmc.PagefileUsage;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	try {
		size_t sizeNumPages;
		const long pageSize = sysconf(_SC_PAGESIZE);
		std::ifstream fs("/proc/self/statm");
		fs >> std::skipws >> sizeNumPages;
		return sizeNumPages * pageSize;
	} catch (...) {
		return 0;
	}
#else
	return 0;
#endif
}

size_t getAllocatedMemorySize() {
#if defined(UTIL_HAVE_MALLOC_H) && defined(UTIL_HAVE_MALLOC_INFO) && defined(UTIL_HAVE_OPEN_MEMSTREAM)
	char * ptr;
	size_t size;
	auto memStream = open_memstream(&ptr, &size);
	if(memStream == nullptr) {
		WARN("open_memstream failed.");
		return 0;
	}
	malloc_info(0, memStream);
	fclose(memStream);
	std::istringstream stream(std::string(ptr, size));
	free(ptr);

	size_t allocatedSize = 0;
	bool insideHeapTag = false;
	MicroXML::Reader::traverse(
		stream,
		[&insideHeapTag, &allocatedSize](const std::string & tagName, 
										 const Util::MicroXML::attributes_t & attributes) {
			if(tagName == "malloc") {
				const auto versionIt = attributes.find("version");
				if(versionIt == attributes.cend()) {
					WARN("No malloc_info version found.");
					return false;
				} else if(versionIt->second != "1") {
					WARN("Unsupported malloc_info version found.");
					return false;
				}
			} else if(tagName == "heap") {
				if(insideHeapTag) {
					WARN("Nested heap tags found in malloc_info.");
					return false;
				}
				insideHeapTag = true;
			} else if(!insideHeapTag && tagName == "system") {
				const auto typeIt = attributes.find("type");
				if(typeIt == attributes.cend()) {
					WARN("No attribute type in tag system found in malloc_info.");
					return false;
				}
				if(typeIt->second == "current") {
					const auto sizeIt = attributes.find("size");
					if(typeIt == attributes.cend()) {
						WARN("No attribute size in tag system found in malloc_info.");
						return false;
					}
					allocatedSize = Util::StringUtils::toNumber<size_t>(sizeIt->second);
					return false;
				}
			}
			return true;
		},
		[&insideHeapTag](const std::string & tagName) {
			if(tagName == "heap") {
				if(!insideHeapTag) {
					WARN("Closing without opening heap tag found in malloc_info.");
					return false;
				}
				insideHeapTag = false;
			}
			return true;
		},
		[](const std::string &, const std::string &) {
			return true;
		}
	);
	return allocatedSize;
#elif defined(UTIL_HAVE_MALLOC_H) && defined(UTIL_HAVE_MALLINFO)
	const auto mallocInfo = mallinfo();
	return static_cast<size_t>(mallocInfo.hblkhd) + static_cast<size_t>(mallocInfo.uordblks);
#else
	return 0;
#endif
}

void outputProcessMemory() {
	const double Mebi = 1024.0 * 1024.0;
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "Memory:\tVirtual memory size =\t" << std::setw(8) << getVirtualMemorySize() / Mebi << " MiBytes\n";
	std::cout << "Memory:\tResident set size =  \t" << std::setw(8) << getResidentSetMemorySize() / Mebi << " MiBytes\n";
	std::cout << "Memory:\tAllocated memory size =  \t" << std::setw(8) << getAllocatedMemorySize() / Mebi << " MiBytes" << std::endl;
}

size_t getIOBytesRead() {
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hProcess = GetCurrentProcess();
	IO_COUNTERS pioc;
	GetProcessIoCounters(hProcess, &pioc);
	return pioc.ReadTransferCount;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	try {
		std::ifstream fs("/proc/self/io");
		char buffer[256];
		while(fs.good()) {
			fs.getline(buffer, 256, ':');
			if(std::string(buffer) == "rchar") {
				size_t bytesRead;
				fs >> std::skipws >> bytesRead;
				return bytesRead;
			}
			fs.ignore(256, '\n');
		}
		return 0;
	} catch (...) {
		return 0;
	}
#else
	return 0;
#endif
}

size_t getIOBytesWritten() {
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hProcess = GetCurrentProcess();
	IO_COUNTERS pioc;
	GetProcessIoCounters(hProcess, &pioc);
	return pioc.WriteTransferCount;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	try {
		std::ifstream fs("/proc/self/io");
		char buffer[256];
		while(fs.good()) {
			fs.getline(buffer, 256, ':');
			if(std::string(buffer) == "wchar") {
				size_t bytesRead;
				fs >> std::skipws >> bytesRead;
				return bytesRead;
			}
			fs.ignore(256, '\n');
		}
		return 0;
	} catch (...) {
		return 0;
	}
#else
	return 0;
#endif
}

void outputProcessIO() {
	const float Mebi = 1024.0f * 1024.0f;
	const float bytesRead = static_cast<float>(getIOBytesRead()) / Mebi;
	const float bytesWritten = static_cast<float>(getIOBytesWritten()) / Mebi;
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "IO:\t\tRead =\t" << std::setw(8) << bytesRead << " MiBytes\n";
	std::cout << "IO:\t\tWrite =  \t" << std::setw(8) << bytesWritten << " MiBytes" << std::endl;
}

void sleep(unsigned long int ms) {
#if defined(_WIN32) || defined(_WIN64)
	Sleep(ms);
#elif defined(__USE_XOPEN2K) && defined(_POSIX_MONOTONIC_CLOCK)
	timespec t;
	const unsigned long int numSeconds = ms / 1000ul;
	ms -= numSeconds * 1000ul;
	t.tv_sec = numSeconds;
	t.tv_nsec = 1000000ul * ms;
	clock_nanosleep(CLOCK_MONOTONIC, 0, &t, nullptr);
#elif defined(ANDROID)
	timespec t;
	const unsigned long int numSeconds = ms / 1000ul;
	ms -= numSeconds * 1000ul;
	t.tv_sec = numSeconds;
	t.tv_nsec = 1000000ul * ms;
	nanosleep(&t, nullptr);
#elif defined(__USE_XOPEN_EXTENDED) || defined(__APPLE__) || defined(__unix__)
	usleep(1000ul * ms);
#else
#error "Not implemented for your system.";
#endif
}

int32_t getProcessId() {
#if defined(_WIN32) || defined(_WIN64)
	const DWORD pid = GetCurrentProcessId();
	return static_cast<int32_t>(pid);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID) || defined(__APPLE__)
	const pid_t pid = getpid();
	return static_cast<int32_t>(pid);
#else
	WARN("Not implemented for your system.");
	return -1;
#endif
}

std::string getExecutablePath() {
#if defined(_WIN32) || defined(_WIN64)
	std::string buffer(MAX_PATH, '\0');
	const auto numChars = GetModuleFileName(nullptr, 
											&buffer.front(),
											buffer.size());
	if(numChars == 0) {
		WARN("GetModuleFileName failed.");
		return std::string();
	}
	buffer.resize(numChars);
	return buffer;
#elif defined(__linux__)
	std::string buffer(2048, '\0');
	const auto numChars = readlink("/proc/self/exe", 
								   &buffer.front(),
								   buffer.size());
	if(numChars == -1) {
		WARN("readlink failed.");
		return std::string();
	}
	buffer.resize(numChars);
    return buffer;
#elif defined(__APPLE__)
	uint32_t bufferSize = 0;
	_NSGetExecutablePath(nullptr, &bufferSize);
	std::string buffer(bufferSize, '\0');
	const auto result = _NSGetExecutablePath(&buffer.front(), &bufferSize);
	if(result != 0) {
		WARN("_NSGetExecutablePath failed.");
		return std::string();
	}
	return buffer;
#else
	WARN("Not implemented for your system.");
	return std::string();
#endif
}

#if defined(__linux__) || defined(__unix__)
/**
 * Read CPU data from "/proc/stat" and return the overall sum and the sum of
 * the first three values (user, nice, system).
 */
static std::pair<uint32_t, uint32_t> getCPUWorkAndOverall() {
	std::ifstream fs("/proc/stat");
	while(fs.good()) {
		std::string prefix;
		fs >> prefix;
		if(prefix == "cpu") {
			std::deque<double> values;
			while(fs.peek() != '\n') {
				uint32_t value;
				fs >> value;
				values.push_back(value);
			}
			const uint32_t work = values[0] + values[1] + values[2];
			const uint32_t sum = std::accumulate(values.begin(), values.end(), 0);
			return std::make_pair(work, sum);
		}
	}
	return std::make_pair(0, 0);
}
#endif

double getCPUUsage(unsigned long int timespan __attribute__((unused))) {
#if defined(__linux__) || defined(__unix__)
	const auto pairBefore = getCPUWorkAndOverall();
	sleep(timespan);
	const auto pairAfter = getCPUWorkAndOverall();
	const auto workDiff = pairAfter.first - pairBefore.first;
	const auto overallDiff = pairAfter.second - pairBefore.second;
    return static_cast<double>(workDiff) / static_cast<double>(overallDiff);
#else
	WARN("Not implemented for your system.");
	return -1.0;
#endif
}

std::vector<std::string> getBacktrace() {
	std::vector<std::string> result;
#ifdef UTIL_HAVE_EXECINFO_H
	std::array<void *, 128> buffer;
	const int num = backtrace(buffer.data(), buffer.size());
	result.reserve(static_cast<size_t>(num));
	char ** strings = backtrace_symbols(buffer.data(), num);
	for (int i = 0; i < num; ++i) {
		result.emplace_back(strings[i]);
	}
	free(strings);
#else
	WARN("Not implemented for your system.");
#endif
	return result;
}

std::string createTimeStamp() {
	const auto time = std::time(nullptr);
	std::string buffer(64, '\0');
	const auto numBytes = std::strftime(&buffer.front(), 
										buffer.size(),
										"%Y-%m-%d_%H-%M-%S",
										std::localtime(&time));
	buffer.resize(numBytes);      
	return buffer;
}

}

DebugOutput info;

void enableInfo()
{
	info.setStream(&std::cout);
}
void disableInfo()
{
	info.setStream(nullptr);
}

//
//void useStdInfo() {
//    info=&std::cout;
//}
//static std::ostringstream infoString;
//
//void useInternalInfo() {
//    info=&infoString;
//}
//
//std::string getInfo() {
//    return infoString.str();
//}
//void clearInfo() {
//    infoString.clear();
//}

}
