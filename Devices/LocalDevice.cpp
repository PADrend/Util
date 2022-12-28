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
#include "LocalDevice.h"
#include "../Resources/Buffer.h"
#include "../Graphics/Bitmap.h"
#include "../Macros.h"
#include "../StringUtils.h"

namespace Util {
namespace {
std::pair<uint32_t, uint32_t> computeBitmapDimensions(const ImageConfig& config, uint32_t mipLevel=0) {
	ImageRegion region = ImageRegion::resolve(config, 0, mipLevel);
	uint32_t width = region.width;
	uint32_t height = region.height * region.depth * region.arraySlice * config.sampleCount;
	return {width, height};
}
} //

//---------------

struct LocalDevice::InternalBufferResource : InternalResource {
	InternalBufferResource(const BufferHandle& buffer) : InternalResource(buffer) {}
	std::vector<uint8_t> data;
};

//---------------

struct LocalDevice::InternalImageResource : InternalResource {
	InternalImageResource(const ImageHandle& image) : InternalResource(image) {}
	std::vector<Reference<Bitmap>> mips;

};

//---------------

Reference<LocalDevice> LocalDevice::get() {
	static Reference<LocalDevice> defaultDevice = new LocalDevice;
	return defaultDevice;
}


//---------------

void LocalDevice::shutdown() {
	if(get() == this)
		return; // keep default device alive
	Device::shutdown();
}

//---------------

void LocalDevice::allocateBuffer(BufferHandle buffer, const uint8_t* data) {
	if(!buffer)
		return;
	if (!ownsResource(buffer)) {
		auto resource = std::make_unique<InternalBufferResource>(buffer);
		resource->data.resize(buffer->config.byteSize);
		takeOwnership(buffer, std::move(resource));
	}
	if(data) {
		uploadBuffer(buffer, BufferRange::resolve(buffer), data);
	}
}

//---------------

bool LocalDevice::uploadBuffer(BufferHandle buffer, const BufferRange& range, const uint8_t* data) {
	if(!buffer)
		return false;
	if (!ownsResource(buffer))
		allocateBuffer(buffer);
	WARN_AND_RETURN_IF(buffer->config.byteSize < range.byteOffset+range.byteSize, StringUtils::format("Failed to upload buffer %d. invalid range %d > %d+%d", buffer->getId(), range.byteOffset,range.byteSize, buffer->config.byteSize), false);
	auto resource = static_cast<InternalBufferResource*>(getInternalRepresentation(buffer));
	std::copy(data, data + range.byteSize, resource->data.data() + range.byteOffset);
	// TODO: notify other devices of change
	// TODO: how to handle persitent mapped buffers of GPU?
	return true;
}

//---------------

bool LocalDevice::downloadBuffer(BufferHandle buffer, const BufferRange& range, uint8_t* data) const {
	if(!buffer || !ownsResource(buffer))
		return false;
	WARN_AND_RETURN_IF(buffer->config.byteSize < range.byteOffset+range.byteSize, StringUtils::format("Failed to download buffer %d. invalid range %d > %d+%d", buffer->getId(), range.byteOffset,range.byteSize, buffer->config.byteSize), false);
	auto resource = static_cast<InternalBufferResource*>(getInternalRepresentation(buffer));
	std::copy(resource->data.data() + range.byteOffset, resource->data.data() + range.byteOffset + range.byteSize, data);
	return true;
}

//---------------

void LocalDevice::releaseResource(InternalResource* resource) {
	if (!resource)
		return;
	// nothing to do
}

//---------------

void LocalDevice::allocateImage(ImageHandle image) {
	if(!image)
		return;
	if (!ownsResource(image)) {
		WARN_AND_RETURN_IF(image->config.sampleCount > 1, StringUtils::format("Failed to allocate image %d. Multiple samples are currently not supported.", image->getId()),);
		auto resource = std::make_unique<InternalImageResource>(image);
		resource->mips.resize(image->config.mipLevels);
		// only allocate base level & allocate other levels lazily
		auto [width, height] = computeBitmapDimensions(image->config, 0);
		resource->mips.front() = new Bitmap(width, height, image->config.format);
		takeOwnership(image, std::move(resource));
	}
}

//---------------

bool LocalDevice::uploadImage(ImageHandle image, const ImageRegion& region, Reference<Bitmap> bitmap) {
	if(!image || !ownsResource(image) || !bitmap)
		return false;
	const auto& config = image->config;
	uint64_t regionSize = region.getByteSize(image->config);
	WARN_AND_RETURN_IF(config.sampleCount > 1, StringUtils::format("Failed to upload image %d. Multiple samples are currently not supported.", image->getId()), false);
	WARN_AND_RETURN_IF(!region.isValid(image->config), StringUtils::format("Failed to upload image %d. Invalid region.", image->getId()), false);
	WARN_AND_RETURN_IF(bitmap->getDataSize() != regionSize, StringUtils::format("Failed to upload image %d. Invalid bitmap size %d != %d.", image->getId(), bitmap->getDataSize(), regionSize), false);
	WARN_AND_RETURN_IF(bitmap->getPixelFormat() != image->config.format, StringUtils::format("Failed to upload image %d. Invalid bitmap format.", image->getId()), false);
	auto resource = static_cast<InternalImageResource*>(getInternalRepresentation(image));
	WARN_AND_RETURN_IF(resource->mips.size() < region.mipLevel, StringUtils::format("Failed to upload image %d. Invalid mip level %d.", image->getId(), region.mipLevel), false);
	// TODO: handle compressed images
	if(resource->mips[region.mipLevel].isNull()) {
		auto [width, height] = computeBitmapDimensions(image->config, region.mipLevel);
		resource->mips[region.mipLevel] = new Bitmap(width, height, image->config.format);
	}
	auto tgtBitmap = resource->mips[region.mipLevel];
	uint32_t pixelByteSize = config.format.getDataSize();
	uint32_t startSlice = region.z * config.sampleCount + region.arraySlice * std::max(1u, config.depth>>region.mipLevel);
	uint64_t tgtRowByteSize = std::max(1u, config.width>>region.mipLevel) * pixelByteSize;
	uint64_t tgtSliceByteSize = std::max(1u, config.height>>region.mipLevel) * tgtRowByteSize;
	uint64_t tgtRowByteOffset = region.x * pixelByteSize;
	uint64_t tgtColByteOffset = region.y * tgtRowByteSize;
	uint64_t tgtSliceByteOffset = startSlice * tgtSliceByteSize;
	uint64_t srcRowByteSize = bitmap->getWidth() * pixelByteSize;
	uint32_t rowsPerSlice = region.height == ~0ul ? std::max(1u, config.height>>region.mipLevel)-region.y : region.height;
	uint32_t sliceCount = (region.depth == ~0ul ? std::max(1u, config.depth>>region.mipLevel)-region.z : region.depth) * config.sampleCount;

	uint8_t* tgtPtr = tgtBitmap->data() + tgtSliceByteOffset + tgtColByteOffset + tgtRowByteOffset;
	uint8_t* srcPtr = bitmap->data();
	// copy row-by-row from tgt image to bitmap
	for(uint32_t slice=0; slice<sliceCount; ++slice) {
		for(uint32_t row=0; row<rowsPerSlice; ++row) {
			std::copy(srcPtr, srcPtr + srcRowByteSize, tgtPtr);
			tgtPtr += tgtRowByteSize;
			srcPtr += srcRowByteSize;
		}
		tgtPtr += tgtSliceByteOffset - tgtRowByteSize * rowsPerSlice;
	}
	return true;
}

//---------------

bool LocalDevice::downloadImage(ImageHandle image, const ImageRegion& region, Reference<Bitmap> bitmap) {
	if(!image || !ownsResource(image) || !bitmap)
		return false;
	const auto& config = image->config;
	uint64_t regionSize = region.getByteSize(image->config);
	WARN_AND_RETURN_IF(config.sampleCount > 1, StringUtils::format("Failed to download image %d. Multiple samples are currently not supported.", image->getId()), false);
	WARN_AND_RETURN_IF(!region.isValid(image->config), StringUtils::format("Failed to download image %d. Invalid region.", image->getId()), false);
	WARN_AND_RETURN_IF(bitmap->getDataSize() != regionSize, StringUtils::format("Failed to download image %d. Invalid bitmap size %d != %d.", image->getId(), bitmap->getDataSize(), regionSize), false);
	WARN_AND_RETURN_IF(bitmap->getPixelFormat() != image->config.format, StringUtils::format("Failed to download image %d. Invalid bitmap format.", image->getId()), false);
	auto resource = static_cast<InternalImageResource*>(getInternalRepresentation(image));
	WARN_AND_RETURN_IF(resource->mips.size() < region.mipLevel || resource->mips[region.mipLevel].isNull(), StringUtils::format("Failed to download image %d. Mip level %d invalid or not allocated.", image->getId(), region.mipLevel), false);	
	// TODO: handle compressed images

	auto srcBitmap = resource->mips[region.mipLevel];
	uint32_t pixelByteSize = config.format.getDataSize();
	uint32_t startSlice = region.z * config.sampleCount + region.arraySlice * std::max(1u, config.depth>>region.mipLevel);
	uint64_t srcRowByteSize = std::max(1u, config.width>>region.mipLevel) * pixelByteSize;
	uint64_t srcSliceByteSize = std::max(1u, config.height>>region.mipLevel) * srcRowByteSize;
	uint64_t srcRowByteOffset = region.x * pixelByteSize;
	uint64_t srcColByteOffset = region.y * srcRowByteSize;
	uint64_t srcSliceByteOffset = startSlice * srcSliceByteSize;
	uint64_t tgtRowByteSize = bitmap->getWidth() * pixelByteSize;
	uint32_t rowsPerSlice = region.height == ~0ul ? std::max(1u, config.height>>region.mipLevel)-region.y : region.height;
	uint32_t sliceCount = (region.depth == ~0ul ? std::max(1u, config.depth>>region.mipLevel)-region.z : region.depth) * config.sampleCount;

	uint8_t* srcPtr = srcBitmap->data() + srcSliceByteOffset + srcColByteOffset + srcRowByteOffset;
	uint8_t* tgtPtr = bitmap->data();
	// copy row-by-row from src image to bitmap
	for(uint32_t slice=0; slice<sliceCount; ++slice) {
		for(uint32_t row=0; row<rowsPerSlice; ++row) {
			std::copy(srcPtr, srcPtr + tgtRowByteSize, tgtPtr);
			srcPtr += srcRowByteSize;
			tgtPtr += tgtRowByteSize;
		}
		srcPtr += srcSliceByteOffset - srcRowByteSize * rowsPerSlice;
	}
	return true;
}

//---------------

} // namespace Util