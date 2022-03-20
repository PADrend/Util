/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UI_H_
#define UI_H_

#include "Cursor.h"
#include "SplashScreen.h"
#include "Window.h"
#include <memory>
#include <string>

namespace Util {
namespace UI {

/**
 * Factory function to create a new Cursor.
 * This will automatically select the preferred implementation of this class.
 *
 * @param image Bitmap that will be used as cursor image
 * @param hotSpotX Horizontal coordinate of the hot spot of the cursor
 * @param hotSpotY Vertical coordinate of the hot spot of the cursor
 * @return Object of preferred Cursor implementation. If no implementation was found or an error occurred, @c nullptr will be returned.
 */
UTILAPI std::unique_ptr<Cursor> createCursor(const Reference<Bitmap> & image, unsigned int hotSpotX, unsigned int hotSpotY);

/**
 * Factory function to create a new SplashScreen.
 * This will automatically select the preferred implementation of this class.
 *
 * @param splashTitle Window title for the splash screen.
 * @param splashImage Bitmap that will be displayed by the splash screen.
 * The bitmap is not copied and the pointer has to stay valid for the time the splash screen is displayed.
 * @return Object of preferred SplashScreen implementation. If no implementation was found or an error occurred, @c nullptr will be returned.
 */
UTILAPI std::unique_ptr<SplashScreen> createSplashScreen(const std::string & splashTitle, const Reference<Bitmap> & splashImage);

/**
 * Factory function to create a new Window.
 * This will automatically select the preferred implementation of this class.
 *
 * @param properties @see Window.h.
 * @return Object of preferred Window implementation. If no implementation was found or an error occurred, @c nullptr will be returned.
 */
UTILAPI Util::Reference<Window> createWindow(const Window::Properties & properties);

/**
 * Checks if Vulkan rendering is supported.
 */
UTILAPI bool isVulkanSupported();

/**
 * Checks if the given given queue family on a physical device supports presenting to a surface (Vulkan only).
 */
UTILAPI bool getPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice device, int32_t queueFamily);

/**
 * Returns the extensions needed to create a surface from an API instance (e.g., Vulkan).
 * @return A list of extension names.
 */
UTILAPI std::vector<const char*> getRequiredInstanceExtensions();
}
}

#endif /* UI_H_ */
