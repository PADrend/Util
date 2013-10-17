/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "EventQueue.h"
#include "Event.h"
#include <deque>
#include <functional>

namespace Util {
namespace UI {

std::size_t EventQueue::getNumEventsAvailable() const {
	return events.size();
}

void EventQueue::pushEvent(const Event & newEvent) {
	events.emplace_back(newEvent);
}

const Event & EventQueue::peekEvent() const {
	return events.front();
}

Event EventQueue::popEvent() {
	Event event = events.front();
	events.pop_front();
	return event;
}

void EventQueue::registerEventGenerator(const event_generator_t & generator) {
	generators.emplace_back(generator);
}

void EventQueue::registerEventHandler(const event_handler_t & handler) {
	handlers.emplace_back(handler);
}

void EventQueue::generate() {
	for(const auto & generator : generators) {
		const auto newEvents = generator();
		events.insert(events.end(), newEvents.begin(), newEvents.end());
	}
}

void EventQueue::handle() {
	std::deque<Event> unhandledEvents;
	for(const auto & event : events) {
		bool handled = false;
		for(const auto & handler : handlers) {
			if(handler(event)) {
				handled = true;
				break;
			}
		}
		if(!handled) {
			unhandledEvents.emplace_back(event);
		}
	}
	events.swap(unhandledEvents);
}

void EventQueue::process() {
	generate();
	handle();
}

}
}
