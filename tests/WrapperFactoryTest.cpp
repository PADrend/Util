/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "WrapperFactoryTest.h"
#include <cppunit/TestAssert.h>
#include <Util/TypeNameMacro.h>
#include <Util/Factory/WrapperFactory.h>
#include <cstddef>
#include <functional>
#include <typeindex>
#include <typeinfo>
#include <sstream>
CPPUNIT_TEST_SUITE_REGISTRATION(WrapperFactoryTest);

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

void WrapperFactoryTest::test() {
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

	CPPUNIT_ASSERT(wrapperU != nullptr);
	CPPUNIT_ASSERT(wrapperV != nullptr);
	CPPUNIT_ASSERT(wrapperW != nullptr);

	CPPUNIT_ASSERT(typeid(WrapperBase) == typeid(*wrapperU));
	CPPUNIT_ASSERT(typeid(WrapperDerivedA) == typeid(*wrapperV));
	CPPUNIT_ASSERT(typeid(WrapperDerivedB) == typeid(*wrapperW));

	delete wrapperW;
	delete wrapperV;
	delete wrapperU;
	delete w;
	delete v;
	delete u;
}

void WrapperFactoryTest::testUnknown() {
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

		CPPUNIT_ASSERT(productA != nullptr);
		CPPUNIT_ASSERT(productB != nullptr);
		CPPUNIT_ASSERT(productC != nullptr);

		CPPUNIT_ASSERT_EQUAL(3, *productA);
		CPPUNIT_ASSERT_EQUAL(4, *productB);
		CPPUNIT_ASSERT_EQUAL(3, *productC);

		delete productC;
		delete productB;
		delete productA;

		typedef Util::FallbackPolicies::ExceptionFallback<int *, int>::Exception FactoryException;
		CPPUNIT_ASSERT_THROW(factory.create(0, &one), FactoryException);
		CPPUNIT_ASSERT_THROW(factory.create(4, &two), FactoryException);
		CPPUNIT_ASSERT_THROW(factory.create(17, &three), FactoryException);
	}
	{
		Util::WrapperFactory<int *, int *, int, IntCreator, Util::FallbackPolicies::NULLFallback> factory;
		factory.registerType(1, IntFactory::timesOne);
		factory.registerType(2, IntFactory::timesTwo);
		factory.registerType(3, IntFactory::timesThree);

		int one = 1;
		int two = 2;
		int three = 3;

		CPPUNIT_ASSERT_EQUAL(static_cast<int *>(nullptr), factory.create(0, &one));
		CPPUNIT_ASSERT_EQUAL(static_cast<int *>(nullptr), factory.create(4, &two));
		CPPUNIT_ASSERT_EQUAL(static_cast<int *>(nullptr), factory.create(17, &three));

		CPPUNIT_ASSERT_NO_THROW(factory.create(0, &one));
		CPPUNIT_ASSERT_NO_THROW(factory.create(4, &two));
		CPPUNIT_ASSERT_NO_THROW(factory.create(17, &three));
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
		
		CPPUNIT_ASSERT(outputStream.good());
		CPPUNIT_ASSERT(!outputStream.str().empty());

		CPPUNIT_ASSERT(productA != nullptr);
		CPPUNIT_ASSERT(productB != nullptr);
		CPPUNIT_ASSERT(productC != nullptr);

		CPPUNIT_ASSERT_EQUAL(2, *productA);
		CPPUNIT_ASSERT_EQUAL(4, *productB);
		CPPUNIT_ASSERT_EQUAL(6, *productC);

		delete productC;
		delete productB;
		delete productA;
	}
}
