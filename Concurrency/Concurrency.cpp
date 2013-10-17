/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Concurrency.h"
#include "Mutex.h"
#include "Semaphore.h"
#include "SpinLock.h"
#include "Thread.h"

#include "DummyMutex.h"
#include "DummySemaphore.h"
#include "DummySpinLock.h"
#include "DummyThread.h"

#if not defined(__clang__) and not defined(_WIN32)
#include "CppMutex.h"
#include "CppSemaphore.h"
#include "CppThread.h"
#elif defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32)
#include "POSIXMutex.h"
#include "POSIXSemaphore.h"
#if not defined(ANDROID)
#include "POSIXSpinLock.h"
#endif
#include "POSIXThread.h"
#elif defined(UTIL_HAVE_LIB_SDL2)
#include "SDLMutex.h"
#include "SDLSemaphore.h"
#include "SDLThread.h"
#endif

namespace Util {
namespace Concurrency {

Mutex * createMutex() {
#if not defined(__clang__) and not defined(_WIN32)
	return new CppMutex;
#elif defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32)
	return new POSIXMutex;
#elif defined(UTIL_HAVE_LIB_SDL2)
	return new SDLMutex;
#else
	return new DummyMutex;
#endif
}

Semaphore * createSemaphore() {
#if not defined(__clang__) and not defined(_WIN32)
	return new CppSemaphore;
#elif defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32)
	return new POSIXSemaphore;
#elif defined(UTIL_HAVE_LIB_SDL2)
	return new SDLSemaphore;
#else
	return new DummySemaphore;
#endif
}

SpinLock * createSpinLock() {
#if not defined(__clang__) and not defined(_WIN32)
	return new DummySpinLock;
#elif defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32)
	return new POSIXSpinLock;
#else
	return new DummySpinLock;
#endif
}

Thread * createThread() {
#if not defined(__clang__) and not defined(_WIN32)
	return new CppThread;
#elif defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32)
	return new POSIXThread;
#elif defined(UTIL_HAVE_LIB_SDL2)
	return new SDLThread;
#else
	return new DummyThread;
#endif
}

}
}
