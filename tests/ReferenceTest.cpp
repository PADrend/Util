/*
	This file is part of the Platform for Algorithm Development and Rendering (PADrend).
	Web page: http://www.padrend.de/
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2014-2022 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <catch2/catch.hpp>
#include "ReferenceCounter.h"

using namespace Util;

class ClassA : public Util::ReferenceCounter<ClassA> {
public:
	static uint32_t destructorCalls;
	virtual ~ClassA() {
		destructorCalls++;
	}
};

uint32_t ClassA::destructorCalls = 0;

class ClassB : public ClassA {
};

Reference<ClassA> functionTestA(ClassA* obj) {
	Reference<ClassA> ref = obj;
	return ref;
}

Reference<ClassA> functionTestB2A(ClassB* obj) {
	Reference<ClassB> ref = obj;
	return ref;
}

Reference<ClassA> functionTestNewB2A(bool condition) {
	Reference<ClassB> ref = new ClassB;
	return condition ? ref : nullptr;
}

TEST_CASE("TestReferences", "[ReferenceTest]") {
	{
		ClassA objA;
		ClassB objB;
		ClassA::addReference(&objA); // force reference count of 1 to avoid destruction
		ClassB::addReference(&objB);
		REQUIRE(objA.countReferences() == 1);
		REQUIRE(objB.countReferences() == 1);
		
		{
			Reference<ClassA> refA = &objA;
			Reference<ClassB> refB = &objB;
			REQUIRE(objA.countReferences() == 2);
			REQUIRE(objB.countReferences() == 2);
		}
		REQUIRE(objA.countReferences() == 1);
		REQUIRE(objB.countReferences() == 1);
		
		{
			Reference<ClassA> refA = &objA;
			Reference<ClassA> refB = &objB;
			REQUIRE(objA.countReferences() == 2);
			REQUIRE(objB.countReferences() == 2);
		}
		REQUIRE(objA.countReferences() == 1);
		REQUIRE(objB.countReferences() == 1);
		
		{
			Reference<ClassA> refA = &objA;
			REQUIRE(objA.countReferences() == 2);
			Reference<ClassA> refB = refA;
			REQUIRE(objA.countReferences() == 3);
		}
		REQUIRE(objA.countReferences() == 1);
		
		{
			Reference<ClassA> refA = &objA;
			REQUIRE(objA.countReferences() == 2);
			Reference<ClassA> refB = std::move(refA);
			REQUIRE(objA.countReferences() == 2);
		}
		REQUIRE(objA.countReferences() == 1);
		
		{
			Reference<ClassA> refA, refB;
			refA = &objA;
			REQUIRE(objA.countReferences() == 2);
			refB = refA;
			REQUIRE(objA.countReferences() == 3);
		}
		REQUIRE(objA.countReferences() == 1);
		
		{
			Reference<ClassA> refA, refB;
			refA = &objA;
			REQUIRE(objA.countReferences() == 2);
			refB = std::move(refA);
			REQUIRE(objA.countReferences() == 2);
		}
		REQUIRE(objA.countReferences() == 1);
		REQUIRE(ClassA::destructorCalls == 0);
		
		{
			Reference<ClassA> refA = functionTestA(&objA);
			REQUIRE(objA.countReferences() == 2);
			Reference<ClassA> refB = functionTestB2A(&objB);
			REQUIRE(objB.countReferences() == 2);

			Reference<ClassA> refC = functionTestNewB2A(true);
			REQUIRE(ClassA::destructorCalls == 0);
			REQUIRE(refC->countReferences() == 1);

			Reference<ClassA> refD = functionTestNewB2A(false);
			REQUIRE(ClassA::destructorCalls == 1);
			REQUIRE(refD.isNull());
		}
		REQUIRE(objA.countReferences() == 1);
		REQUIRE(objB.countReferences() == 1);
		
		REQUIRE(ClassA::destructorCalls == 2);
	}
	REQUIRE(ClassA::destructorCalls == 4);
}