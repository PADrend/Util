/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Encoding.h"
#include "Macros.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Util {

static const char * const base64Symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base64Padding = '=';

std::string encodeBase64(const std::vector<uint8_t> & source) {
	const size_t inputSize = source.size();
	const size_t outputSize = (inputSize + 2 - ((inputSize + 2) % 3)) * 4 / 3;

	const uint8_t * const input = source.data();
	std::string destination(outputSize, base64Padding);

	size_t inputCursor = 0;
	std::string::iterator outputCursor = destination.begin();

	const size_t completeSize = inputSize - (inputSize % 3);
	while (inputCursor < completeSize) {
		const uint32_t triplet = static_cast<uint32_t>(input[inputCursor + 2]) | (static_cast<uint32_t>(input[inputCursor + 1]) << 8) | (static_cast<uint32_t>(input[inputCursor]) << 16);
		inputCursor += 3;
		*outputCursor++ = base64Symbols[(triplet >> 18) & 0x3f];
		*outputCursor++ = base64Symbols[(triplet >> 12) & 0x3f];
		*outputCursor++ = base64Symbols[(triplet >> 6) & 0x3f];
		*outputCursor++ = base64Symbols[triplet & 0x3f];
	}
	switch (inputSize % 3) {
		case 1: {
			const uint32_t triplet = (static_cast<uint32_t>(input[inputCursor]) << 16);
			*outputCursor++ = base64Symbols[(triplet >> 18) & 0x3f];
			*outputCursor++ = base64Symbols[(triplet >> 12) & 0x3f];
			// Two padding characters at the end.
			break;
		}
		case 2: {
			const uint32_t triplet = (static_cast<uint32_t>(input[inputCursor + 1]) << 8) | (static_cast<uint32_t>(input[inputCursor]) << 16);
			*outputCursor++ = base64Symbols[(triplet >> 18) & 0x3f];
			*outputCursor++ = base64Symbols[(triplet >> 12) & 0x3f];
			*outputCursor++ = base64Symbols[(triplet >> 6) & 0x3f];
			// One padding character at the end.
			break;
		}
		default:
			break;
	}
	return destination;
}

//! (static)
std::vector<uint8_t> decodeBase64(const std::string & source) {
	static std::vector<uint8_t> symbolTable;
	if (symbolTable.empty()) {
		symbolTable.assign(256, 0xff);
		for (uint_fast8_t i = 0; i < 64; ++i) {
			const char c = base64Symbols[i];
			symbolTable[static_cast<uint8_t> (c)] = i;
		}
		symbolTable[static_cast<uint8_t>(base64Padding)] = 0xfe;
	}
	// count valid chars
	size_t numSymbols = 0;
	const size_t inputSize = source.size();
	const uint8_t * const input = reinterpret_cast<const uint8_t * const>(source.data());
	const uint8_t * inputCursor = input;
	for(size_t i = 0; i < inputSize; ++i) {
		const uint8_t s = symbolTable[*inputCursor];
		++inputCursor;
		if (s == 0xff) { // other char
			continue;
		} else if (s == 0xfe) { // padding found '='; end of input
			break;
		}
		++numSymbols;
	}
	// calculate data size
	size_t outputSize = (numSymbols / 4) * 3;
	switch (numSymbols % 4) {
		case 1:
			WARN("decodeBase64 source length is not multiple of 4");
			break;
		case 2:
			++outputSize;
			break;
		case 3:
			outputSize += 2;
			break;
		default:
			break;
	}
	// allocate data
	std::vector<uint8_t> destination(outputSize);
	// decode source string
	inputCursor = input;
	uint8_t * outputCursor = destination.data();
	uint_fast8_t count = 0;
	uint32_t value = 0;
	for(size_t i = 0; i < inputSize; ++i) {
		const uint8_t s = symbolTable[*inputCursor];
		++inputCursor;
		if (s == 0xff) { // other char
			continue;
		} else if (s == 0xfe) { // '='
			if (count == 2) {
				outputCursor[0] = static_cast<uint8_t> (value >> 10);
				break;
			} else if (count == 3) {
				outputCursor[0] = static_cast<uint8_t> (value >> 16);
				outputCursor[1] = static_cast<uint8_t> (value >> 8);
				break;
			}
			WARN("decodeBase64 padding error");
			break;
		} else {
			value |= s;
			// four input characters produce three output bytes
			if ((++count) == 4) {
				outputCursor[0] = static_cast<uint8_t> (value >> 16);
				outputCursor[1] = static_cast<uint8_t> (value >> 8);
				outputCursor[2] = static_cast<uint8_t> (value);
				outputCursor += 3;
				value = 0;
				count = 0;
			} else {
				value = value << 6;
			}
		}
	}
	return destination;
}
}
