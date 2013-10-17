/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "DummyMutex.h"
#include "../Macros.h"

namespace Util {
namespace Concurrency {

DummyMutex::DummyMutex() : Mutex() {
	WARN("DummyMutex created.");
}

DummyMutex::~DummyMutex() {
}

bool DummyMutex::lock() {
	return false;
}

bool DummyMutex::unlock() {
	return false;
}

}
}
