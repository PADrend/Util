/*
	This file is part of the Util library.
	Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "../IO/FileName.h"
#include "../IO/FileUtils.h"
#include "../StringUtils.h"
#include "../Utils.h"
#include "../Encoding.h"
#include "../IO/TemporaryDirectory.h"
#include <catch2/catch.hpp>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <vector>
#include <iostream>

const std::string testString1("Test12345678");
const std::vector<uint8_t> byteString1(testString1.cbegin(), testString1.cend());
	
#ifdef UTIL_HAVE_LIB_CURL
TEST_CASE("NetProviderTest_HTTP", "[NetProviderTest]") {
	const Util::FileName fileName("http://httpbin.org/base64/" + Util::encodeBase64(byteString1));
	
	std::string content = Util::FileUtils::getFileContents(fileName);
	REQUIRE(testString1.compare(content) == 0);
}

TEST_CASE("NetProviderTest_HTTPS", "[NetProviderTest]") {
	const Util::FileName fileName("https://httpbin.org/base64/" + Util::encodeBase64(byteString1));
	
	std::string content = Util::FileUtils::getFileContents(fileName);
	REQUIRE(testString1.compare(content) == 0);
}

TEST_CASE("NetProviderTest_FTP", "[NetProviderTest]") {
	const std::string md5 = "0f343b0931126a20f133d67c2b018a3b";
	const Util::FileName fileName("ftp://speedtest.tele2.net/1KB.zip");
	
	std::string content = Util::FileUtils::getFileContents(fileName);
	REQUIRE(md5.compare(Util::md5(content)) == 0);
}

TEST_CASE("NetProviderTest_Redirect", "[NetProviderTest]") {
	const Util::FileName fileName("https://httpbin.org/redirect-to?url=https%3A%2F%2Fhttpbin.org%2Fbase64%2F" + Util::encodeBase64(byteString1));
	
	std::string content = Util::FileUtils::getFileContents(fileName);
	REQUIRE(testString1.compare(content) == 0);
}

#endif
