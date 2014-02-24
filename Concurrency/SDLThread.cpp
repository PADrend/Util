/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_SDL2

#include "SDLThread.h"
#include "../Macros.h"
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL.h>
#include <SDL_thread.h>
COMPILER_WARN_POP

namespace Util {
namespace Concurrency {

SDLThread::SDLThread() : Thread(), thread(nullptr) {
}

SDLThread::~SDLThread() = default;

//! Structure used by the wrapper function.
struct SDLThreadWrapper_t {
	void * (*userFunction)(void *);
	void * userData;
};

//! Wrapper function, which calls the user function of the Thread.
static int RunSDLThread(void * wrapperData) {
	SDLThreadWrapper_t * wrapper = static_cast<SDLThreadWrapper_t *>(wrapperData);
	wrapper->userFunction(wrapper->userData);
	delete wrapper;
	return 0;
}

bool SDLThread::start(void * (*run)(void *), void * data) {
	auto wrapper = new SDLThreadWrapper_t;
	wrapper->userFunction = run;
	wrapper->userData = data;
	thread = SDL_CreateThread(RunSDLThread, nullptr, wrapper);
	return true;
}

bool SDLThread::join() {
	SDL_WaitThread(thread, nullptr);
	thread = nullptr;
	return true;
}

}
}

#endif /* UTIL_HAVE_LIB_SDL2 */
