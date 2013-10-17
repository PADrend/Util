/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_CURL

#include "NetProvider.h"
#include "FileName.h"
#include "FileUtils.h"
#include "../Factory/Factory.h"
#include "../Macros.h"
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <curl/curl.h>

namespace Util {

bool NetProvider::init() {
	static NetProvider provider;
	bool result = true;
	if(!FileUtils::registerFSProvider("http", PointerHolderCreator<NetProvider>(&provider))) {
		result = false;
	}
	if(!FileUtils::registerFSProvider("https", PointerHolderCreator<NetProvider>(&provider))) {
		result = false;
	}
	if(!FileUtils::registerFSProvider("ftp", PointerHolderCreator<NetProvider>(&provider))) {
		result = false;
	}
	return result;
}

NetProvider::NetProvider() :
	AbstractFSProvider() {
	if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
		WARN("Error initializing libcurl.");
	}
	handle = curl_easy_init();
	if (handle == nullptr) {
		WARN("Error initializing libcurl.");
	}
}

NetProvider::~NetProvider() {
	curl_easy_cleanup(handle);
	curl_global_cleanup();
}

static size_t NetProviderWriteCallback(void * ptr, size_t size, size_t nmemb, void * data) {
	const size_t overallSize = size * nmemb;
	const uint8_t * src = static_cast<const uint8_t *>(ptr);

	std::vector<uint8_t> * binData = static_cast<std::vector<uint8_t> *>(data);
	binData->insert(binData->end(), src, src + overallSize);

	return overallSize;
}

AbstractFSProvider::status_t NetProvider::readFile(const FileName & url, std::vector<uint8_t> & data) {
	curl_easy_setopt(handle, CURLOPT_URL, url.getPath().c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, NetProviderWriteCallback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, reinterpret_cast<void *>(&data));

	if (curl_easy_perform(handle) != CURLE_OK) {
		WARN("Transfer in libcurl failed.");
		return FAILURE;
	}

	return OK;
}

bool NetProvider::isFile(const FileName & /*url*/) {
	return true;
}

}

#endif /* UTIL_HAVE_LIB_CURL */
