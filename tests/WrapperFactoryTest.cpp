/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch.hpp>
#include "TypeNameMacro.h"
#include "Factory/WrapperFactory.h"
#include <cstddef>
#include <functional>
#include <typeindex>
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

struct WrapperBase {
	Base * internal;
	WrapperBase(Base * object) : internal(object) {
	}
	virtual ~WrapperBase() {
	}
};
struct WrapperDerivedA : public WrapperBase {
	WrapperDerivedA(DerivedA * object) : WrapperBase(object) {
	}
	virtual ~WrapperDerivedA() {
	}
};
struct WrapperDerivedB : public WrapperBase {
	WrapperDerivedB(DerivedB * object) : WrapperBase(object) {
	}
	virtual ~WrapperDerivedB() {
	}
};

TEST_CASE("WrapperFactoryTest_test", "[WrapperFactoryTest]") {
	Util::WrapperFactory<Base *, WrapperBase *, std::type_index> factory;
	factory.registerType(typeid(Base), Util::PolymorphicWrapperCreator<Base, Base, WrapperBase>());
	factory.registerType(typeid(DerivedA), Util::PolymorphicWrapperCreator<Base, DerivedA, WrapperDerivedA>());
	factory.registerType(typeid(DerivedB), Util::PolymorphicWrapperCreator<Base, DerivedB, WrapperDerivedB>());

	Base * u = new Base;
	Base * v = new DerivedA;
	Base * w = new DerivedB;
	WrapperBase * wrapperU = factory.create(typeid(*u), u);
	WrapperBase * wrapperV = factory.create(typeid(*v), v);
	WrapperBase * wrapperW = factory.create(typeid(*w), w);

	REQUIRE(wrapperU != nullptr);
	REQUIRE(wrapperV != nullptr);
	REQUIRE(wrapperW != nullptr);

	REQUIRE(typeid(WrapperBase) == typeid(*wrapperU));
	REQUIRE(typeid(WrapperDerivedA) == typeid(*wrapperV));
	REQUIRE(typeid(WrapperDerivedB) == typeid(*wrapperW));

	delete wrapperW;
	delete wrapperV;
	delete wrapperU;
	delete w;
	delete v;
	delete u;
}

TEST_CASE("WrapperFactoryTest_testUnknown", "[WrapperFactoryTest]") {
	struct IntFactory {
		static int * timesOne(int * factor) {
			int * result = new int;
			*result = *factor * 1;
			return result;
		}
		static int * timesTwo(int * factor) {
			int * result = new int;
			*result = *factor * 2;
			return result;
		}
		static int * timesThree(int * factor) {
			int * result = new int;
			*result = *factor * 3;
			return result;
		}
	};
	typedef std::function<int * (int *)> IntCreator;
	{
		Util::WrapperFactory<int *, int *, int, IntCreator, Util::FallbackPolicies::ExceptionFallback> factory;
		factory.registerType(1, IntFactory::timesOne);
		factory.registerType(2, IntFactory::timesTwo);
		factory.registerType(3, IntFactory::timesThree);

		int one = 1;
		int two = 2;
		int three = 3;

		int * productA = factory.create(3, &one);
		int * productB = factory.create(2, &two);
		int * productC = factory.create(1, &three);

		REQUIRE(productA != nullptr);
		REQUIRE(productB != nullptr);
		REQUIRE(productC != nullptr);

		REQUIRE(3 == *productA);
		REQUIRE(4 == *productB);
		REQUIRE(3 == *productC);

		delete productC;
		delete productB;
		delete productA;

		typedef Util::FallbackPolicies::ExceptionFallback<int *, int>::Exception FactoryException;
		REQUIRE_THROWS_AS(factory.create(0, &one), FactoryException);
		REQUIRE_THROWS_AS(factory.create(4, &two), FactoryException);
		REQUIRE_THROWS_AS(factory.create(17, &three), FactoryException);
	}
	{
		Util::WrapperFactory<int *, int *, int, IntCreator, Util::FallbackPolicies::NULLFallback> factory;
		factory.registerType(1, IntFactory::timesOne);
		factory.registerType(2, IntFactory::timesTwo);
		factory.registerType(3, IntFactory::timesThree);

		int one = 1;
		int two = 2;
		int three = 3;

		REQUIRE(static_cast<int *>(nullptr) == factory.create(0, &one));
		REQUIRE(static_cast<int *>(nullptr) == factory.create(4, &two));
		REQUIRE(static_cast<int *>(nullptr) == factory.create(17, &three));

		REQUIRE_NOTHROW(factory.create(0, &one));
		REQUIRE_NOTHROW(factory.create(4, &two));
		REQUIRE_NOTHROW(factory.create(17, &three));
	}
	{
		std::ostringstream outputStream;
		Util::FallbackPolicies::DefaultCreatorFallback<int *, int> fallback(2, outputStream);
		Util::WrapperFactory<int *, int *, int, IntCreator, Util::FallbackPolicies::DefaultCreatorFallback> factory(fallback);
		factory.registerType(1, IntFactory::timesOne);
		factory.registerType(2, IntFactory::timesTwo);
		factory.registerType(3, IntFactory::timesThree);

		int one = 1;
		int two = 2;
		int three = 3;

		int * productA = factory.create(0, &one);
		int * productB = factory.create(4, &two);
		int * productC = factory.create(17, &three);
		
		REQUIRE(outputStream.good());
		REQUIRE(!outputStream.str().empty());

		REQUIRE(productA != nullptr);
		REQUIRE(productB != nullptr);
		REQUIRE(productC != nullptr);

		REQUIRE(2 == *productA);
		REQUIRE(4 == *productB);
		REQUIRE(6 == *productC);

		delete productC;
		delete productB;
		delete productA;
	}
}
