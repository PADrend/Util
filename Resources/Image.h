/*
	This file is part of the Platform for Algorithm Development and Rendering (PADrend).
	Web page: http://www.padrend.de/
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2014-2022 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_IMAGE_H_
#define UTIL_RESOURCES_IMAGE_H_

#include "Resource.h"
#include "AttributeFormat.h"
#include "../Graphics/PixelFormat.h"

namespace Util {

class Image;
using ImageHandle = Reference<Image>;

enum class ImageDimensions : uint8_t {
	Unknown = 0,
	Image1D,
	Image1DArray,
	Image2D,
	Image2DArray,
	ImageCube,
	ImageCubeArray,
	Image2DMS,
	Image2DMSArray,
	Image3D
};

struct ImageConfig {
	uint32_t width = 1;
	uint32_t height = 1;
	uint32_t depth = 1;
	uint32_t arraySize = 1;
	uint32_t mipLevels = 1;
	uint32_t sampleCount = 1;
	ImageDimensions dimensions = ImageDimensions::Image2D;
	PixelFormat format = PixelFormat::RGBA8UNorm;
	std::string debugName;
};

struct ImageRegion {
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t z = 0;
	uint32_t width = ~0ul; // ~0ul means entire dimension of the region
	uint32_t height = ~0ul;
	uint32_t depth = ~0ul;
	uint32_t mipLevel = 0;
	uint32_t arraySlice = 0;

	constexpr ImageRegion& setOrigin(uint32_t _x=0, uint32_t _y=0, uint32_t _z=0) { x = _x; y = _y; z = _z; return *this; }
	constexpr ImageRegion& setSize(uint32_t _width=~0ul, uint32_t _height=~0ul, uint32_t _depth=~0ul) { width = _width; height = _height; depth = _depth; return *this; }
	constexpr ImageRegion& setX(uint32_t _x) { x = _x; return *this; }
	constexpr ImageRegion& setY(uint32_t _y) { y = _y; return *this; }
	constexpr ImageRegion& setZ(uint32_t _z) { z = _z; return *this; }
	constexpr ImageRegion& setWidth(uint32_t _width) { width = _width; return *this; }
	constexpr ImageRegion& setHeight(uint32_t _height) { height = _height; return *this; }
	constexpr ImageRegion& setDepth(uint32_t _depth) { depth = _depth; return *this; }
	constexpr ImageRegion& setMipLevel(uint32_t _mipLevel) { mipLevel = _mipLevel; return *this; }
	constexpr ImageRegion& setArraySlice(uint32_t _arraySlice) { arraySlice = _arraySlice; return *this; }

	constexpr bool operator==(const ImageRegion& other) const { return x == other.x && y == other.y && z == other.z && width == other.width && height == other.height && depth == other.depth && mipLevel == other.mipLevel && arraySlice == other.arraySlice; }

	constexpr static ImageRegion resolve(const ImageConfig& config, uint32_t arraySlice=0, uint32_t mipLevel=0) { return ImageRegion{0,0,0,config.width>>mipLevel,config.height>>mipLevel,config.depth>>mipLevel,0,0}; }
	static ImageRegion resolve(const ImageHandle& image, uint32_t arraySlice=0, uint32_t mipLevel=0);
	
	uint64_t getByteSize(const ImageConfig& config) const {
		const auto w = (width == ~0ul ? (config.width>>mipLevel)-x : width);
		const auto h = (height == ~0ul ? (config.height>>mipLevel)-y : height);
		const auto d = (depth == ~0ul ? (config.depth>>mipLevel)-z : depth);
		return computeImageSizeBytes(config.format, w, h, d) * config.sampleCount;
	}

	bool isValid(const ImageConfig& config) const {
		const uint32_t iw = config.width>>mipLevel;
		const uint32_t ih = config.height>>mipLevel;
		const uint32_t id = config.depth>>mipLevel;
		const uint32_t w = (width == ~0ul ? iw-x : width);
		const uint32_t h = (height == ~0ul ? ih-y : height);
		const uint32_t d = (depth == ~0ul ? id-z : depth);
		return (x+w <= iw) && (x+h <= ih) && (x+d <= id) && mipLevel < config.mipLevels && arraySlice < config.arraySize;
	}
};

/**
 * @brief Represents a contiguous allocation of memory on a device.
 * 
 */
class Image : public Resource {
	PROVIDES_TYPE_NAME(Image)
protected:
	Image(const ImageConfig& config) : Resource(), config(config) {}
public:
	static ImageHandle create(const ImageConfig& config) { return new Image(config); }

	const ImageConfig config;
};

inline ImageRegion ImageRegion::resolve(const ImageHandle& image, uint32_t arraySlice, uint32_t mipLevel) { return image ? resolve(image->config, arraySlice, mipLevel) : ImageRegion{0,0,0,0,0,0,0,0}; }

} // Util

#endif // UTIL_RESOURCES_IMAGE_H_