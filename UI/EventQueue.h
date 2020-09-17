/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_EVENTQUEUE_H
#define UTIL_EVENTQUEUE_H

#include "Event.h"
#include <deque>
#include <functional>

namespace Util {
namespace UI {

/**
 * @brief Data structure for event handling
 *
 * The event queue manages input events that are created by different
 * generators and consumed by different handlers.
 *
 * The event generators can be the windowing system or a library that supports
 * game controllers, for example. The different generators can register
 * themselves as event generator using registerEventGenerator().
 *
 * Multiple event handlers can be registered using registerEventHandler().
 * Optionally, a filter function can be given that ensures that only specific
 * events are given to the handler.
 *
 * During process(), first the event generators are asked for new events.
 * After that, all existing events are filtered and given to the respective
 * event handlers.
 *
 * @note This class has to be used from the same thread as the window.
 * For the reasons, see http://www.libsdl.org/docs/html/sdlpumpevents.html
 * or Section 2.7 of http://www.x.org/docs/X11/xlib.pdf.
 * @author Benjamin Eikel
 * @date 2012-09-12
 */
class EventQueue {
	public:
		/**
		 * Function type for an event generator that returns an array of
		 * events.
		 */
		typedef std::function<std::deque<Event>()> event_generator_t;

		/**
		 * Function type for an event handler that receives a single event.
		 * The return type indicates if the event has been handled: If the
		 * handler returns @c true, it has handled the event. If the event
		 * has not been handled (return value @c false), it will be passed
		 * to the next handler.
		 */
		typedef std::function<bool (const Event &)> event_handler_t;

	private:
		/**
		 * Data structure storing the events.
		 * New events will be added to the back.
		 * Events will be extracted from the front.
		 */
		std::deque<Event> events;

		//! Container holding the event generators
		std::deque<event_generator_t> generators;

		//! Container holding the event handlers
		std::deque<event_handler_t> handlers;

		//! Internal function that calls all generators
		UTILAPI void generate();

		//! Internal function that calls all handler
		UTILAPI void handle();
	public:
		/**
		 * Return the number of unprocessed events that are waiting in the
		 * event queue.
		 * 
		 * @return Number of events waiting for processing
		 */
		UTILAPI std::size_t getNumEventsAvailable() const;

		/**
		 * Add a new event to the event queue.
		 *
		 * @param newEvent Event that will be inserted
		 */
		UTILAPI void pushEvent(const Event & newEvent);

		/**
		 * Read the first element from the event queue without removing it.
		 *
		 * @return Read-only reference to the first event
		 * @throw std::logic_error if there are no events available
		 */
		UTILAPI const Event & peekEvent() const;

		/**
		 * Remove the first element from the event queue and return it.
		 *
		 * @return Element that has been the first element of the event queue
		 * @throw std::logic_error if there are no events available
		 */
		UTILAPI Event popEvent();

		/**
		 * Register the given function as an event generator.
		 *
		 * @param generator Event generator that creates an array of events
		 */
		UTILAPI void registerEventGenerator(const event_generator_t & generator);

		/**
		 * Register the given function as an event handler.
		 *
		 * @param handler Event handler that takes a single event
		 */
		UTILAPI void registerEventHandler(const event_handler_t & handler);

		// FIXME: Add filter possibility

		/**
		 * Collect generated events and pass them to handlers.
		 *
		 * @note This function has to be called regularly to ensure the processing
		 * of events.
		 */
		UTILAPI void process();
};

}
}

#endif /* UTIL_EVENTQUEUE_H */
