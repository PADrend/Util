/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "SplashScreen.h"
#include "../Concurrency/Concurrency.h"
#include "../Concurrency/Lock.h"
#include "../Concurrency/Mutex.h"

namespace Util {
namespace UI {

SplashScreen::SplashScreen() : Concurrency::UserThread(), errorMessage(), statusMutex(Concurrency::createMutex()), status(UNDEFINED_STATUS) {
}

SplashScreen::~SplashScreen() = default;

SplashScreen::status_t SplashScreen::getStatus() const {
	auto lock = Concurrency::createLock(*statusMutex);
	return status;
}

void SplashScreen::setStatus(SplashScreen::status_t newStatus) {
	auto lock = Concurrency::createLock(*statusMutex);
	status = newStatus;
}

}
}
