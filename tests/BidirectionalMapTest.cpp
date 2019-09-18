/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2013 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "BidirectionalMap.h"
#include "References.h"
#include "Graphics/Bitmap.h"

#include <catch2/catch.hpp>

#include <map>
#include <iostream>

TEST_CASE("BidirectionalMapTest", "[BidirectionalMapTest]") {
	Util::BidirectionalUnorderedMap<int,int> map;

	//! insertion
	REQUIRE(map.insert(1,2));
	REQUIRE(map.insert(3,4));
	REQUIRE(!map.insert(1,5));
	REQUIRE(!map.insert(5,4));
	REQUIRE(map.insert(std::make_pair<int,int>(5,5)));

	//! searching
	REQUIRE(map.findLeft(1) != map.endLeft());
	REQUIRE(map.findLeft(1)->second == 2);
	REQUIRE(map.findRight(2) != map.endRight());
	REQUIRE(map.findRight(2)->second == 1);

	//! erasing
	REQUIRE(map.eraseRight(2) == 1);
	REQUIRE(map.findRight(2) == map.endRight());
	REQUIRE(map.findLeft(1) == map.endLeft());
	REQUIRE(map.eraseLeft(3) == 1);
	REQUIRE(map.findLeft(3) == map.endLeft());
	REQUIRE(map.findRight(4) == map.endRight());

	//! size & clear
	REQUIRE(map.size() == 1);
	REQUIRE(!map.empty());
	map.eraseRight(5);
	REQUIRE(map.empty());
	map.insert(1,2);
	map.insert(3,4);
	map.clear();
	REQUIRE(map.empty());

	//! swap
	Util::BidirectionalUnorderedMap<int,int> map2;
	map.insert(1,1);
	map.insert(2,2);
	map2.insert(1,2);
	map.swap(map2);
	REQUIRE(map2.findLeft(1)->second == 1);
	REQUIRE(map2.findLeft(2)->second == 2);
	REQUIRE(map.findLeft(1)->second == 2);
	REQUIRE(map2.size()==2);
	REQUIRE(map.size()==1);
	
	SECTION("util references") {
		Util::BidirectionalUnorderedMap<Util::Reference<Util::Bitmap>,int, Util::BidirectionalMapPolicies::hashByGet > map3;
		Util::Reference<Util::Bitmap> a = new Util::Bitmap(), b = new Util::Bitmap(), c = new Util::Bitmap();
		map3.insert(a, 1);
		map3.insert(b.get(), 2);
		map3.insert(Util::Reference<Util::Bitmap>(c.get()), 3);
		REQUIRE(map3.findLeft(a.get())->second == 1);
		REQUIRE(map3.findLeft(Util::Reference<Util::Bitmap>(b))->second == 2);
		REQUIRE(map3.findLeft(c)->second == 3);
	}
	
	{
		Util::BidirectionalUnorderedMap<int,std::string> m;

		m.insert(1,"a");
		m.insert(2,"b");
		m.insert(3,"c");
		m.eraseLeft(1);
	}

	SECTION("different maps for left and right") {
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
		REQUIRE(m.findRight(a.get())->second == 1);
		REQUIRE(m.findRight(b.get())->second == 2);
		REQUIRE(m.findLeft(2)->second == b);
		REQUIRE(m.findLeft(3) == m.endLeft() );
		
		REQUIRE(!m.insert(2, c.get()) );
		REQUIRE(!m.insert(3, a.get()) ); // a already set
		m.eraseRight(a.get()) ;
		REQUIRE( m.insert(3, a.get()) );

		m.eraseLeft(3) ;
		REQUIRE( m.insert(3, a.get()) );

	}
		
	SECTION("different maps for left and right (with conversion function)") {
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
		REQUIRE(m.findRight(a.get())->second == 1);
		REQUIRE(m.findRight(b.get())->second == 2);
		REQUIRE(m.findLeft(2)->second == b.get());
		REQUIRE(m.findLeft(3) == m.endLeft() );
		
		REQUIRE(!m.insert(2, c.get()) );
		REQUIRE(!m.insert(3, a.get()) );
		REQUIRE( m.insert(3, c.get()) );
		
		// if this compiles, the convert parameters should be correct
		m.eraseLeft(1);
		m.eraseRight(a);
	}    


	SECTION("use std::map on one side") {
		typedef Util::BidirectionalMap<
					std::map<int, Util::Reference<Util::Bitmap> >,
					std::unordered_map<Util::Bitmap*, int>
					> bitmapRegistry_t;

			bitmapRegistry_t m;
		Util::Reference<Util::Bitmap> a = new Util::Bitmap(), b = new Util::Bitmap(), c = new Util::Bitmap();
		m.insert(1, a.get());
		m.insert(2, b.get());
		REQUIRE(m.findRight(a.get())->second == 1);
		REQUIRE(m.findRight(b.get())->second == 2);
		REQUIRE(m.findLeft(2)->second == b);
		REQUIRE(m.findLeft(3) == m.endLeft() );
		
		REQUIRE(!m.insert(2, c.get()) );
		REQUIRE(!m.insert(3, a.get()) );
		REQUIRE( m.insert(3, c.get()) );
	}   
}
