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

#include "SDLMutex.h"
#include "../Macros.h"
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL.h>
#include <SDL_thread.h>
COMPILER_WARN_POP

namespace Util {
namespace Concurrency {

SDLMutex::SDLMutex() :
	Mutex() {
	mutex = SDL_CreateMutex();
}

SDLMutex::~SDLMutex() {
	SDL_DestroyMutex(mutex);
}

bool SDLMutex::lock() {
	return (SDL_mutexP(mutex) == 0);
}

bool SDLMutex::unlock() {
	return (SDL_mutexV(mutex) == 0);
}

}
}

#endif /* UTIL_HAVE_LIB_SDL2 */
