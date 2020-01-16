/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Bitmap.h"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Util {

Bitmap::Bitmap(const uint32_t _width,const uint32_t _height,AttributeFormat _pixelFormat) :
		pixelFormat(std::move(_pixelFormat)), width(_width), height(_height), pixelData(pixelFormat.getDataSize() * width * height) {
}

Bitmap::Bitmap(const uint32_t _width,const uint32_t _height,size_t rawDataSize) :
		pixelFormat(PixelFormat::UNKNOWN), width(_width), height(_height), pixelData(rawDataSize) {
}

Bitmap::Bitmap(const Bitmap & source) :
		ReferenceCounter_t(),
		pixelFormat(source.pixelFormat), width(source.width), height(source.height),
		pixelData(source.pixelData) {
}

void Bitmap::swap(Bitmap & other){
	using std::swap;
	swap(pixelFormat, other.pixelFormat);
	swap(width, other.width);
	swap(height, other.height);
	swap(pixelData, other.pixelData);
}

void Bitmap::setData(const std::vector<uint8_t> & newData) {
	if(newData.size() != pixelData.size()) 
		throw std::invalid_argument("Bitmap::setData: Sizes differ.");
	pixelData = newData;
}
void Bitmap::swapData(std::vector<uint8_t> & other) {
	if(other.size() != pixelData.size()) 
		throw std::invalid_argument("Bitmap::swapData: Sizes differ.");
	using std::swap;
	swap( other, pixelData);
}

void Bitmap::flipVertically() {
	if(pixelData.empty()) 
		return;

	std::vector<uint8_t> temp(pixelData.size());
	const uint8_t * src = pixelData.data();
	uint8_t * dst = temp.data();
	const uint32_t rowDataSize( width*pixelFormat.getDataSize() );

	for(int32_t y = (height - 1); y >= 0; --y) {
		const uint8_t * rowBegin = src + y * rowDataSize;
		std::copy(rowBegin, rowBegin + rowDataSize, dst);
		dst += rowDataSize;
	}
	using std::swap;
	swap(pixelData, temp);
}

}
