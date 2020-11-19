/*
	This file is part of the Util library.
	Copyright (C) 2017 Sascha Brandt <myeti@mail.upb.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_LOADLIBRARY_H_
#define UTIL_LOADLIBRARY_H_

#include "StringIdentifier.h"

#include <string>

namespace Util {
	
//! @addtogroup util_helper
//! @{

/**
 * Function used to dynamically load a library at runtime.
 *
 * @note This is highly experimental and not intended for use in production code. Use with caution.
 *
 * @param filename File path to the dynamic library.
 * @return StringIdentifier of the filename on success (used as library handle), or empty StringIdentifier on failure.
 */
UTILAPI const StringIdentifier loadLibrary(const std::string& filename);

/**
 * Loads a function of a dynamically loaded library at runtime.
 *
 * @note This is highly experimental and not intended for use in production code. Use with caution.
 *
 * @param libraryId Library handle received from loadLibrary.
 * @param name name of the function.
 * @return Function handle or nullptr.
 */
UTILAPI void* loadFunction(const StringIdentifier& libraryId, const std::string& name);

/**
 * Function used to unload a dynamically loaded library.
 *
 * @note This is highly experimental and not intended for use in production code. Use with caution.  
 *       Actually, don't use it at all, because it can cause unforseeable side effects. 
 *
 * @param libraryId Library handle received from loadLibrary.
 */
UTILAPI void unloadLibrary(const StringIdentifier& libraryId);
	
	
//! @}
}

#endif /* end of include guard: UTIL_LOADLIBRARY_H_ */
