/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_PIXELFORMAT_H
#define UTIL_PIXELFORMAT_H

#include "../TypeConstant.h"
#include "../Utils.h"

#include <cstdint>

namespace Util {

constexpr uint16_t IdMask					= 0xff00u;
constexpr uint16_t sRGBMask				= 1 << 7;
constexpr uint16_t NormalizedMask = 1 << 6;
constexpr uint16_t ChannelMask		= 0b11 << 4;
constexpr uint16_t BaseTypeMask		= 0b1111;

#define BASETYPE(Type)																			(BaseTypeMask & static_cast<uint16_t>(Type))
#define CHANNELS(Num)																				(((Num - 1) << 4) & ChannelMask)
#define NORMALIZED(V)																				((V << 6) & NormalizedMask)
#define SRGB(V)																							((V << 7) & sRGBMask)
#define ID(Id)																							((static_cast<uint16_t>(Id) << 8) & IdMask)
#define DEFINE_PIXEL_FORMAT(Id, sRGB, Norm, Channels, Type) ID(Id) | SRGB(sRGB) | NORMALIZED(Norm) | CHANNELS(Channels) | BASETYPE(Type)

//! Some predefined internal pixel format types
enum class InternalType : uint8_t {
	Standard = 0,
	BGRA,
	B5G6R5,
	BGR5A1,
	R11G11B10,
	RGB10A2,
	Depth,
	BC1,
	BC2,
	BC3,
	BC4,
	BC5,
	BC6H,
	BC7,
	ETC2,
	ETC2A,
	EAC,
};

/** Representation of a pixel data format. 
 * The value encodes the base type in the least significant 4 bits.
 * Other bits are encoded as follows:
 * | 8:id | 1:sRGB | 1:normalized | 2:channels | 4:type |
 * @note The channel count and base type does not neccessarily represent the layout and byte size
 * for the pixel, but how it should be decoded, e.g., for compressed formats.
 * Id is 0 for all standard types.
 */
enum class PixelFormat : uint16_t {
	Unknown = 0xffffu,

	// 8-bit integer
	R8UInt			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::UInt8), // R 8-bit unsigned integer
	RG8UInt			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::UInt8), // RG 8-bit unsigned integer
	RGB8UInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::UInt8), // RGB 8-bit unsigned integer
	RGBA8UInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::UInt8), // RGBA 8-bit unsigned integer
	R8UNorm			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 1, BaseType::UInt8), // R 8-bit unsigned normalized integer
	RG8UNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 2, BaseType::UInt8), // RG 8-bit unsigned normalized integer
	RGB8UNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 3, BaseType::UInt8), // RGB 8-bit unsigned normalized integer
	RGBA8UNorm	= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 4, BaseType::UInt8), // RGBA 8-bit unsigned normalized integer
	sR8UNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 1, 1, 1, BaseType::UInt8), // R 8-bit sRGB unsigned normalized integer
	sRG8UNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 1, 1, 2, BaseType::UInt8), // RG 8-bit sRGB unsigned normalized integer
	sRGB8UNorm	= DEFINE_PIXEL_FORMAT(InternalType::Standard, 1, 1, 3, BaseType::UInt8), // RGB 8-bit sRGB unsigned normalized integer
	sRGBA8UNorm = DEFINE_PIXEL_FORMAT(InternalType::Standard, 1, 1, 4, BaseType::UInt8), // RGBA 8-bit sRGB unsigned normalized integer
	R8SInt			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::Int8),	 // R 8-bit signed integer
	RG8SInt			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::Int8),	 // RG 8-bit signed integer
	RGB8SInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::Int8),	 // RGB 8-bit signed integer
	RGBA8SInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::Int8),	 // RGBA 8-bit signed integer
	R8SNorm			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 1, BaseType::Int8),	 // R 8-bit signed normalized integer
	RG8SNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 2, BaseType::Int8),	 // RG 8-bit signed normalized integer
	RGB8SNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 3, BaseType::Int8),	 // RGB 8-bit signed normalized integer
	RGBA8SNorm	= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 4, BaseType::Int8),	 // RGBA 8-bit signed normalized integer

	// 16-bit integer
	R16UInt			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::UInt16), // R 16-bit unsigned integer
	RG16UInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::UInt16), // RG 16-bit unsigned integer
	RGB16UInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::UInt16), // RGB 16-bit unsigned integer
	RGBA16UInt	= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::UInt16), // RGBA 16-bit unsigned integer
	R16UNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 1, BaseType::UInt16), // R 16-bit unsigned normalized integer
	RG16UNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 2, BaseType::UInt16), // RG 16-bit unsigned normalized integer
	RGB16UNorm	= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 3, BaseType::UInt16), // RGB 16-bit unsigned normalized integer
	RGBA16UNorm = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 4, BaseType::UInt16), // RGBA 16-bit unsigned normalized integer
	R16SInt			= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::Int16),	// R 16-bit signed integer
	RG16SInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::Int16),	// RG 16-bit signed integer
	RGB16SInt		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::Int16),	// RGB 16-bit signed integer
	RGBA16SInt	= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::Int16),	// RGBA 16-bit signed integer
	R16SNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 1, BaseType::Int16),	// R 16-bit signed normalized integer
	RG16SNorm		= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 2, BaseType::Int16),	// RG 16-bit signed normalized integer
	RGB16SNorm	= DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 3, BaseType::Int16),	// RGB 16-bit signed normalized integer
	RGBA16SNorm = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 1, 4, BaseType::Int16),	// RGBA 16-bit signed normalized integer

	// 32-bit integer
	R32UInt		 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::UInt32), // R 32-bit unsigned integer
	RG32UInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::UInt32), // RG 32-bit unsigned integer
	RGB32UInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::UInt32), // RGB 32-bit unsigned integer
	RGBA32UInt = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::UInt32), // RGBA 32-bit unsigned integer
	R32SInt		 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::Int32),	 // R 32-bit signed integer
	RG32SInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::Int32),	 // RG 32-bit signed integer
	RGB32SInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::Int32),	 // RGB 32-bit signed integer
	RGBA32SInt = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::Int32),	 // RGBA 32-bit signed integer

	// 64-bit integer
	R64UInt		 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::UInt64), // R 64-bit unsigned integer
	RG64UInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::UInt64), // RG 64-bit unsigned integer
	RGB64UInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::UInt64), // RGB 64-bit unsigned integer
	RGBA64UInt = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::UInt64), // RGBA 64-bit unsigned integer
	R64SInt		 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::Int64),	 // R 64-bit signed integer
	RG64SInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::Int64),	 // RG 64-bit signed integer
	RGB64SInt	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::Int64),	 // RGB 64-bit signed integer
	RGBA64SInt = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::Int64),	 // RGBA 64-bit signed integer

	// float
	R16SFloat		 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::Float16), // R 16-bit float
	RG16SFloat	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::Float16), // RG 16-bit float
	RGB16SFloat	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::Float16), // RGB 16-bit float
	RGBA16SFloat = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::Float16), // RGBA 16-bit float
	R32SFloat		 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::Float32), // R 32-bit float
	RG32SFloat	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::Float32), // RG 32-bit float
	RGB32SFloat	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::Float32), // RGB 32-bit float
	RGBA32SFloat = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::Float32), // RGBA 32-bit float
	R64SFloat		 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 1, BaseType::Float64), // R 64-bit float
	RG64SFloat	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 2, BaseType::Float64), // RG 64-bit float
	RGB64SFloat	 = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 3, BaseType::Float64), // RGB 64-bit float
	RGBA64SFloat = DEFINE_PIXEL_FORMAT(InternalType::Standard, 0, 0, 4, BaseType::Float64), // RGBA 64-bit float

	// 8-bit reversed integer (BGRA)
	BGR8UInt		= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 0, 3, BaseType::UInt8), // BGR 8-bit unsigned integer
	BGRA8UInt		= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 0, 4, BaseType::UInt8), // BGRA 8-bit unsigned integer
	BGR8UNorm		= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 1, 3, BaseType::UInt8), // BGR 8-bit unsigned normalized integer
	BGRA8UNorm	= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 1, 4, BaseType::UInt8), // BGRA 8-bit unsigned normalized integer
	sBGR8UNorm	= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 1, 1, 3, BaseType::UInt8), // BGR 8-bit sRGB unsigned normalized integer
	sBGRA8UNorm = DEFINE_PIXEL_FORMAT(InternalType::BGRA, 1, 1, 4, BaseType::UInt8), // BGRA 8-bit sRGB unsigned normalized integer
	BGR8SInt		= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 0, 3, BaseType::Int8),	 // BGR 8-bit signed integer
	BGRA8SInt		= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 0, 4, BaseType::Int8),	 // BGRA 8-bit signed integer
	BGR8SNorm		= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 1, 3, BaseType::Int8),	 // BGR 8-bit signed normalized integer
	BGRA8SNorm	= DEFINE_PIXEL_FORMAT(InternalType::BGRA, 0, 1, 4, BaseType::Int8),	 // BGRA 8-bit signed normalized integer

	// non-uniform packed formats
	B5G6R5UNorm			= DEFINE_PIXEL_FORMAT(InternalType::B5G6R5, 0, 1, 3, BaseType::UInt8),			// B 5-bit, G 6-bit, R 5-bit, unsigned normalized integer (16-bit total)
	BGR5A1UNorm			= DEFINE_PIXEL_FORMAT(InternalType::BGR5A1, 0, 1, 4, BaseType::UInt8),			// BGR 5-bit, A 1-bit unsigned normalized integer (16-bit total)
	R11G11B10SFloat = DEFINE_PIXEL_FORMAT(InternalType::R11G11B10, 0, 0, 3, BaseType::Float16), // RG 11-bit, B 10-bit, unsigned float (32-bit total; 5-bit exponents)
	RGB10A2UNorm		= DEFINE_PIXEL_FORMAT(InternalType::RGB10A2, 0, 1, 4, BaseType::UInt8),			// RGB 10-bit, A 2-bit, unsigned normalized integer (32-bit total)

	// depth/stencil formats
	D16UNorm		= DEFINE_PIXEL_FORMAT(InternalType::Depth, 0, 1, 1, BaseType::UInt16),	// Depth 16-bit unsigned normalized integer
	D24S8UNorm	= DEFINE_PIXEL_FORMAT(InternalType::Depth, 0, 1, 2, BaseType::UInt32),	// Depth 24-bit, Stencil 8-bit unsigned normalized integer (32-bit total)
	D32SFloat		= DEFINE_PIXEL_FORMAT(InternalType::Depth, 0, 0, 1, BaseType::Float32), // Depth 32-bit float
	D32S8SFloat = DEFINE_PIXEL_FORMAT(InternalType::Depth, 0, 0, 2, BaseType::Float32), // Depth 32-bit float, Stencil 8-bit unsigned normalized integer (40-bit total; might be aligned to 64-bit)

	// BC compressed formats
	RGB8UNormBC1		= DEFINE_PIXEL_FORMAT(InternalType::BC1, 0, 1, 3, BaseType::UInt8),		 // RGB 8-bit BC1 compressed unsigned normalized integer (4x4 block in 64-bit)
	sRGB8UNormBC1		= DEFINE_PIXEL_FORMAT(InternalType::BC1, 1, 1, 3, BaseType::UInt8),		 // RGB 8-bit BC1 compressed sRGB unsigned normalized integer (4x4 block in 64-bit)
	RGB8A1UNormBC1	= DEFINE_PIXEL_FORMAT(InternalType::BC1, 0, 1, 4, BaseType::UInt8),		 // RGB 8-bit, A 1-bit BC1 compressed unsigned normalized integer (4x4 block in 64-bit)
	sRGB8A1UNormBC1 = DEFINE_PIXEL_FORMAT(InternalType::BC1, 1, 1, 4, BaseType::UInt8),		 // RGB 8-bit, A 1-bit BC1 compressed sRGB unsigned normalized integer (4x4 block in 64-bit)
	RGB8A4UNormBC2	= DEFINE_PIXEL_FORMAT(InternalType::BC2, 0, 1, 4, BaseType::UInt8),		 // RGB 8-bit, A 4-bit BC2 compressed unsigned normalized integer (4x4 block in 128-bit; uncompressed A + compressed RGB)
	sRGB8A4UNormBC2 = DEFINE_PIXEL_FORMAT(InternalType::BC2, 1, 1, 4, BaseType::UInt8),		 // RGB 8-bit, A 4-bit BC2 compressed sRGB unsigned normalized integer (4x4 block in 128-bit; uncompressed A + compressed RGB)
	RGBA8UNormBC3		= DEFINE_PIXEL_FORMAT(InternalType::BC3, 0, 1, 4, BaseType::UInt8),		 // RGBA 8-bit BC2 compressed unsigned normalized integer (4x4 block in 128-bit; compressed A + compressed RGB)
	sRGBA8UNormBC3	= DEFINE_PIXEL_FORMAT(InternalType::BC3, 1, 1, 4, BaseType::UInt8),		 // RGBA 8-bit BC2 compressed sRGB unsigned normalized integer (4x4 block in 128-bit; compressed A + compressed RGB)
	R8UNormBC4			= DEFINE_PIXEL_FORMAT(InternalType::BC4, 0, 1, 1, BaseType::UInt8),		 // R 8-bit BC4 compressed unsigned normalized integer (4x4 block in 64-bit)
	R8SNormBC4			= DEFINE_PIXEL_FORMAT(InternalType::BC4, 0, 1, 1, BaseType::Int8),		 // R 8-bit BC4 compressed signed normalized integer (4x4 block in 64-bit)
	RG8UNormBC5			= DEFINE_PIXEL_FORMAT(InternalType::BC5, 0, 1, 2, BaseType::UInt8),		 // RG 8-bit BC5 compressed unsigned normalized integer (4x4 block in 128-bit)
	RG8SNormBC5			= DEFINE_PIXEL_FORMAT(InternalType::BC5, 1, 1, 2, BaseType::Int8),		 // RG 8-bit BC5 compressed signed normalized integer (4x4 block in 128-bit)
	RGB16SFloatBC6H = DEFINE_PIXEL_FORMAT(InternalType::BC6H, 1, 0, 3, BaseType::Float32), // RGB 16-bit BC6 compressed signed half-float (4x4 block in 128-bit)
	RGBA8UNormBC7		= DEFINE_PIXEL_FORMAT(InternalType::BC7, 0, 1, 4, BaseType::UInt8),		 // RGBA 8-bit BC7 compressed unsigned normalized integer (4x4 block in 128-bit)
	sRGBA8UNormBC7	= DEFINE_PIXEL_FORMAT(InternalType::BC7, 1, 1, 4, BaseType::UInt8),		 // RGBA 8-bit BC7 compressed sRGB unsigned normalized integer (4x4 block in 128-bit)

	// ETC compressed formats
	RGB8UNormETC2		 = DEFINE_PIXEL_FORMAT(InternalType::ETC2, 0, 1, 3, BaseType::UInt8),	 // RGB 8-bit ETC2 compressed unsigned normalized integer (4x4 block in 64-bit)
	sRGB8UNormETC2	 = DEFINE_PIXEL_FORMAT(InternalType::ETC2, 1, 1, 3, BaseType::UInt8),	 // RGB 8-bit ETC2 compressed sRGB unsigned normalized integer (4x4 block in 64-bit)
	RGB8A1UNormETC2	 = DEFINE_PIXEL_FORMAT(InternalType::ETC2A, 0, 1, 4, BaseType::UInt8), // RGB 8-bit, A 1-bit ETC2 compressed unsigned normalized integer (4x4 block in 64-bit)
	sRGB8A1UNormETC2 = DEFINE_PIXEL_FORMAT(InternalType::ETC2A, 1, 1, 4, BaseType::UInt8), // RGB 8-bit, A 1-bit ETC2 compressed sRGB unsigned normalized integer (4x4 block in 64-bit)
	RGBA8UNormETC2	 = DEFINE_PIXEL_FORMAT(InternalType::ETC2, 0, 1, 4, BaseType::UInt8),	 // RGBA 8-bit ETC2 compressed unsigned normalized integer (4x4 block in 128-bit)
	sRGBA8UNormETC2	 = DEFINE_PIXEL_FORMAT(InternalType::ETC2, 1, 1, 4, BaseType::UInt8),	 // RGBA 8-bit ETC2 compressed sRGB unsigned normalized integer (4x4 block in 128-bit)
	R11UIntEAC			 = DEFINE_PIXEL_FORMAT(InternalType::EAC, 0, 0, 1, BaseType::UInt16),	 // R 11-bit EAC compressed unsigned integer (4x4 block in 64-bit)
	R11SIntEAC			 = DEFINE_PIXEL_FORMAT(InternalType::EAC, 0, 0, 1, BaseType::Int16),	 // R 11-bit EAC compressed signed integer (4x4 block in 64-bit)
	RG11UIntEAC			 = DEFINE_PIXEL_FORMAT(InternalType::EAC, 0, 0, 2, BaseType::UInt16),	 // RG 11-bit EAC compressed unsigned integer (4x4 block in 128-bit)
	RG11SIntEAC			 = DEFINE_PIXEL_FORMAT(InternalType::EAC, 0, 0, 2, BaseType::Int16),	 // RG 11-bit EAC compressed signed integer (4x4 block in 128-bit)
};

//! Returns the string representation of the pixel format.
UTILAPI std::string toString(PixelFormat format);

//! Extracts the internal format from the pixel format.
constexpr InternalType getInternalType(PixelFormat format) {
	return static_cast<InternalType>(static_cast<uint16_t>(format) >> 8);
}

//! Extracts the internal format id from the pixel format.
constexpr uint8_t getInternalTypeId(PixelFormat format) {
	return static_cast<uint8_t>(getInternalType(format));
}

//! Extracts the sRGB flag from the pixel format.
constexpr bool isSRGB(PixelFormat format) {
	return (static_cast<uint16_t>(format) & sRGBMask) > 0;
}

//! Extracts the normalized flag from the pixel format.
constexpr bool isNormalized(PixelFormat format) {
	return (static_cast<uint16_t>(format) & NormalizedMask) > 0;
}

//! Extracts the number of channels from the pixel format.
constexpr uint8_t getChannelCount(PixelFormat format) {
	return static_cast<uint8_t>((static_cast<uint16_t>(format) & ChannelMask) >> 4) + 1;
}

//! Extracts the base type from the pixel format.
constexpr BaseType getBaseType(PixelFormat format) {
	return static_cast<BaseType>(static_cast<uint16_t>(format) & BaseTypeMask);
}

//! Returns the block size in bytes of the pixel format as it is used in a compressed or packed format (e.g., 16 for BC7).
//! For non-compressed formats, this matches the byte size of the pixel.
constexpr uint8_t getBlockSizeBytes(PixelFormat format) {
	switch (getInternalType(format)) {
		case InternalType::Standard:
		case InternalType::BGRA:
			return getChannelCount(format) * getNumBytes(getBaseType(format));
		case InternalType::B5G6R5:
		case InternalType::BGR5A1:
			return 2;
		case InternalType::R11G11B10:
		case InternalType::RGB10A2:
			return 4;
		case InternalType::Depth:
			if (format == PixelFormat::D32S8SFloat)
				return 5; // special snowflake
			else if (format == PixelFormat::D16UNorm)
				return 2;
			else
				return 4;
		case InternalType::BC1:
		case InternalType::BC4:
		case InternalType::ETC2A:
			return 8;
		case InternalType::BC2:
		case InternalType::BC3:
		case InternalType::BC5:
		case InternalType::BC6H:
		case InternalType::BC7:
			return 16;
		case InternalType::ETC2:
			return getChannelCount(format) == 4 ? 16 : 8;
		case InternalType::EAC:
			return getChannelCount(format) * 8;
		default:
			return 0;
	}
}

//! Returns the the dimensions (widthxheightxdepth) of a compressed pixel block (e.g., 4x4x1 for BC7), or 1x1x1 for other pixel formats.
constexpr std::tuple<uint8_t, uint8_t, uint8_t> getBlockDimensions(PixelFormat format) {
	switch (getInternalType(format)) {
		case InternalType::BC1:
		case InternalType::BC4:
		case InternalType::ETC2A:
		case InternalType::BC2:
		case InternalType::BC3:
		case InternalType::BC5:
		case InternalType::BC6H:
		case InternalType::BC7:
		case InternalType::ETC2:
		case InternalType::EAC:
			return {4,4,1};
		default:
			return {1,1,1};
	}
}

//! Computes the total size of an image in bytes based on the image dimensions and pixel format.
constexpr uint64_t computeImageSizeBytes(PixelFormat format, uint32_t width, uint32_t height, uint32_t depth=1) {
	const auto blockSize = getBlockSizeBytes(format);
	const auto [dx,dy,dz] = getBlockDimensions(format);
	return ((width + dx - 1) / dx) * ((height + dy - 1) / dy) * ((depth + dz - 1) / dz) * blockSize;
}

// gets the pixel format for the given values
constexpr PixelFormat getPixelFormat(BaseType baseType, uint8_t channels = 1, bool normalized = false, bool sRGB = false, uint8_t internalType = 0) {
	return static_cast<PixelFormat>(DEFINE_PIXEL_FORMAT(internalType, sRGB, normalized, channels, baseType));
}

// converts the pixel format to the same format with different channel count
constexpr PixelFormat updateChannelCount(PixelFormat format, uint8_t channels) {
	return static_cast<PixelFormat>((static_cast<uint16_t>(format) & ~ChannelMask) | CHANNELS(channels));
}

// converts the pixel format to the same format with different internal type
constexpr PixelFormat updateInternalType(PixelFormat format, uint8_t id) {
	return static_cast<PixelFormat>((static_cast<uint16_t>(format) & ~IdMask) | ID(id));
}

// converts the pixel format to the same format with different internal type
constexpr PixelFormat updateInternalType(PixelFormat format, InternalType id) {
	return static_cast<PixelFormat>((static_cast<uint16_t>(format) & ~IdMask) | ID(id));
}

// converts the pixel format to the same format with a different base type
constexpr PixelFormat updateBaseType(PixelFormat format, BaseType type) {
	return static_cast<PixelFormat>((static_cast<uint16_t>(format) & ~BaseTypeMask) | BASETYPE(type));
}

#undef DEFINE_PIXEL_FORMAT
#undef BASETYPE
#undef CHANNELS
#undef NORMALIZED
#undef SRGB
#undef ID

} // namespace Util

#endif /* UTIL_PIXELFORMAT_H */
