/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef BIDIRECTIONALUNORDEREDMAP_H
#define BIDIRECTIONALUNORDEREDMAP_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <utility>

namespace Util {

//! @addtogroup data_structures
//! @{

/*! Collection of policies used for BinaryMaps */
namespace BidirectionalMapPolicies{

/*! [functor] Returns the given value of type a_t without changes. The template
 * parameter b_t is ignored (but it should be equal a_t).
 */
template<typename a_t,typename b_t>
struct convertByIdentity{
	const a_t & operator()(const a_t & a)const{
		return a;
	}
};

/*! [functor] Returns the result of the .get() method of the given value. Can be used to
 *	convert a smart pointer (Reference or Weak
 */
template<typename a_t,typename b_t>
struct convertByGet{
	b_t operator()(const a_t & a)const{
		return a.get();
	}
};

/*! [functor] Used to hash values stored in a smart pointer (Reference or WeakPointer) */
struct hashByGet {
	template<typename ref_t>
	size_t operator() (const ref_t & r) const {
		return reinterpret_cast<size_t>(r.get());
	}
};

struct hashEnum{
	template<typename T>
	size_t operator() (const T & t) const {
		return static_cast<size_t>(t);
	}
};

}

/*!
 * Class for storing bidirectional mappings between objects.
 * The types of the used maps are given as template parameters. If the types of the values differ for
 * the two maps (e.g. leftToRight: int -> Object*   rightToLeft: int <- Reference<Object>), corresponding
 * conversion functors may have to be given as additional parameters (if an implicit conversion is not available)
 * \code
 * // leftToRight: int -> Util::Bitmap*
 * // rightToLeft: int <- Util::Reference<Util::Bitmap>
 * // the additional conversion parameters are necessary as there is no implicit cast from Util::Reference<Util::Bitmap> to Util::Bitmap*
 * typedef Util::BidirectionalMap<
 *					std::unordered_map<int, Util::Bitmap*>,
 *					std::unordered_map<Util::Reference<Util::Bitmap> , int, Util::BidirectionalMapPolicies::hashByGet>,
 * 					Util::BidirectionalMapPolicies::convertByIdentity, 	// convert_leftKeyToRightMapped_t: int -> int
 *					Util::BidirectionalMapPolicies::convertByIdentity, 	// convert_rightMappedToLeftKey_t: int <- int
 *					Util::BidirectionalMapPolicies::convertByGet,  		// convert_leftMappedToRightKey_t: Reference -> Bitmap*
 *					Util::BidirectionalMapPolicies::convertByIdentity	// convert_rightKeyToLeftMapped_t: Reference <- Bitmap*
 *					> 	bitmapRegistry_t;
 *
 * \endcode
 * \note All iterators are const_iterators to ensure that both internal maps stay consistent.
 * \note Have a look at BidirectionalUnorderedMap, which should be what you are looking for in most cases.
 */
template <
	class map_leftToRight_t,
	class map_rightToLeft_t,
	template<typename a_t,typename b_t> class convert_leftKeyToRightMapped_t = BidirectionalMapPolicies::convertByIdentity,
	template<typename a_t,typename b_t> class convert_rightMappedToLeftKey_t = BidirectionalMapPolicies::convertByIdentity,
	template<typename a_t,typename b_t> class convert_leftMappedToRightKey_t = BidirectionalMapPolicies::convertByIdentity,
	template<typename a_t,typename b_t> class convert_rightKeyToLeftMapped_t = BidirectionalMapPolicies::convertByIdentity
>
class BidirectionalMap
{
private:
	map_leftToRight_t leftToRight;
	map_rightToLeft_t rightToLeft;

public:
	typedef typename map_leftToRight_t::key_type leftToRight_key_t;
	typedef typename map_leftToRight_t::value_type leftToRight_value_t;
	typedef typename map_leftToRight_t::mapped_type leftToRight_mapped_t;

	typedef typename map_rightToLeft_t::key_type rightToLeft_key_t;
	typedef typename map_rightToLeft_t::value_type rightToLeft_value_t;
	typedef typename map_rightToLeft_t::mapped_type rightToLeft_mapped_t;


	//! the size type of this map
	typedef typename map_leftToRight_t::size_type size_type;

	//! pair of left and right value
	typedef typename map_leftToRight_t::value_type value_type;

	//! const_iterator for the left to right mapping
	typedef typename map_leftToRight_t::const_iterator const_iterator_left;

	//! const_iterator for the right to left mapping
	typedef typename map_rightToLeft_t::const_iterator const_iterator_right;

	/*! normal iterator only declared for convenience,
	 *  they are also const because otherwise it would be
	 *  possible to change only one of the internal maps */
	typedef const_iterator_left iterator_left;
	/*! normal iterator only declared for convenience,
	 *  they are also const because otherwise it would be
	 *  possible to change only one of the internal maps */
	typedef const_iterator_right iterator_right;

public:

	//! ctor, constructs an empty map
	BidirectionalMap()
	{}

	/*!
	 * copy ctor
	 * @param other the map to be copied
	 */
	BidirectionalMap(const BidirectionalMap & other):
		leftToRight(other.leftToRight), rightToLeft(other.rightToLeft)
	{}

	//! dtor
	~BidirectionalMap()
	{}

	//! assignment operator, assigns the content of other map to this map
	BidirectionalMap & operator=(const BidirectionalMap & other) {
		leftToRight = other.leftToRight;
		rightToLeft = other.rightToLeft;
		return *this;
	}
	
	//! @return an iterator to the beginning of the left to right map
	const_iterator_left beginLeft() const {
		return leftToRight.begin();
	}

	//! @return an iterator to the beginning of the right to left map
	const_iterator_right beginRight() const {
		return rightToLeft.begin();
	}

	//! removes all enties from the map
	void clear() {
		leftToRight.clear();
		rightToLeft.clear();
	}


	//! @return true iff the map is empty
	bool empty() const {
		return leftToRight.empty();
	}

	//! @return an iterator to the end of the left to right map
	const_iterator_left endLeft() const {
		return leftToRight.end();
	}

	//! @return an iterator to the end of the right to left map
	const_iterator_right endRight() const {
		return rightToLeft.end();
	}

	/*!
	 * @param _left an iterator of the left to right map that points to the value to be deleted
	 * @return an iterator to the value following to _left
	 */
	const_iterator_left eraseLeft(const const_iterator_left & _left) {
		if(_left == leftToRight.end())
			return leftToRight.end();

		typedef convert_leftMappedToRightKey_t<leftToRight_mapped_t,rightToLeft_key_t> c_t;
		c_t c;
		rightToLeft.erase( c(_left->second) ); //! \note if you get an error in this line, you probably need to adjust the 'convert_leftMappedToRightKey_t'-template parameter.
		return leftToRight.erase(_left);
	}


	/*!
	 * @param _right an iterator of the right to left map that points to the value to be deleted
	 * @return an iterator to the value following to _right
	 */
	const_iterator_right eraseRight(const const_iterator_right & _right) {
		if(_right == rightToLeft.end())
			return rightToLeft.end();

		typedef convert_rightMappedToLeftKey_t<rightToLeft_mapped_t,leftToRight_key_t> c_t;
		c_t c;
		leftToRight.erase( c(_right->second) ); //! \note if you get an error in this line, you probably need to adjust the 'convert_rightMappedToLeftKey_t'-template parameter.
		return rightToLeft.erase(_right);
	}

	/*!
	 * @param _left a left key to be deleted from the mapping
	 * @return one if the key was found in the map, zero otherwise
	 */
	size_type eraseLeft(const leftToRight_key_t & _left) {
		const_iterator_left li = leftToRight.find(_left);
		if(li == leftToRight.end())
			return 0;
		eraseLeft(li);
		return 1;
	}


	/*!
	 * @param _right a right key to be deleted from the mapping
	 * @return one if the key was found in the map, zero otherwise
	 */
	size_type eraseRight(const rightToLeft_key_t & _right) {
		const_iterator_right ri = rightToLeft.find(_right);
		if(ri == rightToLeft.end())
			return 0;
		eraseRight(ri);
		return 1;
	}

	/*!
	 * searches for a left key in the left to right mapping
	 * @param _left a left key
	 * @return an iterator pointing to the value found in the left to right mapping or to its end if the key does not exist
	 */
	const_iterator_left findLeft(const leftToRight_key_t & _left) const {
		return leftToRight.find(_left);
	}

	/*!
	 * searches for a right key in the right to left mapping
	 * @param _left a right key
	 * @return an iterator pointing to the value found in the right to left mapping or to its end if the key does not exist
	 */
	const_iterator_right findRight(const rightToLeft_key_t & _right) const {
		return rightToLeft.find(_right);
	}

	/*!
	 * delegated to insert(x.first, x.second)
	 */
	bool insert(const value_type & x) {
		return insert(x.first, x.second);
	}


	/*!
	 * tries to insert a new pair of keys into the map
	 * @param _left a left key
	 * @param _right a right key
	 * @return true iff the pair of keys has been inserted into the map
	 * @note
	 *  the map is only changed if and only if
	 *  neither _left exists in left to right mapping
	 *  nor _right exists in right to left mapping
	 * @note
	 *  if you want to insert existing keys,
	 *  please erase them before inserting
	 */
	bool insert(const leftToRight_key_t & _left, const rightToLeft_key_t & _right) {
		const_iterator_left li = leftToRight.find(_left);
		if(li != leftToRight.end())
			return false;
		const_iterator_right ri = rightToLeft.find(_right);
		if(ri != rightToLeft.end())
			return false;

		// insert right to left
		typedef convert_rightKeyToLeftMapped_t<rightToLeft_key_t,leftToRight_mapped_t> c1_t;
		c1_t c1;
		leftToRight.insert(std::make_pair(_left, c1(_right) )); //! \note if you get an error in this line, you probably need to adjust the 'convert_rightKeyToLeftMapped_t'-template parameter.

		// insert left to right
		typedef convert_leftKeyToRightMapped_t<leftToRight_key_t,rightToLeft_mapped_t> c2_t;
		c2_t c2;
		rightToLeft.insert(std::make_pair(_right, c2(_left) )); //! \note if you get an error in this line, you probably need to adjust the 'convert_leftKeyToRightMapped_t'-template parameter.
		return true;
	}


	/*!
	 * @return the maximum number of elements that the BidirectionalUnorderedMap container object can hold.
	 */
	size_type max_Size() {
		return std::min(leftToRight.max_Size(), rightToLeft.max_Size());
	}

	/*!
	 * @return the number of elements currently stored in this mapping
	 */
	size_type size() const {
		return leftToRight.size();
	}

	/*!
	 * Exchanges the content of the container with the content of other. Sizes may differ.
	 * @param other another BidirectionalUnorderedMap object containing elements of the same type
	 */
	void swap(BidirectionalMap & other) {
		leftToRight.swap(other.leftToRight);
		rightToLeft.swap(other.rightToLeft);
	}
	 
	//friend const_iterator_left begin(BidirectionalMap & c)			{	return c.beginLeft();	}
	friend const_iterator_left begin(const BidirectionalMap & c) 	{	return c.beginLeft();	}
	//friend const_iterator_left end(BidirectionalMap & c)			{	return c.endLeft();	}
	friend const_iterator_left end(const BidirectionalMap & c) 		{	return c.endLeft();	}
};


/*!
 * class for storing bidirectional mappings between objects
 * internaly uses two unordered maps for left to right and right to left mapping
 * all iterators are const_iterators to ensure that both internal maps stay consistent
 */
template <
	class Key_Left,
	class Key_Right,
	class Hash_Left = std::hash<Key_Left>, //!< if Key_Left is a smart pointer (Reference or WeakPointer) use BidirectionalMapPolicies::hashByGet here.
	class Hash_Right = std::hash<Key_Right>, //!< if Key_Right is a smart pointer (Reference or WeakPointer) use BidirectionalMapPolicies::hashByGet here.
	class Pred_Left = std::equal_to<Key_Left>,
	class Pred_Right = std::equal_to<Key_Right>
>
class BidirectionalUnorderedMap : public BidirectionalMap<
			std::unordered_map<Key_Left, Key_Right, Hash_Left, Pred_Left>,
			std::unordered_map<Key_Right, Key_Left, Hash_Right, Pred_Right> >
{
	typedef BidirectionalMap<std::unordered_map<Key_Left, Key_Right, Hash_Left, Pred_Left>,  std::unordered_map<Key_Right, Key_Left, Hash_Right, Pred_Right> >super_t;
public:
	BidirectionalUnorderedMap() : super_t(){}
	BidirectionalUnorderedMap(BidirectionalUnorderedMap & other) : super_t(other){}

	~BidirectionalUnorderedMap(){}
};


//! @}
}

#endif // BIDIRECTIONALUNORDEREDMAP_H
