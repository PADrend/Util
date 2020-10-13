/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(_WIN32)

#ifndef SPLASHSCREENWIN_H_
#define SPLASHSCREENWIN_H_

// at least win2k: needed for some extended functions
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <windows.h>

#include "SplashScreen.h"
#include "../References.h"
#include <memory>
#include <thread>
#include <atomic>

namespace Util {
namespace UI {

/**
 * Class to create a splash screen using Windows.
 * ---|> SplashScreen
 */
class SplashScreenWin : public SplashScreen {
	public:
		//! Destroy the splash screen and free the allocated resources.
		virtual ~SplashScreenWin();

		//! ---|> SplashScreen (unimplemented)
		void showMessage(const std::string & /*message*/) override {
		}

		//! ---|> SplashScreen
		void removeMessage() override {
		}

	private:
		//! Check for events.
		UTILAPI void eventLoop();

		//! Create a new splash screen.
		UTILAPI SplashScreenWin(const std::string & splashTitle, const Reference<Bitmap> & splashImage);

		//! Allow access to constructor from factory.
		UTILAPI friend std::unique_ptr<SplashScreen> createSplashScreen(const std::string & splashTitle, const Reference<Bitmap> & splashImage);

		
		Reference<Bitmap> splashImage;
		std::atomic<bool> running;
		std::thread myThread;
		
};

}
}

#endif /* SPLASHSCREENWIN_H_ */

#endif /* defined(_WIN32) */
