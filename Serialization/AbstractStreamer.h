/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_ABSTRACTSTREAMER_H_
#define UTIL_ABSTRACTSTREAMER_H_

#include "../Macros.h"
#include <cstdint>
#include <iosfwd>
#include <string>

namespace Util {
class GenericAttributeList;
namespace Serialization {

/**
 * Interface for classes that are capable of converting between objects and streams.
 * Subclasses are allowed to implement only a subset of the functions.
 * Capabilities can be queried.
 *
 * @author Benjamin Eikel
 * @date 2011-02-03
 */
class AbstractStreamer {
	public:
		virtual ~AbstractStreamer() {
		}

		/**
		 * Load generic data from the given stream.
		 *
		 * @param input Use the data from the stream beginning at the preset position.
		 * @return Description list containing the generic data. The caller is responsible for the memory deallocation.
		 */
		virtual GenericAttributeList * loadGeneric(std::istream & /*input*/) {
			WARN("Unsupported call for loading generic data.");
			return nullptr;
		}

		/**
		 * Save generic data to the given stream.
		 *
		 * @param data Description list containing the generic data to save.
		 * @param output Use the stream for writing beginning at the preset position.
		 * @return @c true if successful, @c false otherwise
		 */
		virtual bool saveGeneric(GenericAttributeList * /*data*/, std::ostream & /*output*/) {
			WARN("Unsupported call for saving generic data.");
			return false;
		}

		static const uint8_t CAP_LOAD_GENERIC =	1 << 0; //!< Streamer supports the function @a loadGeneric
		static const uint8_t CAP_SAVE_GENERIC =	1 << 1; //!< Streamer supports the function @a saveGeneric

		/**
		 * Check which capabilities are supported for the given file extension.
		 *
		 * @param extension File extension in lower case to check capabilities for.
		 * @return Bitmask consisting of a combination of @a CAP_LOAD_GENERIC, @a CAP_SAVE_GENERIC, flags that are defined by subclasses, or zero.
		 */
		static uint8_t queryCapabilities(const std::string & /*extension*/) {
			return 0;
		}

	protected:
		//! Creation is only possible in subclasses.
		AbstractStreamer() {
		}
};

}
}

#endif /* UTIL_ABSTRACTSTREAMER_H_ */
