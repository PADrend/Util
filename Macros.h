/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

#include <string>
#include <stdexcept>

namespace Util {
	enum output_priority_t {
		OUTPUT_DEBUG,
		OUTPUT_WARNING,
		OUTPUT_ERROR
	};
	void output(output_priority_t priority,const std::string & message);
	std::string composeDebugMessage(const std::string & message,const char * file, int line);
}

#ifdef DEBUG_MODE
#define DEBUG(M) \
	Util::output(Util::OUTPUT_DEBUG, Util::composeDebugMessage(M, __FILE__, __LINE__))
#else
#define DEBUG(M)
#endif

#define WARN(M) \
	Util::output(Util::OUTPUT_WARNING, Util::composeDebugMessage(M, __FILE__, __LINE__))

#define FAIL() \
	do{ \
		const std::string msg = Util::composeDebugMessage( "Runtime error.", __FILE__, __LINE__);\
		Util::output(Util::OUTPUT_ERROR, msg);\
		throw std::runtime_error(msg);\
	}while(false)

#define FAIL_IF(C) \
	do{ \
		if (C)	FAIL(); \
	}while(false)

//! Use this macro if a parameter value can not be applied in the context of a function (e.g. wrong color format)
#define INVALID_ARGUMENT_EXCEPTION(msg) \
	do{ \
		throw std::invalid_argument( Util::composeDebugMessage(msg, __FILE__, __LINE__) );\
	}while(false)
	
//! Macros for disabling compiler warnings when using including library headers or or for code that produces warnings which can not be fixed
	
#define GCC_DIAG_STR(s) #s
#define GCC_DIAG_DO_PRAGMA(x) _Pragma (#x)
#define GCC_DIAG_PRAGMA(x) GCC_DIAG_DO_PRAGMA(GCC diagnostic x)

#ifdef __clang__
 
#define COMPILER_WARN_PUSH GCC_DIAG_PRAGMA(push)
#define COMPILER_WARN_POP GCC_DIAG_PRAGMA(pop)
#define COMPILER_WARN_OFF_CLANG(x) GCC_DIAG_PRAGMA(ignored GCC_DIAG_STR(x))
#define COMPILER_WARN_OFF_GCC(x)
#define COMPILER_WARN_OFF(x) COMPILER_WARN_OFF_CLANG(x)

#elif __GNUC__

#define COMPILER_WARN_PUSH GCC_DIAG_PRAGMA(push)
#define COMPILER_WARN_POP GCC_DIAG_PRAGMA(pop)
#define COMPILER_WARN_OFF_CLANG(x)
#define COMPILER_WARN_OFF_GCC(x) GCC_DIAG_PRAGMA(ignored GCC_DIAG_STR(x))
#define COMPILER_WARN_OFF(x) COMPILER_WARN_OFF_GCC(x)

#else

#define COMPILER_WARN_PUSH
#define COMPILER_WARN_POP
#define COMPILER_WARN_OFF(x)
#define COMPILER_WARN_OFF_GCC(x)
#define COMPILER_WARN_OFF_CLANG(x)

#endif
	

#endif // MACROS_H_INCLUDED
