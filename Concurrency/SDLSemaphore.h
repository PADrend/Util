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

#ifndef UTIL_CONCURRENCY_SDLSEMAPHORE_H
#define UTIL_CONCURRENCY_SDLSEMAPHORE_H

#include "Semaphore.h"

// Forward declaration
struct SDL_semaphore;

namespace Util {
namespace Concurrency {

//! Semaphore implementation using SDL.
class SDLSemaphore : public Semaphore {
	public:
		//! Destructor
		virtual ~SDLSemaphore();

		bool wait() override;
		bool tryWait() override;
		bool post() override;
		uint32_t getValue() override;

	protected:
		//! Semaphore creation is done by the factory method @a createSemaphore().
		SDLSemaphore();

		//! Allow access to constructor from factory.
		friend Semaphore * createSemaphore();

	private:
		SDL_semaphore * sem;
};

}
}

#endif /* UTIL_CONCURRENCY_SDLSEMAPHORE_H */

#endif /* UTIL_HAVE_LIB_SDL2 */
