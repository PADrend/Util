/*
	This file is part of the Util library.
	Copyright (C) 2020-2022 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_DATA_STREAM_H_
#define UTIL_DATA_STREAM_H_

#include "Macros.h"

#include <cstdint>
#include <vector>
#include <list>

namespace Util {

/**
 * @brief Generic data stream for arbitrary data.
 * Allows reading/writing arbitrary packed data from/to memory without relocation.
 * The data is stored in a list of non-continuous blocks of fixed size in memory.
 */
class DataStream {
public:
	DataStream(uint64_t blockSize) : blockSize(blockSize) {}
	DataStream() : blockSize(1024) {}

	UTILAPI void clear();
	UTILAPI void reset();
	UTILAPI bool read(uint8_t* data, uint64_t size);
	UTILAPI void write(const uint8_t* data, uint64_t size);

	template<typename T>
	T read() {
		T value;
		read(reinterpret_cast<uint8_t*>(&value), sizeof(T));
		return value;
	}

	template<typename T>
	T* readPtr() {
		return reinterpret_cast<T*>(readPtr(sizeof(T)));
	}

	template<typename T>
	DataStream& operator >> (T& value) {
		read(reinterpret_cast<uint8_t*>(&value), sizeof(value));
		return *this;
	}

	template<typename T>
	void write(const T& value) {
		write(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
	}

	template<typename T>
	DataStream& operator << (const T& value) {
		write(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
		return *this;
	}

	bool endOfStream() const { return blocks.empty() || readIter == blocks.end(); }
private:
	struct BlockAllocation {
		std::vector<uint8_t> data;
		uint64_t readPos;
		uint64_t writePos;
	};
	const uint64_t blockSize;
	void allocate(size_t size);
	UTILAPI uint8_t* readPtr(uint64_t size);

	std::list<BlockAllocation> blocks;
	std::list<BlockAllocation>::iterator readIter;
	std::list<BlockAllocation>::iterator writeIter;
};

} /* Util */

#endif /* end of include guard: UTIL_DATA_STREAM_H_ */