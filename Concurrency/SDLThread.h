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

#ifndef UTIL_CONCURRENCY_SDLTHREAD_H
#define UTIL_CONCURRENCY_SDLTHREAD_H

#include "Thread.h"

// Forward declaration
struct SDL_Thread;

namespace Util {
namespace Concurrency {

//! Thread implementation using SDL.
class SDLThread : public Thread {
	public:
		//! Destructor
		virtual ~SDLThread();

		bool start(void * (*run)(void *), void * data) override;
		bool join() override;

	protected:
		//! Thread creation is done by the factory function @a createThread().
		SDLThread();

		//! Allow access to constructor from factory.
		friend Thread * createThread();

	private:
		SDL_Thread * thread;
};

}
}

#endif /* UTIL_CONCURRENCY_SDLTHREAD_H */

#endif /* UTIL_HAVE_LIB_SDL2 */
