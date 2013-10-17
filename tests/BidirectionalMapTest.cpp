/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2013 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "BidirectionalMapTest.h"
#include <Util/BidirectionalMap.h>
#include <Util/References.h>
#include <Util/Graphics/Bitmap.h>
CPPUNIT_TEST_SUITE_REGISTRATION(BidirectionalUnorderedMapTest);

#include<iostream>

void BidirectionalUnorderedMapTest::test() {

    Util::BidirectionalUnorderedMap<int,int> map;

    //! insertion
    CPPUNIT_ASSERT(map.insert(1,2));
    CPPUNIT_ASSERT(map.insert(3,4));
    CPPUNIT_ASSERT(!map.insert(1,5));
    CPPUNIT_ASSERT(!map.insert(5,4));
    CPPUNIT_ASSERT(map.insert(std::make_pair<int,int>(5,5)));

    //! searching
    CPPUNIT_ASSERT(map.findLeft(1) != map.endLeft());
    CPPUNIT_ASSERT(map.findLeft(1)->second == 2);
    CPPUNIT_ASSERT(map.findRight(2) != map.endRight());
    CPPUNIT_ASSERT(map.findRight(2)->second == 1);

    //! erasing
    CPPUNIT_ASSERT(map.eraseRight(2) == 1);
    CPPUNIT_ASSERT(map.findRight(2) == map.endRight());
    CPPUNIT_ASSERT(map.findLeft(1) == map.endLeft());
    CPPUNIT_ASSERT(map.eraseLeft(3) == 1);
    CPPUNIT_ASSERT(map.findLeft(3) == map.endLeft());
    CPPUNIT_ASSERT(map.findRight(4) == map.endRight());

    //! size & clear
    CPPUNIT_ASSERT(map.size() == 1);
    CPPUNIT_ASSERT(!map.empty());
    map.eraseRight(5);
    CPPUNIT_ASSERT(map.empty());
    map.insert(1,2);
    map.insert(3,4);
    map.clear();
    CPPUNIT_ASSERT(map.empty());

    //! swap
    Util::BidirectionalUnorderedMap<int,int> map2;
    map.insert(1,1);
    map.insert(2,2);
    map2.insert(1,2);
    map.swap(map2);
    CPPUNIT_ASSERT(map2.findLeft(1)->second == 1);
    CPPUNIT_ASSERT(map2.findLeft(2)->second == 2);
    CPPUNIT_ASSERT(map.findLeft(1)->second == 2);
    CPPUNIT_ASSERT(map2.size()==2);
    CPPUNIT_ASSERT(map.size()==1);
	{
		//! util references
		Util::BidirectionalUnorderedMap<Util::Reference<Util::Bitmap>,int, Util::BidirectionalMapPolicies::hashByGet > map3;
		Util::Reference<Util::Bitmap> a = new Util::Bitmap(), b = new Util::Bitmap(), c = new Util::Bitmap();
		map3.insert(a, 1);
		map3.insert(b.get(), 2);
		map3.insert(Util::Reference<Util::Bitmap>(c.get()), 3);
		CPPUNIT_ASSERT(map3.findLeft(a.get())->second == 1);
		CPPUNIT_ASSERT(map3.findLeft(Util::Reference<Util::Bitmap>(b))->second == 2);
		CPPUNIT_ASSERT(map3.findLeft(c)->second == 3);
	}
	
    {
    	Util::BidirectionalUnorderedMap<int,std::string> m;

    	m.insert(1,"a");
    	m.insert(2,"b");
    	m.insert(3,"c");
    	m.eraseLeft(1);
    }

	{ //! different maps for left and right
		typedef Util::BidirectionalMap<
					std::unordered_map<int, Util::Reference<Util::Bitmap> >,
					std::unordered_map<Util::Bitmap*, int>,
					Util::BidirectionalMapPolicies::convertByIdentity, 	// convert_leftKeyToRightMapped_t: int -> int
					Util::BidirectionalMapPolicies::convertByIdentity, 	// convert_rightMappedToLeftKey_t: int <- int
					Util::BidirectionalMapPolicies::convertByGet,  		// convert_leftMappedToRightKey_t: Reference -> Bitmap*
					Util::BidirectionalMapPolicies::convertByIdentity	// convert_rightKeyToLeftMapped_t: Reference <- Bitmap*
					> bitmapRegistry_t;

	    bitmapRegistry_t m;
		Util::Reference<Util::Bitmap> a = new Util::Bitmap(), b = new Util::Bitmap(), c = new Util::Bitmap();
		m.insert(1, a.get());
		m.insert(2, b.get());
		CPPUNIT_ASSERT(m.findRight(a.get())->second == 1);
		CPPUNIT_ASSERT(m.findRight(b.get())->second == 2);
		CPPUNIT_ASSERT(m.findLeft(2)->second == b);
		CPPUNIT_ASSERT(m.findLeft(3) == m.endLeft() );
		
		CPPUNIT_ASSERT(!m.insert(2, c.get()) );
		CPPUNIT_ASSERT(!m.insert(3, a.get()) ); // a already set
		m.eraseRight(a.get()) ;
		CPPUNIT_ASSERT( m.insert(3, a.get()) );

		m.eraseLeft(3) ;
		CPPUNIT_ASSERT( m.insert(3, a.get()) );

	}    
     
	{ //! different maps for left and right (with conversion function)
		typedef Util::BidirectionalMap<
					std::unordered_map<int, Util::Bitmap*>,
					std::unordered_map<Util::Reference<Util::Bitmap> , int, Util::BidirectionalMapPolicies::hashByGet>,
					Util::BidirectionalMapPolicies::convertByIdentity, 	// convert_leftKeyToRightMapped_t: int -> int
					Util::BidirectionalMapPolicies::convertByIdentity, 	// convert_rightMappedToLeftKey_t: int <- int
					Util::BidirectionalMapPolicies::convertByIdentity,  // convert_leftMappedToRightKey_t: Bitmap* -> Reference
					Util::BidirectionalMapPolicies::convertByGet		// convert_rightKeyToLeftMapped_t: Bitmap* <- Reference
				> bitmapRegistry_t;

	    bitmapRegistry_t m;
		Util::Reference<Util::Bitmap> a = new Util::Bitmap(), b = new Util::Bitmap(), c = new Util::Bitmap();
		m.insert(1, a);
		m.insert(2, b.get());
		CPPUNIT_ASSERT(m.findRight(a.get())->second == 1);
		CPPUNIT_ASSERT(m.findRight(b.get())->second == 2);
		CPPUNIT_ASSERT(m.findLeft(2)->second == b.get());
		CPPUNIT_ASSERT(m.findLeft(3) == m.endLeft() );
		
		CPPUNIT_ASSERT(!m.insert(2, c.get()) );
		CPPUNIT_ASSERT(!m.insert(3, a.get()) );
		CPPUNIT_ASSERT( m.insert(3, c.get()) );
		
		// if this compiles, the convert parameters should be correct
		m.eraseLeft(1);
		m.eraseRight(a);
	}    


	{ //! use std::map on one side
		typedef Util::BidirectionalMap<
					std::map<int, Util::Reference<Util::Bitmap> >,
					std::unordered_map<Util::Bitmap*, int>
					> bitmapRegistry_t;

	    bitmapRegistry_t m;
		Util::Reference<Util::Bitmap> a = new Util::Bitmap(), b = new Util::Bitmap(), c = new Util::Bitmap();
		m.insert(1, a.get());
		m.insert(2, b.get());
		CPPUNIT_ASSERT(m.findRight(a.get())->second == 1);
		CPPUNIT_ASSERT(m.findRight(b.get())->second == 2);
		CPPUNIT_ASSERT(m.findLeft(2)->second == b);
		CPPUNIT_ASSERT(m.findLeft(3) == m.endLeft() );
		
		CPPUNIT_ASSERT(!m.insert(2, c.get()) );
		CPPUNIT_ASSERT(!m.insert(3, a.get()) );
		CPPUNIT_ASSERT( m.insert(3, c.get()) );

	}   
}
