/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "TypeNameMacro.h"
#include "Factory/Factory.h"
#include <catch2/catch.hpp>
#include <cstddef>
#include <functional>
#include <typeinfo>
#include <sstream>

struct Base {
	virtual ~Base() {
	}
};
struct DerivedA : public Base {
	virtual ~DerivedA() {
	}
};
struct DerivedB : public Base {
	virtual ~DerivedB() {
	}
};

TEST_CASE("FactoryTest", "[FactoryTest]") {
	Util::Factory<Base *, std::string> factory;
	factory.registerType("BaseType", Util::ObjectCreator<Base>());
	factory.registerType("DerivedAType", Util::ObjectCreator<DerivedA>());
	factory.registerType("DerivedBType", Util::ObjectCreator<DerivedB>());

	Base * objectU = factory.create("BaseType");
	Base * objectV = factory.create("DerivedAType");
	Base * objectW = factory.create("DerivedBType");

	REQUIRE(objectU != nullptr);
	REQUIRE(objectV != nullptr);
	REQUIRE(objectW != nullptr);

	REQUIRE(typeid(Base) == typeid(*objectU));
	REQUIRE(typeid(DerivedA) == typeid(*objectV));
	REQUIRE(typeid(DerivedB) == typeid(*objectW));

	delete objectW;
	delete objectV;
	delete objectU;
}

TEST_CASE("FactoryTestUnknown", "[FactoryTest]") {
	struct IntFactory {
		static int * setOne() {
			int * result = new int;
			*result = 1;
			return result;
		}
		static int * setTwo() {
			int * result = new int;
			*result = 2;
			return result;
		}
		static int * setThree() {
			int * result = new int;
			*result = 3;
			return result;
		}
	};
	typedef std::function<int * ()> IntCreator;
	{
		Util::Factory<int *, int, IntCreator, Util::FallbackPolicies::ExceptionFallback> factory;
		factory.registerType(1, IntFactory::setOne);
		factory.registerType(2, IntFactory::setTwo);
		factory.registerType(3, IntFactory::setThree);

		int * objectA = factory.create(3);
		int * objectB = factory.create(2);
		int * objectC = factory.create(1);

		REQUIRE(objectA != nullptr);
		REQUIRE(objectB != nullptr);
		REQUIRE(objectC != nullptr);

		REQUIRE(3 == *objectA);
		REQUIRE(2 == *objectB);
		REQUIRE(1 == *objectC);

		delete objectC;
		delete objectB;
		delete objectA;

		typedef Util::FallbackPolicies::ExceptionFallback<int *, int>::Exception FactoryException;
		REQUIRE_THROWS_AS(factory.create(0), FactoryException);
		REQUIRE_THROWS_AS(factory.create(4), FactoryException);
		REQUIRE_THROWS_AS(factory.create(17), FactoryException);
	}
	{
		Util::Factory<int *, int, IntCreator, Util::FallbackPolicies::NULLFallback> factory;
		factory.registerType(1, IntFactory::setOne);
		factory.registerType(2, IntFactory::setTwo);
		factory.registerType(3, IntFactory::setThree);

		REQUIRE(static_cast<int *>(nullptr) == factory.create(0));
		REQUIRE(static_cast<int *>(nullptr) == factory.create(4));
		REQUIRE(static_cast<int *>(nullptr) == factory.create(17));

		REQUIRE_NOTHROW(factory.create(0));
		REQUIRE_NOTHROW(factory.create(4));
		REQUIRE_NOTHROW(factory.create(17));
	}
	{
		std::ostringstream outputStream;
		Util::FallbackPolicies::DefaultCreatorFallback<int *, int> fallback(2, outputStream);
		Util::Factory<int *, int, IntCreator, Util::FallbackPolicies::DefaultCreatorFallback> factory(fallback);
		factory.registerType(1, IntFactory::setOne);
		factory.registerType(2, IntFactory::setTwo);
		factory.registerType(3, IntFactory::setThree);

		int * objectA = factory.create(0);
		int * objectB = factory.create(4);
		int * objectC = factory.create(17);
		
		REQUIRE(outputStream.good());
		REQUIRE(!outputStream.str().empty());

		REQUIRE(objectA != nullptr);
		REQUIRE(objectB != nullptr);
		REQUIRE(objectC != nullptr);

		REQUIRE(2 == *objectA);
		REQUIRE(2 == *objectB);
		REQUIRE(2 == *objectC);

		delete objectC;
		delete objectB;
		delete objectA;
	}
}
