/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "BitmapUtils.h"
#include "Bitmap.h"
#include "PixelAccessor.h"
#include "../Macros.h"
#include "../References.h"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace Util {
namespace BitmapUtils {

Reference<Bitmap> blendTogether(PixelFormat targetFormat, 
								const std::vector<Reference<Bitmap>> & sources) {
	if(sources.empty()){
		throw std::invalid_argument("blendTogether: 'sources' may not be empty.");
	}

	const uint32_t width = sources.front()->getWidth();
	const uint32_t height = sources.front()->getHeight();

	std::vector<Color4f> buffer(width*height);

	for(const auto & source : sources) {
		Reference<PixelAccessor> reader( PixelAccessor::create(source.get()));

		auto pixel = buffer.begin();
		for(uint32_t y = 0;y<height;++y )
			for(uint32_t x = 0;x<width;++x )
				(*(pixel++)) += reader->readColor4f(x,y);
	}

	Reference<Bitmap> target(new Bitmap(width,height,targetFormat));
	{
		const float scale = 1.0f / static_cast<float>(sources.size());
		Reference<PixelAccessor> writer( PixelAccessor::create(target.get()));
		auto pixel = buffer.begin();
		for(uint32_t y = 0;y<height;++y )
			for(uint32_t x = 0;x<width;++x )
				writer->writeColor(x,y,(*(pixel++)) * scale );
	}

	return target;
}

Reference<Bitmap> combineInterleaved(PixelFormat targetFormat, 
									 const std::vector<Reference<Bitmap>> & sourceBitmaps) {
	if(sourceBitmaps.empty()){
		throw std::invalid_argument("blendTogether: 'sources' may not be empty.");
	}

	const uint32_t count = static_cast<uint32_t>(std::sqrt(sourceBitmaps.size()));
	const uint32_t width = sourceBitmaps.front()->getWidth();
	const uint32_t height = sourceBitmaps.front()->getHeight();
	
	Reference<Bitmap> targetBitmap(new Bitmap(width*count,height*count,targetFormat));
	{
		Reference<PixelAccessor> target = PixelAccessor::create(targetBitmap.get());

		std::vector<Reference<PixelAccessor>> sources;
		for(auto & bm : sourceBitmaps){
			sources.push_back(PixelAccessor::create(bm.get()));
		}

		for(uint32_t x = 0; x < width*count; x++)
			for(uint32_t y = 0; y < height*count; y++){
				target->writeColor(x,y,sources[(y%count)*count+(x%count)]->readColor4f(x/count,y/count));
			}
	}
	return targetBitmap;
}

Reference<Bitmap> convertBitmap(const Bitmap & source, 
								PixelFormat newFormat) {
	const uint32_t width = source.getWidth();
	const uint32_t height = source.getHeight();

	Reference<Bitmap> target(new Bitmap(width,height,newFormat));
	{
		Reference<PixelAccessor> reader( PixelAccessor::create(const_cast<Bitmap *>(&source)));
		Reference<PixelAccessor> writer( PixelAccessor::create(target.get()));
		for(uint32_t y = 0;y<height;++y )
			for(uint32_t x = 0;x<width;++x )
				writer->writeColor(x,y,reader->readColor4f(x,y));
	}
	return target;
}


Reference<Bitmap> expandChannels(const Bitmap & source, uint32_t desiredChannels) {
	desiredChannels = clamp(desiredChannels, 1u, 4u);
	const uint32_t width = source.getWidth();
	const uint32_t height = source.getHeight();
	PixelFormat f = source.getPixelFormat();
	PixelFormat newFormat = getPixelFormat(getBaseType(f), desiredChannels, isNormalized(f), isSRGB(f), getInternalTypeId(f));
	const uint32_t channels = getChannelCount(f);

	Reference<Bitmap> target(new Bitmap(width,height,newFormat));
	{
		Reference<PixelAccessor> reader( PixelAccessor::create(const_cast<Bitmap *>(&source)));
		Reference<PixelAccessor> writer( PixelAccessor::create(target.get()));
		for(uint32_t y = 0;y<height;++y ) {
			for(uint32_t x = 0;x<width;++x ) {
				auto color = reader->readColor4f(x,y);
				if(channels < 4)
					color.a(1.0);
				if(channels < 2)
					color.g(color.r());
				if(channels < 3)
					color.b(color.g());
				writer->writeColor(x,y,color);
			}
		}
	}
	return target;
}

void alterBitmap(Bitmap & bitmap, const BitmapAlteringFunction & op) {
	const uint32_t width = bitmap.getWidth();
	const uint32_t height = bitmap.getHeight();
	Reference<PixelAccessor> pixels = PixelAccessor::create(&bitmap);
	BitmapAlteringContext ctxt;
	ctxt.pixels = pixels.get();
	for(ctxt.y = 0; ctxt.y < height; ++ctxt.y) {
		for(ctxt.x = 0; ctxt.x < width; ++ctxt.x) {
			pixels->writeColor(ctxt.x, ctxt.y, op(ctxt));
		}
	}
}

Reference<Bitmap> createBitmapFromBitMask(const uint32_t width,
										  const uint32_t height,
										  PixelFormat format,
										  const size_t dataSize,
										  const uint8_t * data) {
	if(width*height!=dataSize*8 || width%8!=0){
		throw std::invalid_argument("createBitmapFromBitMask: Illegal bitmap size.");
	}
	Reference<Bitmap> target(new Bitmap(width,height,format));
	{
		Reference<PixelAccessor> writer( PixelAccessor::create(target.get()));
		//	const Color4ub black(0,0,0,0);
		const Color4ub white(255,255,255,255);
		const uint8_t * cursor = data;
		for(uint32_t y = 0;y<height;++y ){
			for(uint32_t x = 0;x<width; ){
				uint8_t value = *cursor++;
				for(uint8_t mask = 128 ;mask!=0;mask = mask >> 1){
					if( (value&mask) >0)
						writer->writeColor(x,y,white);
					++x;
				}
			}
		}
		
	}
	return target;
}

void normalizeBitmap(Bitmap & bitmap) {
	const uint32_t width = bitmap.getWidth();
	const uint32_t height = bitmap.getHeight();
	Reference<PixelAccessor> pixels = PixelAccessor::create(&bitmap);
	Color4f max(0,0,0,0);
	// get max
	for(uint32_t y = 0; y < height; ++y) {
		for(uint32_t x = 0; x < width; ++x) {
			auto p = pixels->readColor4f(x, y);
			max.r(std::max(max.r(), p.r()));
			max.g(std::max(max.g(), p.g()));
			max.b(std::max(max.b(), p.b()));
			max.a(std::max(max.a(), p.a()));
		}
	}
	// normalize
	for(uint32_t y = 0; y < height; ++y) {
		for(uint32_t x = 0; x < width; ++x) {
			auto p = pixels->readColor4f(x, y);
			p.r(p.r() / max.r());
			p.g(p.g() / max.g());
			p.b(p.b() / max.b());
			p.a(p.a() / max.a());
			pixels->writeColor(x, y, p);
		}
	}
}

}
}

