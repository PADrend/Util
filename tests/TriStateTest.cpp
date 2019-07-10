/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2013 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch.hpp>

#include "TriState.h"


TEST_CASE("TriStateTest", "[TriStateTest]") {
	
	using Util::TriState;

	REQUIRE(sizeof(TriState)==1);

	TriState x;
	REQUIRE(x==TriState());
	REQUIRE(x!=true);
	REQUIRE(x!=false);
	REQUIRE(!x.isTrue());
	REQUIRE(!x.isFalse());
	REQUIRE(x.isUndefined());
	REQUIRE(!x.isDefined());

	TriState y(true);
	REQUIRE(y!=TriState());
	REQUIRE(y!=x);
	REQUIRE(y==true);
	REQUIRE(y.isTrue());
	REQUIRE(y!=false);
	REQUIRE(!y.isUndefined());
	REQUIRE(y.isDefined());

	REQUIRE(TriState(y) == y);
	REQUIRE(TriState(x) != y);

	y.undefine();
	REQUIRE(y==x);

	y = true;
	REQUIRE(x!=y);
	x = false;
	REQUIRE(x.isFalse());
	REQUIRE(x!=y);
	y = false;
	REQUIRE(x==y);
}
