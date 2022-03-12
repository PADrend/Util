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
#ifndef UTIL_RESOURCES_DEVICE_H_
#define UTIL_RESOURCES_DEVICE_H_

#include <memory>

namespace Util {

using DeviceFamily = uint32_t; // TODO: make opaque handle

/**
 * @brief Basic representation of a device.
 * 
 * A Device manages resources and commands that use/modify these resources.
 * This can be a physical device (e.g., GPU, CPU) or virtual device (e.g., remote device).
 * Resources can be shared/transfered between devices and resource usage can be synchronized between them.
 */
class Device {
public:
	//! @name Resources
	// @{
	
	// @}

	//! @name Command execution
	// @{
	
	//! waits until all commands have been finished on this device.
	virtual void wait() const = 0;

	// @}
private:
	
};

} // Util

#endif // UTIL_RESOURCES_DEVICE_H_