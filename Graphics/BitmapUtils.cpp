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

#ifdef UTIL_HAVE_LIB_SDL2
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL.h>
COMPILER_WARN_POP
#endif /* UTIL_HAVE_LIB_SDL2 */

#include <cmath>
#include <stdexcept>
#include <vector>

namespace Util {
namespace BitmapUtils {

#ifdef UTIL_HAVE_LIB_SDL2
Reference<Bitmap> createBitmapFromSDLSurface(SDL_Surface * surface) {
	SDL_PixelFormat * sdlFormat = surface->format;
	const uint8_t & bytes = sdlFormat->BytesPerPixel;

	Reference<Bitmap> bitmap(new Bitmap(static_cast<uint32_t>(surface->w), 
										static_cast<uint32_t>(surface->h), 
										bytes > 3 ? PixelFormat::RGBA : PixelFormat::RGB));

	SDL_LockSurface(surface);

	const uint8_t * source = reinterpret_cast<const uint8_t *>(surface->pixels);
	uint8_t * pixels = bitmap->data();

	for (uint_fast16_t y = 0; y < static_cast<uint_fast16_t>(surface->h); ++y) {
		for (uint_fast16_t x = 0; x < static_cast<uint_fast16_t>(surface->w); ++x) {
			if (bytes == 1) {
				// Use color palette to retrieve colors.
				uint8_t index = source[x];
				const SDL_Color & color = sdlFormat->palette->colors[index];
				*pixels = color.r;
				++pixels;
				*pixels = color.g;
				++pixels;
				*pixels = color.b;
				++pixels;
			} else {
				const uint8_t * sourcePixel = source + bytes * x;
				uint8_t r;
				uint8_t g;
				uint8_t b;
				if (bytes > 3) {
					uint8_t a;
					SDL_GetRGBA(*reinterpret_cast<const uint32_t *>(sourcePixel), sdlFormat, &r, &g, &b, &a);
					*pixels = r;
					++pixels;
					*pixels = g;
					++pixels;
					*pixels = b;
					++pixels;
					*pixels = a;
					++pixels;
				} else {
					// Make sure only 24 bits are read here.
					const uint32_t pixel = static_cast<uint32_t>(sourcePixel[2] << 16) | static_cast<uint32_t>(sourcePixel[1] << 8) | static_cast<uint32_t>(sourcePixel[0]);
					SDL_GetRGB(pixel, sdlFormat, &r, &g, &b);
					*pixels = r;
					++pixels;
					*pixels = g;
					++pixels;
					*pixels = b;
					++pixels;
				}
			}
		}
		source += surface->pitch;
	}

	SDL_UnlockSurface(surface);

	return bitmap;
}

SDL_Surface * createSDLSurfaceFromBitmap(const Bitmap & bitmap) {
	uint32_t rMask=0x00;
	uint32_t gMask=0x00;
	uint32_t bMask=0x00;
	uint32_t aMask=0x00;

	const auto & f = bitmap.getPixelFormat();
	if(f==PixelFormat::RGBA){ /// \note assumes little endianess!!!
		rMask = 0x000000ff;
		gMask = 0x0000ff00;
		bMask = 0x00ff0000;
		aMask = 0xff000000;
	}else if(f==PixelFormat::BGRA){
		rMask = 0x00ff0000;
		gMask = 0x0000ff00;
		bMask = 0x000000ff;
		aMask = 0xff000000;
	}else if(f==PixelFormat::RGB){
		rMask = 0x000000ff;
		gMask = 0x0000ff00;
		bMask = 0x00ff0000;
	}else if(f==PixelFormat::BGR){
		rMask = 0x00ff0000;
		gMask = 0x0000ff00;
		bMask = 0x000000ff;
	}else if(f==PixelFormat::MONO){
		rMask = 0xff;
	}else{
		WARN("createSDLSurfaceFromBitmap: Unsupported color format: ");
		return nullptr;
	}

	int depth = 8 * f.getBytesPerPixel();
	int pitch = static_cast<int>(bitmap.getWidth()) * f.getBytesPerPixel();
	return SDL_CreateRGBSurfaceFrom(const_cast<void *>(reinterpret_cast<const void *>(bitmap.data())), 
									static_cast<int>(bitmap.getWidth()), 
									static_cast<int>(bitmap.getHeight()), 
									depth, pitch, rMask, gMask, bMask, aMask);
}
#endif /* UTIL_HAVE_LIB_SDL2 */

Reference<Bitmap> blendTogether(const AttributeFormat & targetFormat, 
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
		const float scale = 1.0 / sources.size();
		Reference<PixelAccessor> writer( PixelAccessor::create(target.get()));
		auto pixel = buffer.begin();
		for(uint32_t y = 0;y<height;++y )
			for(uint32_t x = 0;x<width;++x )
				writer->writeColor(x,y,(*(pixel++)) * scale );
	}

	return target;
}

Reference<Bitmap> combineInterleaved(const AttributeFormat & targetFormat, 
									 const std::vector<Reference<Bitmap>> & sourceBitmaps) {
	if(sourceBitmaps.empty()){
		throw std::invalid_argument("blendTogether: 'sources' may not be empty.");
	}

	const uint32_t count = std::sqrt(sourceBitmaps.size());
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
								const AttributeFormat & newFormat) {
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
										  const AttributeFormat & format,
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

