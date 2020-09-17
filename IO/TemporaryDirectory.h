/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef TEMPORARYDIRECTORY_H_
#define TEMPORARYDIRECTORY_H_

#include "FileName.h"
#include "../ReferenceCounter.h"
#include <cstdint>
#include <string>

namespace Util {

/**
 * When creating an object of this class, a new directory is created, which can be used to store temporary files.
 * When the object is destroyed, the directory will be deleted together with all files inside.
 *
 * @brief Creation and deletion of temporary directories
 * @author Benjamin Eikel
 * @date 2011-08-15
 * @ingroup io
 */
class TemporaryDirectory : public ReferenceCounter<TemporaryDirectory>{
	private:
		const FileName path;

		//! Counter that is increased for every instance of this class.
		UTILAPI static uint32_t counter;

		TemporaryDirectory(const TemporaryDirectory &) = delete;
		TemporaryDirectory(TemporaryDirectory &&) = delete;
		TemporaryDirectory & operator=(const TemporaryDirectory &) = delete;
		TemporaryDirectory & operator=(TemporaryDirectory &&) = delete;
	public:
		/**
		 * Create a new temporary directory.
		 * The part will contain the current process identifier, an automatically increasing counter and the given fragment.
		 *
		 * @param fragment String that will be used as part of the path.
		 * This must not contain characters that are invalid in file names.
		 */
		UTILAPI TemporaryDirectory(const std::string & fragment);

		//! Delete the directory and all its contents.
		UTILAPI ~TemporaryDirectory();

		/**
		 * Retrieve the path to the directory.
		 *
		 * @return Directory location that can be used for temporary files.
		 */
		const FileName & getPath() const {
			return path;
		}

};

}

#endif /* TEMPORARYDIRECTORY_H_ */
