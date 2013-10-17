/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_LIB_X11)

#ifndef SPLASHSCREENX11_H_
#define SPLASHSCREENX11_H_

#include "SplashScreen.h"
#include <memory>
#include <string>

namespace Util {
class Bitmap;
namespace UI {

/**
 * Class to create a splash screen using X11.
 *
 * @author Benjamin Eikel
 * @date 2010-08-23
 */
class SplashScreenX11 : public SplashScreen {
	public:
		//! Destroy the splash screen and free the allocated resources.
		virtual ~SplashScreenX11();

		//! Display a message on the splash screen.
		void showMessage(const std::string & message) override;

		//! Remove an existing message from the splash screen.
		void removeMessage() override;

	protected:
		//! ---|> UserThread
		void run() override;

		//! Create a new splash screen.
		SplashScreenX11(const std::string & splashTitle, const Reference<Bitmap> & splashImage);

		//! Allow access to constructor from factory.
		friend SplashScreen * createSplashScreen(const std::string & splashTitle, const Reference<Bitmap> & splashImage);

	private:
		//! Structure used to decouple declaration and definition of attributes.
		struct SplashScreenData;

		//! Storage of attributes.
		std::unique_ptr<SplashScreenData> data;
};

}
}

#endif /* SPLASHSCREENX11_H_ */

#endif /* defined(UTIL_HAVE_LIB_X11) */
