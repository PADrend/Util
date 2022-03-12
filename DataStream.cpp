/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "DataStream.h"

#include <cstring>

namespace Util {

//----------------------------

void DataStream::clear() {
	blocks.clear();
	readIter = blocks.end();
	writeIter = blocks.end();
}

//----------------------------

void DataStream::reset() {
	for(auto& block : blocks) {
		block.readPos = 0;
		block.writePos = 0;
	}
	readIter = blocks.begin();
	writeIter = blocks.begin();
}

//----------------------------

uint8_t* DataStream::readPtr(uint64_t size) {
	if(size == 0 || endOfStream())
		return nullptr;
	WARN_AND_RETURN_IF(size > blockSize, "DataStream: Invalid size for reading! size exceeds blockSize.", nullptr);
	if(readIter->readPos + size > readIter->writePos) {
		++readIter;
		if(readIter == blocks.end())
			return nullptr;
		readIter->readPos = 0;
	}

	uint8_t* ptr = &readIter->data[readIter->readPos];
	readIter->readPos += size;

	if(readIter->readPos >= readIter->writePos) {
		++readIter;
		if(readIter != blocks.end())
			readIter->readPos = 0;
	}

	return ptr;
}

//----------------------------

bool DataStream::read(uint8_t* data, uint64_t size) {
	if(!data)
		return false;
	const uint8_t* ptr = readPtr(size);
	if(!ptr)
		return false;
	std::memcpy(data, ptr, size);
	return true;
}

//----------------------------

void DataStream::write(const uint8_t* data, uint64_t size) {
	if(size == 0 || data == nullptr)
		return;
	WARN_AND_RETURN_IF(size > blockSize, "DataStream: Invalid size for writing! size exceeds blockSize.",);
	allocate(size);
	if(writeIter->writePos + size > writeIter->data.size()) {
		++writeIter;
		writeIter->writePos = 0;
	}

	std::memcpy(&writeIter->data[writeIter->writePos], data, size);
	writeIter->writePos += size;
}

//----------------------------

void DataStream::allocate(size_t size) {
	if(blocks.empty() || writeIter->data.size() - writeIter->writePos < size) {
		blocks.emplace_back(BlockAllocation{std::vector<uint8_t>(blockSize), 0, 0});
		writeIter = blocks.end();
		--writeIter;

		if(blocks.size() == 1)
			readIter = blocks.begin();
	}
}

//----------------------------


} /* Util */
