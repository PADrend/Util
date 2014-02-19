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

#include "SDLSemaphore.h"
#include "../Macros.h"
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL.h>
#include <SDL_thread.h>
COMPILER_WARN_POP

namespace Util {
namespace Concurrency {

SDLSemaphore::SDLSemaphore() :
	Semaphore() {
	sem = SDL_CreateSemaphore(0);
}

SDLSemaphore::~SDLSemaphore() {
	SDL_DestroySemaphore(sem);
}

bool SDLSemaphore::wait() {
	return (SDL_SemWait(sem) == 0);
}

bool SDLSemaphore::tryWait() {
	return (SDL_SemTryWait(sem) == 0);
}

bool SDLSemaphore::post() {
	return (SDL_SemPost(sem) == 0);
}

uint32_t SDLSemaphore::getValue() {
	return SDL_SemValue(sem);
}

}
}

#endif /* UTIL_HAVE_LIB_SDL2 */
