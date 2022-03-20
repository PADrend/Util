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
#include "PixelFormat.h"

namespace Util {

std::string toString(PixelFormat format) {
	switch (format) {
		case PixelFormat::R8UInt: return "R8UInt";
		case PixelFormat::RG8UInt: return "RG8UInt";
		case PixelFormat::RGB8UInt: return "RGB8UInt";
		case PixelFormat::RGBA8UInt: return "RGBA8UInt";
		case PixelFormat::R8UNorm: return "R8UNorm";
		case PixelFormat::RG8UNorm: return "RG8UNorm";
		case PixelFormat::RGB8UNorm: return "RGB8UNorm";
		case PixelFormat::RGBA8UNorm: return "RGBA8UNorm";
		case PixelFormat::sR8UNorm: return "sR8UNorm";
		case PixelFormat::sRG8UNorm: return "sRG8UNorm";
		case PixelFormat::sRGB8UNorm: return "sRGB8UNorm";
		case PixelFormat::sRGBA8UNorm: return "sRGBA8UNorm";
		case PixelFormat::R8SInt: return "R8SInt";
		case PixelFormat::RG8SInt: return "RG8SInt";
		case PixelFormat::RGB8SInt: return "RGB8SInt";
		case PixelFormat::RGBA8SInt: return "RGBA8SInt";
		case PixelFormat::R8SNorm: return "R8SNorm";
		case PixelFormat::RG8SNorm: return "RG8SNorm";
		case PixelFormat::RGB8SNorm: return "RGB8SNorm";
		case PixelFormat::RGBA8SNorm: return "RGBA8SNorm";
		case PixelFormat::R16UInt: return "R16UInt";
		case PixelFormat::RG16UInt: return "RG16UInt";
		case PixelFormat::RGB16UInt: return "RGB16UInt";
		case PixelFormat::RGBA16UInt: return "RGBA16UInt";
		case PixelFormat::R16UNorm: return "R16UNorm";
		case PixelFormat::RG16UNorm: return "RG16UNorm";
		case PixelFormat::RGB16UNorm: return "RGB16UNorm";
		case PixelFormat::RGBA16UNorm: return "RGBA16UNorm";
		case PixelFormat::R16SInt: return "R16SInt";
		case PixelFormat::RG16SInt: return "RG16SInt";
		case PixelFormat::RGB16SInt: return "RGB16SInt";
		case PixelFormat::RGBA16SInt: return "RGBA16SInt";
		case PixelFormat::R16SNorm: return "R16SNorm";
		case PixelFormat::RG16SNorm: return "RG16SNorm";
		case PixelFormat::RGB16SNorm: return "RGB16SNorm";
		case PixelFormat::RGBA16SNorm: return "RGBA16SNorm";
		case PixelFormat::R32UInt: return "R32UInt";
		case PixelFormat::RG32UInt: return "RG32UInt";
		case PixelFormat::RGB32UInt: return "RGB32UInt";
		case PixelFormat::RGBA32UInt: return "RGBA32UInt";
		case PixelFormat::R32SInt: return "R32SInt";
		case PixelFormat::RG32SInt: return "RG32SInt";
		case PixelFormat::RGB32SInt: return "RGB32SInt";
		case PixelFormat::RGBA32SInt: return "RGBA32SInt";
		case PixelFormat::R64UInt: return "R64UInt";
		case PixelFormat::RG64UInt: return "RG64UInt";
		case PixelFormat::RGB64UInt: return "RGB64UInt";
		case PixelFormat::RGBA64UInt: return "RGBA64UInt";
		case PixelFormat::R64SInt: return "R64SInt";
		case PixelFormat::RG64SInt: return "RG64SInt";
		case PixelFormat::RGB64SInt: return "RGB64SInt";
		case PixelFormat::RGBA64SInt: return "RGBA64SInt";
		case PixelFormat::R16SFloat: return "R16SFloat";
		case PixelFormat::RG16SFloat: return "RG16SFloat";
		case PixelFormat::RGB16SFloat: return "RGB16SFloat";
		case PixelFormat::RGBA16SFloat: return "RGBA16SFloat";
		case PixelFormat::R32SFloat: return "R32SFloat";
		case PixelFormat::RG32SFloat: return "RG32SFloat";
		case PixelFormat::RGB32SFloat: return "RGB32SFloat";
		case PixelFormat::RGBA32SFloat: return "RGBA32SFloat";
		case PixelFormat::R64SFloat: return "R64SFloat";
		case PixelFormat::RG64SFloat: return "RG64SFloat";
		case PixelFormat::RGB64SFloat: return "RGB64SFloat";
		case PixelFormat::RGBA64SFloat: return "RGBA64SFloat";
		case PixelFormat::BGR8UInt: return "BGR8UInt";
		case PixelFormat::BGRA8UInt: return "BGRA8UInt";
		case PixelFormat::BGR8UNorm: return "BGR8UNorm";
		case PixelFormat::BGRA8UNorm: return "BGRA8UNorm";
		case PixelFormat::sBGR8UNorm: return "sBGR8UNorm";
		case PixelFormat::sBGRA8UNorm: return "sBGRA8UNorm";
		case PixelFormat::BGR8SInt: return "BGR8SInt";
		case PixelFormat::BGRA8SInt: return "BGRA8SInt";
		case PixelFormat::BGR8SNorm: return "BGR8SNorm";
		case PixelFormat::BGRA8SNorm: return "BGRA8SNorm";
		case PixelFormat::B5G6R5UNorm: return "B5G6R5UNorm";
		case PixelFormat::BGR5A1UNorm: return "BGR5A1UNorm";
		case PixelFormat::R11G11B10SFloat: return "R11G11B10SFloat";
		case PixelFormat::RGB10A2UNorm: return "RGB10A2UNorm";
		case PixelFormat::D16UNorm: return "D16UNorm";
		case PixelFormat::D24S8UNorm: return "D24S8UNorm";
		case PixelFormat::D32SFloat: return "D32SFloat";
		case PixelFormat::D32S8SFloat: return "D32S8SFloat";
		case PixelFormat::RGB8UNormBC1: return "RGB8UNormBC1";
		case PixelFormat::sRGB8UNormBC1: return "sRGB8UNormBC1";
		case PixelFormat::RGB8A1UNormBC1: return "RGB8A1UNormBC1";
		case PixelFormat::sRGB8A1UNormBC1: return "sRGB8A1UNormBC1";
		case PixelFormat::RGB8A4UNormBC2: return "RGB8A4UNormBC2";
		case PixelFormat::sRGB8A4UNormBC2: return "sRGB8A4UNormBC2";
		case PixelFormat::RGBA8UNormBC3: return "RGBA8UNormBC3";
		case PixelFormat::sRGBA8UNormBC3: return "sRGBA8UNormBC3";
		case PixelFormat::R8UNormBC4: return "R8UNormBC4";
		case PixelFormat::R8SNormBC4: return "R8SNormBC4";
		case PixelFormat::RG8UNormBC5: return "RG8UNormBC5";
		case PixelFormat::RG8SNormBC5: return "RG8SNormBC5";
		case PixelFormat::RGB16SFloatBC6H: return "RGB16SFloatBC6H";
		case PixelFormat::RGBA8UNormBC7: return "RGBA8UNormBC7";
		case PixelFormat::sRGBA8UNormBC7: return "sRGBA8UNormBC7";
		case PixelFormat::RGB8UNormETC2: return "RGB8UNormETC2";
		case PixelFormat::sRGB8UNormETC2: return "sRGB8UNormETC2";
		case PixelFormat::RGB8A1UNormETC2: return "RGB8A1UNormETC2";
		case PixelFormat::sRGB8A1UNormETC2: return "sRGB8A1UNormETC2";
		case PixelFormat::RGBA8UNormETC2: return "RGBA8UNormETC2";
		case PixelFormat::sRGBA8UNormETC2: return "sRGBA8UNormETC2";
		case PixelFormat::R11UIntEAC: return "R11UIntEAC";
		case PixelFormat::R11SIntEAC: return "R11SIntEAC";
		case PixelFormat::RG11UIntEAC: return "RG11UIntEAC";
		case PixelFormat::RG11SIntEAC: return "RG11SIntEAC";
		default: return "Unknown";
	}
}

//=====================================================================
// static tests

#define TEST_PIXEL_FORMAT(Format, Internal, sRGB, Nrm, Channels, Type, BlockSize, BlockDimX, BlockDimY, BlockDimZ, ImgSize8x8, ImgSize9x9, ImgSize2x5) \
	static_assert(InternalType::Internal == getInternalType(PixelFormat::Format), #Format " has wrong internal format");                                 \
	static_assert(sRGB == isSRGB(PixelFormat::Format), #Format " has wrong sRGB flag");                                                                  \
	static_assert(Nrm == isNormalized(PixelFormat::Format), #Format " has wrong normalized flag");                                                       \
	static_assert(Channels == getChannelCount(PixelFormat::Format), #Format " has wrong channel count");                                                 \
	static_assert(BaseType::Type == getBaseType(PixelFormat::Format), #Format " has wrong base type");                                                   \
	static_assert(BlockSize == getBlockSizeBytes(PixelFormat::Format));                                                                                  \
	static_assert(std::make_tuple<>(BlockDimX, BlockDimY, BlockDimZ) == getBlockDimensions(PixelFormat::Format));                                        \
	static_assert(ImgSize8x8 == computeImageSizeBytes(PixelFormat::Format, 8, 8, 1));                                                                    \
	static_assert(ImgSize9x9 == computeImageSizeBytes(PixelFormat::Format, 9, 9, 1));                                                                    \
	static_assert(ImgSize2x5 == computeImageSizeBytes(PixelFormat::Format, 2, 5, 1));                                                                    \
	static_assert(PixelFormat::Format == getPixelFormat(BaseType::Type, Channels, Nrm, sRGB, static_cast<uint8_t>(InternalType::Internal)));

TEST_PIXEL_FORMAT(R8UInt          , Standard , 0, 0, 1, UInt8  , 1 * 1, 1, 1, 1, 1 * 1 * 8 * 8, 1 * 1 * 9 * 9, 1 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RG8UInt         , Standard , 0, 0, 2, UInt8  , 2 * 1, 1, 1, 1, 2 * 1 * 8 * 8, 2 * 1 * 9 * 9, 2 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGB8UInt        , Standard , 0, 0, 3, UInt8  , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA8UInt       , Standard , 0, 0, 4, UInt8  , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(R8UNorm         , Standard , 0, 1, 1, UInt8  , 1 * 1, 1, 1, 1, 1 * 1 * 8 * 8, 1 * 1 * 9 * 9, 1 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RG8UNorm        , Standard , 0, 1, 2, UInt8  , 2 * 1, 1, 1, 1, 2 * 1 * 8 * 8, 2 * 1 * 9 * 9, 2 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGB8UNorm       , Standard , 0, 1, 3, UInt8  , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA8UNorm      , Standard , 0, 1, 4, UInt8  , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(sR8UNorm        , Standard , 1, 1, 1, UInt8  , 1 * 1, 1, 1, 1, 1 * 1 * 8 * 8, 1 * 1 * 9 * 9, 1 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(sRG8UNorm       , Standard , 1, 1, 2, UInt8  , 2 * 1, 1, 1, 1, 2 * 1 * 8 * 8, 2 * 1 * 9 * 9, 2 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(sRGB8UNorm      , Standard , 1, 1, 3, UInt8  , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(sRGBA8UNorm     , Standard , 1, 1, 4, UInt8  , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(R8SInt          , Standard , 0, 0, 1, Int8   , 1 * 1, 1, 1, 1, 1 * 1 * 8 * 8, 1 * 1 * 9 * 9, 1 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RG8SInt         , Standard , 0, 0, 2, Int8   , 2 * 1, 1, 1, 1, 2 * 1 * 8 * 8, 2 * 1 * 9 * 9, 2 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGB8SInt        , Standard , 0, 0, 3, Int8   , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA8SInt       , Standard , 0, 0, 4, Int8   , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(R8SNorm         , Standard , 0, 1, 1, Int8   , 1 * 1, 1, 1, 1, 1 * 1 * 8 * 8, 1 * 1 * 9 * 9, 1 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RG8SNorm        , Standard , 0, 1, 2, Int8   , 2 * 1, 1, 1, 1, 2 * 1 * 8 * 8, 2 * 1 * 9 * 9, 2 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGB8SNorm       , Standard , 0, 1, 3, Int8   , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA8SNorm      , Standard , 0, 1, 4, Int8   , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)

TEST_PIXEL_FORMAT(R16UInt         , Standard , 0, 0, 1, UInt16 , 1 * 2, 1, 1, 1, 1 * 2 * 8 * 8, 1 * 2 * 9 * 9, 1 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RG16UInt        , Standard , 0, 0, 2, UInt16 , 2 * 2, 1, 1, 1, 2 * 2 * 8 * 8, 2 * 2 * 9 * 9, 2 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGB16UInt       , Standard , 0, 0, 3, UInt16 , 3 * 2, 1, 1, 1, 3 * 2 * 8 * 8, 3 * 2 * 9 * 9, 3 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA16UInt      , Standard , 0, 0, 4, UInt16 , 4 * 2, 1, 1, 1, 4 * 2 * 8 * 8, 4 * 2 * 9 * 9, 4 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(R16UNorm        , Standard , 0, 1, 1, UInt16 , 1 * 2, 1, 1, 1, 1 * 2 * 8 * 8, 1 * 2 * 9 * 9, 1 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RG16UNorm       , Standard , 0, 1, 2, UInt16 , 2 * 2, 1, 1, 1, 2 * 2 * 8 * 8, 2 * 2 * 9 * 9, 2 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGB16UNorm      , Standard , 0, 1, 3, UInt16 , 3 * 2, 1, 1, 1, 3 * 2 * 8 * 8, 3 * 2 * 9 * 9, 3 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA16UNorm     , Standard , 0, 1, 4, UInt16 , 4 * 2, 1, 1, 1, 4 * 2 * 8 * 8, 4 * 2 * 9 * 9, 4 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(R16SInt         , Standard , 0, 0, 1, Int16  , 1 * 2, 1, 1, 1, 1 * 2 * 8 * 8, 1 * 2 * 9 * 9, 1 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RG16SInt        , Standard , 0, 0, 2, Int16  , 2 * 2, 1, 1, 1, 2 * 2 * 8 * 8, 2 * 2 * 9 * 9, 2 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGB16SInt       , Standard , 0, 0, 3, Int16  , 3 * 2, 1, 1, 1, 3 * 2 * 8 * 8, 3 * 2 * 9 * 9, 3 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA16SInt      , Standard , 0, 0, 4, Int16  , 4 * 2, 1, 1, 1, 4 * 2 * 8 * 8, 4 * 2 * 9 * 9, 4 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(R16SNorm        , Standard , 0, 1, 1, Int16  , 1 * 2, 1, 1, 1, 1 * 2 * 8 * 8, 1 * 2 * 9 * 9, 1 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RG16SNorm       , Standard , 0, 1, 2, Int16  , 2 * 2, 1, 1, 1, 2 * 2 * 8 * 8, 2 * 2 * 9 * 9, 2 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGB16SNorm      , Standard , 0, 1, 3, Int16  , 3 * 2, 1, 1, 1, 3 * 2 * 8 * 8, 3 * 2 * 9 * 9, 3 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA16SNorm     , Standard , 0, 1, 4, Int16  , 4 * 2, 1, 1, 1, 4 * 2 * 8 * 8, 4 * 2 * 9 * 9, 4 * 2 * 2 * 5)

TEST_PIXEL_FORMAT(R32UInt         , Standard , 0, 0, 1, UInt32 , 1 * 4, 1, 1, 1, 1 * 4 * 8 * 8, 1 * 4 * 9 * 9, 1 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RG32UInt        , Standard , 0, 0, 2, UInt32 , 2 * 4, 1, 1, 1, 2 * 4 * 8 * 8, 2 * 4 * 9 * 9, 2 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RGB32UInt       , Standard , 0, 0, 3, UInt32 , 3 * 4, 1, 1, 1, 3 * 4 * 8 * 8, 3 * 4 * 9 * 9, 3 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA32UInt      , Standard , 0, 0, 4, UInt32 , 4 * 4, 1, 1, 1, 4 * 4 * 8 * 8, 4 * 4 * 9 * 9, 4 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(R32SInt         , Standard , 0, 0, 1, Int32  , 1 * 4, 1, 1, 1, 1 * 4 * 8 * 8, 1 * 4 * 9 * 9, 1 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RG32SInt        , Standard , 0, 0, 2, Int32  , 2 * 4, 1, 1, 1, 2 * 4 * 8 * 8, 2 * 4 * 9 * 9, 2 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RGB32SInt       , Standard , 0, 0, 3, Int32  , 3 * 4, 1, 1, 1, 3 * 4 * 8 * 8, 3 * 4 * 9 * 9, 3 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA32SInt      , Standard , 0, 0, 4, Int32  , 4 * 4, 1, 1, 1, 4 * 4 * 8 * 8, 4 * 4 * 9 * 9, 4 * 4 * 2 * 5)

TEST_PIXEL_FORMAT(R64UInt         , Standard , 0, 0, 1, UInt64 , 1 * 8, 1, 1, 1, 1 * 8 * 8 * 8, 1 * 8 * 9 * 9, 1 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RG64UInt        , Standard , 0, 0, 2, UInt64 , 2 * 8, 1, 1, 1, 2 * 8 * 8 * 8, 2 * 8 * 9 * 9, 2 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RGB64UInt       , Standard , 0, 0, 3, UInt64 , 3 * 8, 1, 1, 1, 3 * 8 * 8 * 8, 3 * 8 * 9 * 9, 3 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA64UInt      , Standard , 0, 0, 4, UInt64 , 4 * 8, 1, 1, 1, 4 * 8 * 8 * 8, 4 * 8 * 9 * 9, 4 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(R64SInt         , Standard , 0, 0, 1, Int64  , 1 * 8, 1, 1, 1, 1 * 8 * 8 * 8, 1 * 8 * 9 * 9, 1 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RG64SInt        , Standard , 0, 0, 2, Int64  , 2 * 8, 1, 1, 1, 2 * 8 * 8 * 8, 2 * 8 * 9 * 9, 2 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RGB64SInt       , Standard , 0, 0, 3, Int64  , 3 * 8, 1, 1, 1, 3 * 8 * 8 * 8, 3 * 8 * 9 * 9, 3 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA64SInt      , Standard , 0, 0, 4, Int64  , 4 * 8, 1, 1, 1, 4 * 8 * 8 * 8, 4 * 8 * 9 * 9, 4 * 8 * 2 * 5)

TEST_PIXEL_FORMAT(R16SFloat       , Standard , 0, 0, 1, Float16, 1 * 2, 1, 1, 1, 1 * 2 * 8 * 8, 1 * 2 * 9 * 9, 1 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RG16SFloat      , Standard , 0, 0, 2, Float16, 2 * 2, 1, 1, 1, 2 * 2 * 8 * 8, 2 * 2 * 9 * 9, 2 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGB16SFloat     , Standard , 0, 0, 3, Float16, 3 * 2, 1, 1, 1, 3 * 2 * 8 * 8, 3 * 2 * 9 * 9, 3 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA16SFloat    , Standard , 0, 0, 4, Float16, 4 * 2, 1, 1, 1, 4 * 2 * 8 * 8, 4 * 2 * 9 * 9, 4 * 2 * 2 * 5)
TEST_PIXEL_FORMAT(R32SFloat       , Standard , 0, 0, 1, Float32, 1 * 4, 1, 1, 1, 1 * 4 * 8 * 8, 1 * 4 * 9 * 9, 1 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RG32SFloat      , Standard , 0, 0, 2, Float32, 2 * 4, 1, 1, 1, 2 * 4 * 8 * 8, 2 * 4 * 9 * 9, 2 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RGB32SFloat     , Standard , 0, 0, 3, Float32, 3 * 4, 1, 1, 1, 3 * 4 * 8 * 8, 3 * 4 * 9 * 9, 3 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA32SFloat    , Standard , 0, 0, 4, Float32, 4 * 4, 1, 1, 1, 4 * 4 * 8 * 8, 4 * 4 * 9 * 9, 4 * 4 * 2 * 5)
TEST_PIXEL_FORMAT(R64SFloat       , Standard , 0, 0, 1, Float64, 1 * 8, 1, 1, 1, 1 * 8 * 8 * 8, 1 * 8 * 9 * 9, 1 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RG64SFloat      , Standard , 0, 0, 2, Float64, 2 * 8, 1, 1, 1, 2 * 8 * 8 * 8, 2 * 8 * 9 * 9, 2 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RGB64SFloat     , Standard , 0, 0, 3, Float64, 3 * 8, 1, 1, 1, 3 * 8 * 8 * 8, 3 * 8 * 9 * 9, 3 * 8 * 2 * 5)
TEST_PIXEL_FORMAT(RGBA64SFloat    , Standard , 0, 0, 4, Float64, 4 * 8, 1, 1, 1, 4 * 8 * 8 * 8, 4 * 8 * 9 * 9, 4 * 8 * 2 * 5)

TEST_PIXEL_FORMAT(BGR8UInt        , BGRA     , 0, 0, 3, UInt8  , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(BGRA8UInt       , BGRA     , 0, 0, 4, UInt8  , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(BGR8UNorm       , BGRA     , 0, 1, 3, UInt8  , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(BGRA8UNorm      , BGRA     , 0, 1, 4, UInt8  , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(sBGR8UNorm      , BGRA     , 1, 1, 3, UInt8  , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(sBGRA8UNorm     , BGRA     , 1, 1, 4, UInt8  , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(BGR8SInt        , BGRA     , 0, 0, 3, Int8   , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(BGRA8SInt       , BGRA     , 0, 0, 4, Int8   , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(BGR8SNorm       , BGRA     , 0, 1, 3, Int8   , 3 * 1, 1, 1, 1, 3 * 1 * 8 * 8, 3 * 1 * 9 * 9, 3 * 1 * 2 * 5)
TEST_PIXEL_FORMAT(BGRA8SNorm      , BGRA     , 0, 1, 4, Int8   , 4 * 1, 1, 1, 1, 4 * 1 * 8 * 8, 4 * 1 * 9 * 9, 4 * 1 * 2 * 5)

TEST_PIXEL_FORMAT(B5G6R5UNorm     , B5G6R5   , 0, 1, 3, UInt8  , 2    , 1, 1, 1, 2 * 8 * 8    , 2 * 9 * 9    , 2 * 2 * 5)
TEST_PIXEL_FORMAT(BGR5A1UNorm     , BGR5A1   , 0, 1, 4, UInt8  , 2    , 1, 1, 1, 2 * 8 * 8    , 2 * 9 * 9    , 2 * 2 * 5)
TEST_PIXEL_FORMAT(R11G11B10SFloat , R11G11B10, 0, 0, 3, Float16, 4    , 1, 1, 1, 4 * 8 * 8    , 4 * 9 * 9    , 4 * 2 * 5)
TEST_PIXEL_FORMAT(RGB10A2UNorm    , RGB10A2  , 0, 1, 4, UInt8  , 4    , 1, 1, 1, 4 * 8 * 8    , 4 * 9 * 9    , 4 * 2 * 5)

TEST_PIXEL_FORMAT(D16UNorm        , Depth    , 0, 1, 1, UInt16 , 2    , 1, 1, 1, 2 * 8 * 8    , 2 * 9 * 9    , 2 * 2 * 5)
TEST_PIXEL_FORMAT(D24S8UNorm      , Depth    , 0, 1, 2, UInt32 , 4    , 1, 1, 1, 4 * 8 * 8    , 4 * 9 * 9    , 4 * 2 * 5)
TEST_PIXEL_FORMAT(D32SFloat       , Depth    , 0, 0, 1, Float32, 4    , 1, 1, 1, 4 * 8 * 8    , 4 * 9 * 9    , 4 * 2 * 5)
TEST_PIXEL_FORMAT(D32S8SFloat     , Depth    , 0, 0, 2, Float32, 5    , 1, 1, 1, 5 * 8 * 8    , 5 * 9 * 9    , 5 * 2 * 5)

TEST_PIXEL_FORMAT(RGB8UNormBC1    , BC1      , 0, 1, 3, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(sRGB8UNormBC1   , BC1      , 1, 1, 3, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(RGB8A1UNormBC1  , BC1      , 0, 1, 4, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(sRGB8A1UNormBC1 , BC1      , 1, 1, 4, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(RGB8A4UNormBC2  , BC2      , 0, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(sRGB8A4UNormBC2 , BC2      , 1, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(RGBA8UNormBC3   , BC3      , 0, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(sRGBA8UNormBC3  , BC3      , 1, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(R8UNormBC4      , BC4      , 0, 1, 1, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(R8SNormBC4      , BC4      , 0, 1, 1, Int8   , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(RG8UNormBC5     , BC5      , 0, 1, 2, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(RG8SNormBC5     , BC5      , 1, 1, 2, Int8   , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(RGB16SFloatBC6H , BC6H     , 1, 0, 3, Float32, 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(RGBA8UNormBC7   , BC7      , 0, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(sRGBA8UNormBC7  , BC7      , 1, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(RGB8UNormETC2   , ETC2     , 0, 1, 3, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(sRGB8UNormETC2  , ETC2     , 1, 1, 3, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(RGB8A1UNormETC2 , ETC2A    , 0, 1, 4, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(sRGB8A1UNormETC2, ETC2A    , 1, 1, 4, UInt8  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(RGBA8UNormETC2  , ETC2     , 0, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(sRGBA8UNormETC2 , ETC2     , 1, 1, 4, UInt8  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(R11UIntEAC      , EAC      , 0, 0, 1, UInt16 , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(R11SIntEAC      , EAC      , 0, 0, 1, Int16  , 8    , 4, 4, 1, 8 * 4        , 8 * 9        , 8 * 2)
TEST_PIXEL_FORMAT(RG11UIntEAC     , EAC      , 0, 0, 2, UInt16 , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)
TEST_PIXEL_FORMAT(RG11SIntEAC     , EAC      , 0, 0, 2, Int16  , 16   , 4, 4, 1, 16 * 4       , 16 * 9       , 16 * 2)

} // namespace Util
