/*
	This file is part of the MinSG library extension Profiling.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_PROFILING_PROFILER_H_
#define UTIL_PROFILING_PROFILER_H_

#include "Action.h"
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>

#ifdef PROFILING_ENABLED

#define INIT_PROFILING(Logger, out) \
	static Util::Profiling::Profiler _profiler; \
	static Util::Profiling::Logger _logger(out); \
	static std::unordered_map<std::string, Util::Profiling::Action> _actions; \
	static bool _loggerRegistered = []() -> bool { _profiler.registerLogger(&_logger); return true; }()

#define INIT_PROFILING_PLAIN_TEXT(out) INIT_PROFILING(LoggerPlainText, out)
#define INIT_PROFILING_TIME(out) INIT_PROFILING(LoggerTime, out)

#define SCOPED_PROFILING(descr) Util::Profiling::ScopedAction _action(&_profiler, #descr)
#define SCOPED_PROFILING_COND(descr, condition) Util::Profiling::ScopedAction _action((condition) ? &_profiler : nullptr, #descr)

#define BEGIN_PROFILING(descr) _actions[#descr] = _profiler.beginTimeMemoryAction(#descr)
#define END_PROFILING(descr) _profiler.endTimeMemoryAction(_actions[#descr])

#define BEGIN_PROFILING_COND(descr, condition) if(condition) _actions[#descr] = _profiler.beginTimeMemoryAction(#descr)
#define END_PROFILING_COND(descr, condition) if(condition) _profiler.endTimeMemoryAction(_actions[#descr])

#else

#define INIT_PROFILING(Logger, out)
#define INIT_PROFILING_PLAIN_TEXT(out)
#define INIT_PROFILING_TIME(out) 
#define SCOPED_PROFILING(descr)
#define BEGIN_PROFILING(descr)
#define END_PROFILING(descr)

#endif // PROFILING_ENABLED

namespace Util {
namespace Profiling {
typedef Util::GenericAttributeMap Action;
class Logger;

/**
 * @brief Profiling code (measure running time, memory consumption and log it)
 * 
 * The profiler manages actions. An action is an extendable object that can be
 * annotated by arbitrary attributes. The profile contains convenience
 * functions to create actions and to annotate some standard attributes.
 * Different loggers can be attached to the profiler that are used to write
 * the actions to streams (e.g. a file, stdout). For example, the profiler can
 * be used to measure the duration and memory consumption of a code section:
 * @code
 * Util::Profiling::Profiler profiler;
 * Util::Profiling::LoggerPlainText logger(std::cout);
 * 
 * profiler.registerLogger(&logger);
 * 
 * auto prepareAction = profiler.beginTimeMemoryAction("My preparation code");
 * ...
 * My preparation code
 * ...
 * profiler.endTimeMemoryAction(prepareAction);
 * auto executeAction = profiler.beginTimeMemoryAction("My execute code");
 * ...
 * My execute code
 * ...
 * profiler.endTimeMemoryAction(executeAction);
 * 
 * profiler.unregisterLogger(&logger);
 * @endcode
 */
class Profiler {
	private:
		// Use Pimpl idiom
		struct Implementation;
		std::unique_ptr<Implementation> impl;

	public:
		Profiler();
		~Profiler();

		/**
		 * Create a new action and set its description.
		 * 
		 * @param description Human-readable description of the action
		 * @return The created action
		 */
		Action createAction(const std::string & description) const;

		/**
		 * Output an action to the associated loggers.
		 * 
		 * @param action Action that will be logged
		 */
		void logAction(const Action & action) const;

		/**
		 * Measure the current time and store it in an attribute of an action.
		 * 
		 * @param action Action that will be annotated
		 * @param attribute Attribute name that will be added to the action
		 */
		void annotateTime(Action & action,
						  const Util::StringIdentifier & attribute) const;

		/**
		 * Measure the current memory consumption and store it in an attribute
		 * of an action.
		 * 
		 * @param action Action that will be annotated
		 * @param attribute Attribute name that will be added to the action
		 */
		void annotateMemory(Action & action,
							const Util::StringIdentifier & attribute) const;

		/**
		 * Create a new action, set its description, and store current time
		 * and memory consumption of the process.
		 * 
		 * @param description Human-readable description of the action
		 * @return The created action containing time and memory
		 */
		Action beginTimeMemoryAction(const std::string & description) const;

		/**
		 * Finish an action. The current time and memory consumption will be
		 * stored to allow comparison with the beginning of the action. The
		 * action will be logged.
		 * 
		 * @param action Action that will be annotated and logged
		 */
		void endTimeMemoryAction(Action & action) const;

		/**
		 * Register a logger that will be used to output profiling information.
		 * The information will be formatted internally by the logger.
		 * 
		 * @param output Logger that will be used to write profiling output.
		 * The given logger has to stay writeable at least until this object
		 * is destroyed or the logger is removed with @a unregisterLogger().
		 */
		void registerLogger(Logger * output);

		/**
		 * Remove the registered logger.
		 * 
		 * @param output Logger that will be removed
		 */
		void unregisterLogger(Logger * output);
};

//-------------------------------------

class ScopedAction {
public:
	explicit ScopedAction(Profiler* profiler, const std::string& descr) : profiler(profiler), action(profiler ? profiler->beginTimeMemoryAction(descr) : Action()) { }
	~ScopedAction() { if(profiler) profiler->endTimeMemoryAction(action); }
private:
	Profiler* profiler;
	Action action;
};

}
}

#endif /* UTIL_PROFILING_PROFILER_H_ */
