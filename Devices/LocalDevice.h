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
#ifndef UTIL_RESOURCES_LOCALDEVICE_H_
#define UTIL_RESOURCES_LOCALDEVICE_H_

#include "Device.h"
#include "../Resources/Buffer.h"
#include "../Resources/Image.h"

namespace Util {
class Bitmap;

/**
 * @brief Represents the local host device.
 * The local device represents the current host device that the system runs on.
 * An instance of this device is always present and can be queried by \code LocalDevice::get() \endcode.
 * 
 * Resources are typically stored in main memory.
 * A resource has to be transfered to this device to be able to directly read/write from/to it.
 */
class LocalDevice : public Device {
	PROVIDES_TYPE_NAME(LocalDevice)
public:
	//! returns the default local device
	static Reference<LocalDevice> get();

	/// ---|> [Device]
	void shutdown() override;
	
	//! @name Resources
	// @{
	
	//! allocates memory for the given buffer and optionally initializes it with data.
	void allocateBuffer(BufferHandle buffer, const uint8_t* data=nullptr);

	//! uploads data to the given buffer & allocates data for it if not owned by this device. Returns false on failure.
	bool uploadBuffer(BufferHandle buffer, const BufferRange& range, const uint8_t* data);

	//! copies a range of data from the given buffer into the given data pointer. Returns false on failure (e.g., if the buffer is not owned)
	bool downloadBuffer(BufferHandle buffer, const BufferRange& range, uint8_t* data) const;

	/**
	 * @brief Allocates memory for the given image.
	 * @note Only the base mip level is allocated. Other levels are allocated lazily when used.
	 */
	void allocateImage(ImageHandle image);
	
	/**
	 * Uploads image data to the given image & allocates data for it if not owned by this device. Returns false on failure.
	 * @note If the image is an image array or has multiple mip levels, only one can be uploaded at a time.
	 */
	bool uploadImage(ImageHandle image, const ImageRegion& region, Reference<Bitmap> bitmap);
	
	//! downloads a region of image data into the given bitmap. Returns false on failure (e.g., if the image is not owned)
	bool downloadImage(ImageHandle image, const ImageRegion& region, Reference<Bitmap> bitmap);

	// TODO: mapping/unmapping/accessors
protected:
	struct InternalBufferResource;
	struct InternalImageResource;
	
	void releaseResource(InternalResource* resource) override;
	// @}
private:
};

} // Util

#endif // UTIL_RESOURCES_LOCALDEVICE_H_