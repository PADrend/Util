/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "PixelAccessor.h"
#include "../Resources/AttributeAccessor.h"
#include "../Macros.h"
#include <algorithm>
#include <cstring>

namespace Util {

bool PixelAccessor::crop(uint32_t & x,uint32_t & y,uint32_t & width,uint32_t & height) const{
	if(x>=getWidth() || y>=getHeight())
		return false;
	if(x+width>=getWidth())
		width -= (x+width)-getWidth();
	if(y+height>=getHeight())
		height -= (y+height)-getHeight();
	return true;
}

//! ---o
void PixelAccessor::doFill(uint32_t x,uint32_t y,uint32_t width,uint32_t height,const Color4f & c){
	const uint32_t maxX = x+width;
	const uint32_t maxY = y+height;
	for(uint32_t cy=y ; cy<maxY; ++cy)
		for(uint32_t cx=x ; cx<maxX ; ++cx)
			doWriteColor(cx,cy,c);
}

//-------------

static uint32_t toFloat11(float f) {
	uint32_t floatBits;
	memcpy(&floatBits, &f, sizeof(float));
	uint32_t exponent = (floatBits >> 23) & 0xffu;
	if(f <= 0 || exponent < 112) return 0;
	return ((exponent - 127 + 15) << 6) | ((floatBits & 0x7fffffu) >> 17);
}

//-------------

static uint32_t toFloat10(float f) {
	uint32_t floatBits;
	memcpy(&floatBits, &f, sizeof(float));
	uint32_t exponent = (floatBits >> 23) & 0xffu;
	if(f <= 0 || exponent < 112) return 0;
	return ((exponent - 127 + 15) << 5) | ((floatBits & 0x7fffffu) >> 18);
}

//-------------

static float fromFloat11(uint32_t float11Bits) {
	float f;
	uint32_t exponent = ((float11Bits & 0x7ffu) >> 6) + 127 - 15;
	uint32_t mantissa = (float11Bits & 0x3fu);
	uint32_t floatBits = (exponent << 23) | mantissa;
	memcpy(&f, &floatBits, sizeof(float));
	return f;
}

//-------------

static float fromFloat10(uint32_t float10Bits) {
	float f;
	uint32_t exponent = ((float10Bits & 0x3ffu) >> 5) + 127 - 15;
	uint32_t mantissa = (float10Bits & 0x1fu);
	uint32_t floatBits = (exponent << 23) | mantissa;
	memcpy(&f, &floatBits, sizeof(float));
	return f;
}

//-------------------------------------------------------------
// R11G11B10FloatAccessor

class R11G11B10FloatAccessor : public AttributeAccessor {
public:
	R11G11B10FloatAccessor(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) : AttributeAccessor(ptr, size, attr, stride) {}
		
	static Reference<AttributeAccessor> create(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) {
		return new R11G11B10FloatAccessor(ptr, size, attr, stride);
	}
		
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {	
		std::vector<float> floatValues(count);
		readValues(index, floatValues.data(), count);
		std::copy(floatValues.begin(), floatValues.end(), values);
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		std::vector<float> floatValues(values, values + count);
		writeValues(index, floatValues.data(), count);
	}
	
	virtual void readValues(size_t index, int8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, float* values, size_t count) const {
		assertRange(index);
		uint32_t v = *_ptr<const uint32_t>(index);
		if(count >= 0) *(values+0) = fromFloat11(v);
		if(count >= 1) *(values+1) = fromFloat11(v >> 11);
		if(count >= 2) *(values+2) = fromFloat10(v >> 22);
	}
	
	virtual void readValues(size_t index, double* values, size_t count) const { _readValues(index, values, count); }
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const float* values, size_t count) const {
		assertRange(index);
		uint32_t r = count >= 0 ? toFloat11(*(values+0)) : 0;
		uint32_t g = count >= 1 ? toFloat11(*(values+1)) : 0;
		uint32_t b = count >= 2 ? toFloat10(*(values+2)) : 0;
		*_ptr<uint32_t>(index) = r | (g << 11) | (b << 22);
	}
	
	virtual void writeValues(size_t index, const double* values, size_t count) const { _writeValues(index, values, count); }
};

static const bool R11G11B10FloatAccRegistered = AttributeAccessor::registerAccessor(PixelFormat::INTERNAL_TYPE_R11G11B10_FLOAT, R11G11B10FloatAccessor::create);

// ------------------------------------

//-------------------------------------------------------------
// BgraAccessor

class BgraAccessor : public AttributeAccessor {
private:
	Reference<AttributeAccessor> acc;
	BgraAccessor(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) : 
		AttributeAccessor(ptr, size, attr, stride),
		acc(AttributeAccessor::create(ptr, size, {attr.getName(), attr.getDataType(), attr.getComponentCount(), attr.isNormalized(), 0, attr.getOffset()}, stride)) { }
public:
		
	static Reference<AttributeAccessor> create(uint8_t* ptr, size_t size, const AttributeFormat& attr, size_t stride) {
		return new BgraAccessor(ptr, size, attr, stride);
	}
		
	template<typename S>
	void _readValues(size_t index, S* values, size_t count) const {	
		acc->readValues(index, values, count);
		if(count >= 3) std::swap(*values, *(values+2));
	}
	
	template<typename S>
	void _writeValues(size_t index, const S* values, size_t count) const {
		std::vector<S> tmp(values, values+count);
		if(count >= 3) std::iter_swap(tmp.begin(), tmp.begin()+2);
		acc->writeValues(index, tmp.data(), count);
	}
	
	virtual void readValues(size_t index, int8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, int64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint8_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint16_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint32_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, uint64_t* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, float* values, size_t count) const { _readValues(index, values, count); }
	virtual void readValues(size_t index, double* values, size_t count) const { _readValues(index, values, count); }
	virtual void writeValues(size_t index, const int8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const int64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint8_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint16_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint32_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const uint64_t* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const float* values, size_t count) const { _writeValues(index, values, count); }
	virtual void writeValues(size_t index, const double* values, size_t count) const { _writeValues(index, values, count); }
};

static const bool BgraAccRegistered = AttributeAccessor::registerAccessor(PixelFormat::INTERNAL_TYPE_BGRA, BgraAccessor::create);

// ------------------------------------

//! WrappedPixelAccessor ---|> PixelAccessor
class WrappedPixelAccessor : public PixelAccessor{
	Reference<AttributeAccessor> acc;
public:
	WrappedPixelAccessor(Reference<Bitmap> bitmap) : PixelAccessor(std::move(bitmap)),
		acc(AttributeAccessor::create(bitmap->data(), bitmap->getDataSize(), bitmap->getPixelFormat())) { }

	virtual ~WrappedPixelAccessor() = default;

private:
	//! ---|> PixelAccessor
	Color4f doReadColor4f(uint32_t x,uint32_t y) const override {
		std::vector<float> v{0,0,0,1};
		acc->readValues(getIndex(x,y), v.data(), 4);
		return Color4f(v);
	}

	//! ---|> PixelAccessor
	Color4ub doReadColor4ub(uint32_t x,uint32_t y) const override {
		std::vector<uint8_t> v{0,0,0,255};
		acc->readValues(getIndex(x,y), v.data(), 4);
		return Color4ub(v);
	}

	//! ---|> PixelAccessor
	float doReadSingleValueFloat(uint32_t x, uint32_t y) const override {
		return acc->readValue<float>(getIndex(x,y));
	}

	//! ---|> PixelAccessor
	uint8_t doReadSingleValueByte(uint32_t x, uint32_t y) const override {
		return acc->readValue<uint8_t>(getIndex(x,y));
	}

	//! ---|> PixelAccessor
	void doWriteColor(uint32_t x,uint32_t y,const Color4f & c) override {
		acc->writeValues(getIndex(x,y), c.data(), 4);
	}

	//! ---|> PixelAccessor
	void doWriteColor(uint32_t x,uint32_t y,const Color4ub & c) override {
		acc->writeValues(getIndex(x,y), c.data(), 4);
	}

	//! ---|> PixelAccessor
	void doWriteSingleValueFloat(uint32_t x, uint32_t y, float value) override {
		doWriteColor(x, y, Util::Color4f(value,0,0,0));
	}
};

// -----------------------------------------------------------------------------------

//! (static)
Reference<PixelAccessor> PixelAccessor::create(Reference<Bitmap> bitmap) {
	if(bitmap.isNull())
		return nullptr;
	
	const auto& format = bitmap->getPixelFormat();
	if(AttributeAccessor::hasAccessor(format)) {
		return new WrappedPixelAccessor(std::move(bitmap));
	} else {
		WARN("PixelAccessor::create: There is no implemented PixelAccessor available for this bitmap format.");
		return nullptr;
	}
}

void PixelAccessor::copy(PixelAccessor * source, PixelAccessor * dest){
	for(uint32_t x = 0; x< std::min(source->getWidth(), dest->getWidth()); ++x)
		for(uint32_t y = 0; y< std::min(source->getHeight(), dest->getHeight()); ++y)
			dest->doWriteColor(x,y,source->doReadColor4f(x,y));
}


}
