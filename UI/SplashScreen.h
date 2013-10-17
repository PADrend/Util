/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef SPLASHSCREEN_H_
#define SPLASHSCREEN_H_

#include "../Concurrency/UserThread.h"
#include "../References.h"
#include <memory>
#include <string>

namespace Util {
// Forward declarations.
class Bitmap;
namespace Concurrency {
class Mutex;
}

namespace UI {

/**
 * Abstract base class for all splash screens.
 *
 * @author Benjamin Eikel
 * @date 2010-08-23
 */
class SplashScreen : public Concurrency::UserThread {
	public:
		//! Destroy the splash screen and free the allocated resources.
		virtual ~SplashScreen();

		//! Display a message on the splash screen.
		virtual void showMessage(const std::string & message) = 0;

		//! Remove an existing message from the splash screen.
		virtual void removeMessage() = 0;

	protected:
		//! Storage of error message.
		std::string errorMessage;

		//! Thread status
		enum status_t {
			UNDEFINED_STATUS,
			ERROR_STATUS,
			RUNNING_STATUS,
			CLOSING_STATUS
		};

		//! Query the current thread status.
		status_t getStatus() const;

		//! Set the current thread status.
		void setStatus(status_t newStatus);

		//! Set internal variables.
		SplashScreen();

		//! Allow access to members from factory.
		friend SplashScreen * createSplashScreen(const std::string & splashTitle, const Reference<Bitmap> & splashImage);

	private:
		SplashScreen(const SplashScreen &) = delete;
		SplashScreen(SplashScreen &&) = delete;
		SplashScreen & operator=(const SplashScreen &) = delete;
		SplashScreen & operator=(SplashScreen &&) = delete;

		//! Guarantee mutal exclusion for @a status.
		std::unique_ptr<Concurrency::Mutex> statusMutex;

		//! Status needed for threading.
		status_t status;
};

}
}

#endif /* SPLASHSCREEN_H_ */
