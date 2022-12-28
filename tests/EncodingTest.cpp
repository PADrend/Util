/*
	This file is part of the Util library.
	Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Encoding.h"

#include <catch2/catch.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <random>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244) // disable possible loss of data warning on windows
#endif

TEST_CASE("EncodingTest", "[EncodingTest]") {
	std::default_random_engine engine;
	std::uniform_int_distribution<uint16_t> distribution(0, 255);
	// Test all three cases of padding.
	const size_t sizes[6] = { 1, 2, 3, 1023, 1024, 1025 };
	for (uint_fast8_t s = 0; s < 6; ++s) {
		std::vector<uint8_t> original(sizes[s]);

		for (uint_fast8_t run = 0; run < 10; ++run) {
			std::generate(original.begin(), original.end(), std::bind(distribution, engine));

			const std::string encoded = Util::encodeBase64(original);

			const std::vector<uint8_t> decoded = Util::decodeBase64(encoded);

			REQUIRE(decoded == original);
		}
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif