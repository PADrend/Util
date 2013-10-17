/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "FactoryTest.h"
#include <cppunit/TestAssert.h>
#include <Util/TypeNameMacro.h>
#include <Util/Factory/Factory.h>
#include <cstddef>
#include <functional>
#include <typeinfo>
#include <sstream>
CPPUNIT_TEST_SUITE_REGISTRATION(FactoryTest);

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

void FactoryTest::test() {
	Util::Factory<Base *, std::string> factory;
	factory.registerType("BaseType", Util::ObjectCreator<Base>());
	factory.registerType("DerivedAType", Util::ObjectCreator<DerivedA>());
	factory.registerType("DerivedBType", Util::ObjectCreator<DerivedB>());

	Base * objectU = factory.create("BaseType");
	Base * objectV = factory.create("DerivedAType");
	Base * objectW = factory.create("DerivedBType");

	CPPUNIT_ASSERT(objectU != nullptr);
	CPPUNIT_ASSERT(objectV != nullptr);
	CPPUNIT_ASSERT(objectW != nullptr);

	CPPUNIT_ASSERT(typeid(Base) == typeid(*objectU));
	CPPUNIT_ASSERT(typeid(DerivedA) == typeid(*objectV));
	CPPUNIT_ASSERT(typeid(DerivedB) == typeid(*objectW));

	delete objectW;
	delete objectV;
	delete objectU;
}

void FactoryTest::testUnknown() {
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

		CPPUNIT_ASSERT(objectA != nullptr);
		CPPUNIT_ASSERT(objectB != nullptr);
		CPPUNIT_ASSERT(objectC != nullptr);

		CPPUNIT_ASSERT_EQUAL(3, *objectA);
		CPPUNIT_ASSERT_EQUAL(2, *objectB);
		CPPUNIT_ASSERT_EQUAL(1, *objectC);

		delete objectC;
		delete objectB;
		delete objectA;

		typedef Util::FallbackPolicies::ExceptionFallback<int *, int>::Exception FactoryException;
		CPPUNIT_ASSERT_THROW(factory.create(0), FactoryException);
		CPPUNIT_ASSERT_THROW(factory.create(4), FactoryException);
		CPPUNIT_ASSERT_THROW(factory.create(17), FactoryException);
	}
	{
		Util::Factory<int *, int, IntCreator, Util::FallbackPolicies::NULLFallback> factory;
		factory.registerType(1, IntFactory::setOne);
		factory.registerType(2, IntFactory::setTwo);
		factory.registerType(3, IntFactory::setThree);

		CPPUNIT_ASSERT_EQUAL(static_cast<int *>(nullptr), factory.create(0));
		CPPUNIT_ASSERT_EQUAL(static_cast<int *>(nullptr), factory.create(4));
		CPPUNIT_ASSERT_EQUAL(static_cast<int *>(nullptr), factory.create(17));

		CPPUNIT_ASSERT_NO_THROW(factory.create(0));
		CPPUNIT_ASSERT_NO_THROW(factory.create(4));
		CPPUNIT_ASSERT_NO_THROW(factory.create(17));
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
		
		CPPUNIT_ASSERT(outputStream.good());
		CPPUNIT_ASSERT(!outputStream.str().empty());

		CPPUNIT_ASSERT(objectA != nullptr);
		CPPUNIT_ASSERT(objectB != nullptr);
		CPPUNIT_ASSERT(objectC != nullptr);

		CPPUNIT_ASSERT_EQUAL(2, *objectA);
		CPPUNIT_ASSERT_EQUAL(2, *objectB);
		CPPUNIT_ASSERT_EQUAL(2, *objectC);

		delete objectC;
		delete objectB;
		delete objectA;
	}
}
