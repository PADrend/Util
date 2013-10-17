/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2013 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef TRISTATE_H
#define TRISTATE_H

namespace Util{
	
/**
 * @brief Class that can hold one of three values: true, fals, undefined
 *
 * this class is kind of an extended bool and can additionally have the value undefined
 * it can be constructed from, assigned from and compared to bool values and other TriStates
 *
 * as well as bool on most systems this class uses one byte of memory
 *
 * @author Ralf Petring
 * @date 2013-01-16
 */
class TriState {

private:
	
	enum STATE : uint8_t {OFF = 0, ON = 1, UNDEFINED = 2} state;

public:

	//! default constructor, constructs a new TriState with value undefined
	TriState() : state(UNDEFINED){
	}

	//! constructor, constructs a new TriState with the given bool as value
	explicit TriState(const bool b) : state(b?ON:OFF){
	}

	/**
	 * assignment operator for bool
	 * @note this does not remove default constructors and default assignment operators etc.
	 */
	TriState & operator=(const bool & b) {
		state = b?ON:OFF;
		return *this;
	}
	
	/**
	 * equality operator
	 * @param other any TriState
	 * @return true iff other is equal to this
	 */
	bool operator==(const TriState & other) const {
		return state == other.state;
	}

	/**
	 * inequality operator
	 * @param other any TriState
	 * @return true iff other is not equal to this
	 */
	bool operator!=(const TriState & other) const {
		return state != other.state;
	}

	/**
	 * equality operator to bool
	 * @param other any bool
	 * @return true iff other is equal to this
	 */
	bool operator==(const bool other) const {
		return state == (other?ON:OFF);
	}

	/**
	 * inequality operator to bool
	 * @param other any bool
	 * @return true iff other is not equal to this
	 */
	bool operator!=(const bool other) const {
		return state != (other?ON:OFF);
	}

	/**
	 * @return true iff current value is true
	 */
	bool isTrue() const {
		return state == ON;
	}

	/**
	 * @return true iff current value is false
	 */
	bool isFalse() const {
		return state == OFF;
	}

	/**
	 * @return true iff current value is undefined
	 */
	bool isUndefined() const {
		return state == UNDEFINED;
	}

	/**
	 * @return true iff current value is true OR false
	 */
	bool isDefined() const {
		return state != UNDEFINED;
	}

	/**
	 * sets the current value to undefined
	 */
	void undefine(){
		state = UNDEFINED;
	}
};

}

#endif // TRISTATE_H
