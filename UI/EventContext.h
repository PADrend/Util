/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_EVENTCONTEXT_H
#define UTIL_EVENTCONTEXT_H

#include "Event.h"
#include "EventQueue.h"
#include <cstdint>
#include <unordered_set>

namespace Util {
namespace UI {

/**
 * @brief Context to store event-related data
 *
 * The event context stores an event queue and event state like the currently
 * pressed keys.
 * 
 * @author Benjamin Eikel
 * @date 2012-09-26
 */
class EventContext {
	private:
		//! Event queue that is used by this context
		EventQueue eventQueue;

		//! Set of keys that are currently pressed
		std::unordered_set<Key> pressedKeys;

		//! Handler maintaining the @a pressedKeys set
		UTILAPI bool handleKeyEvent(const Event & event);
	public:
		//! Create a new event context and associate it with the event queue
		UTILAPI EventContext();

		//! Access to the associated event queue
		EventQueue & getEventQueue() {
			return eventQueue;
		}

		//! Return @c true if the given @p key is currently pressed
		UTILAPI bool isKeyPressed(Key key) const;

		//! Return @c true if the right or left alt key is pressed
		bool isAltPressed() const {
			return isKeyPressed(KEY_ALTL) || isKeyPressed(KEY_ALTR);
		}

		//! Return @c true if the right or left control key is pressed
		bool isCtrlPressed() const {
			return isKeyPressed(KEY_CONTROLL) || isKeyPressed(KEY_CONTROLR);
		}

		//! Return @c true if the right or left shift key is pressed
		bool isShiftPressed() const {
			return isKeyPressed(KEY_SHIFTL) || isKeyPressed(KEY_SHIFTR);
		}
};

}
}

#endif /* UTIL_EVENTCONTEXT_H */
