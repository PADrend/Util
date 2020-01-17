/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Resource.h"
#include "../Macros.h"

namespace Util {

//---------------

Resource::Resource(const ResourceFormat& format) : format(format) { }

//---------------

void Resource::upload(const uint8_t* srcData, size_t size, size_t offset) {
	WARN_AND_RETURN_IF(!srcData, "Resource: Cannot upload data. Invalid source data pointer.",);
	WARN_AND_RETURN_IF(!checkRange(offset, size), "Resource: Cannot upload data. Size + offset is out of range.",);
	doUpload(srcData, size, offset);
}

//---------------

void Resource::download(uint8_t* tgtData, size_t size, size_t offset) {
	WARN_AND_RETURN_IF(!tgtData, "Resource: Cannot download data. Invalid target data pointer.",);
	WARN_AND_RETURN_IF(!checkRange(offset, size), "Resource: Cannot download data. Size + offset is out of range.",);
	doDownload(tgtData, size, offset);
}

//---------------

} /* Util */