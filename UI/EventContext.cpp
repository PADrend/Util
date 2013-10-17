/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "EventContext.h"
#include "Event.h"
#include "EventQueue.h"
#include <functional>
#include <unordered_set>

namespace Util {
namespace UI {

EventContext::EventContext() : eventQueue(), pressedKeys() {
	eventQueue.registerEventHandler(std::bind(&EventContext::handleKeyEvent, this, std::placeholders::_1));
}

bool EventContext::handleKeyEvent(const Event & event) {
	if(event.type == EVENT_KEYBOARD && event.keyboard.key != KEY_NONE) {
		if(event.keyboard.pressed) {
			pressedKeys.insert(event.keyboard.key);
		} else {
			pressedKeys.erase(event.keyboard.key);
		}
	}
	// Never consume an event
	return false;
}

bool EventContext::isKeyPressed(Key key) const {
	return pressedKeys.count(key) == 1;
}

}
}
