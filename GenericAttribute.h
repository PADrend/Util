/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_GENERICATTRIBUTE_H
#define UTIL_GENERICATTRIBUTE_H

#include "References.h"
#include "StringIdentifier.h"
#include "StringUtils.h"
#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace Util{
class BoolAttribute;
template <typename Number> class _NumberAttribute;
template <typename Type> class WrapperAttribute;
template <typename ObjType> class ReferenceAttribute;
template <typename StringType> class _StringAttribute;
typedef _StringAttribute<std::string> StringAttribute;
class GenericAttributeMap;
class GenericAttributeList;

//! @addtogroup generic_attr
//! @{

/**
 * Abstract base class of all generic attributes. A generic attribute is used to
 * store arbitrary types of data into a location.
 */
class GenericAttribute{
	public:
		typedef GenericAttributeMap Map; // alias for GenericAttributeMap
		typedef GenericAttributeList List; // alias for GenericAttributeList
		typedef GenericAttribute attr_t;

		/**
		 * Factory function to create an appropriate GenericAttribute for the
		 * type of the given value.
		 * 
		 * @param value_t Template parameter that decides which type of
		 * GenericAttribute is constructed
		 * @param value Value that is stored in the newly created
		 * GenericAttribute
		 */
		template<typename value_t>
		static typename std::enable_if<std::is_same<typename std::decay<value_t>::type, bool>::value,
									   BoolAttribute *>::type create(value_t && value) {
			return createBool(std::forward<value_t>(value));
		}
		template<typename value_t>
		static typename std::enable_if<(std::is_integral<typename std::decay<value_t>::type>::value || 
											std::is_floating_point<typename std::decay<value_t>::type>::value) &&
											!std::is_same<typename std::decay<value_t>::type, bool>::value,
									   _NumberAttribute<typename std::decay<value_t>::type> *>::type create(value_t && value) {
			return createNumber<typename std::decay<value_t>::type>(std::forward<value_t>(value));
		}
		template<typename value_t>
		static typename std::enable_if<std::is_same<typename std::decay<value_t>::type, std::string>::value,
									   _StringAttribute<std::string> *>::type create(value_t && value) {
			return createString(std::forward<value_t>(value));
		}

		/**
		 * Encapsulate a standard string into a generic attribute.
		 *
		 * @param s Standard string
		 * @return String encapsulated into a generic string attribute
		 */
		inline static StringAttribute * createString(const std::string & s);
		inline static StringAttribute * createString(std::string && s);

		/**
		 * Convert a generic attribute to a string and encapsulate that string
		 * into a generic attribute.
		 *
		 * @param a Arbitrary generic attribute
		 * @return String representation of the generic attribute
		 */
		UTILAPI static StringAttribute * createString(GenericAttribute * a);
		/**
		 * Encapsulate a standard number into a generic attribute.
		 *
		 * @param f Standard number
		 * @return Number encapsulated into a generic number attribute
		 */
		template<typename Number>
		static _NumberAttribute<Number> * createNumber(Number f) {
			return new _NumberAttribute<Number>(f);
		}
		/**
		 * Encapsulate a standard bool into a generic attribute.
		 *
		 * @param f Standard bool
		 * @return Number encapsulated into a generic bool attribute
		 */
		UTILAPI static BoolAttribute * createBool(bool b);
		/**
		 * Encapsulate a Util::Reference to an object into a generic attribute.
		 *
		 * @param f Standard pointer
		 * @return Pointer encapsulated into a generic counted object attribute
		 */
		template<typename ObjType>
		static GenericAttribute * createCountedReference(ObjType *obj){
			return new ReferenceAttribute<ObjType>(obj);
		}
		/**
		 * Encapsulate an arbitrary type into a generic attribute.
		 *
		 * @param value Standard number
		 * @return Arbitrary type encapsulated into a generic undefined attribute
		 */
		template<typename Type>static GenericAttribute * createUndefined(const Type & value){
			return new WrapperAttribute<Type>(value);
		}
		// ----------------------------------

		virtual ~GenericAttribute()	{}

		// ---o
		/**
		 * Create a copy of this object.
		 *
		 * @return Exact copy.
		 */
		virtual attr_t * clone()const=0;
		
		//! returns true iff the object is of the given type.
		template<class specificAttr_t>
		bool isA()const								{	return dynamic_cast<const specificAttr_t*>(this)!=nullptr;	}
		/**
		 * Convert the object to a human-readable string.
		 *
		 * @return String representation.
		 */
		virtual std::string toString()const			{	return std::string("UNDEFINED");	}
		/**
		 * Convert the object to a floating-point value with double precision.
		 *
		 * @return Floating-point representation with double precision.
		 */
		virtual double toDouble()const				{	return 0.0;	}
		/**
		* Convert the object to a floating-point value.
		*
		* @return Floating-point representation.
		*/
		virtual float toFloat()const				{	return toDouble();	}
		/**
		 * Convert the object to an integer value.
		 *
		 * @return Integer representation.
		 */
		virtual int toInt()const					{	return static_cast<int>(toDouble());	}
		/**
		 * Convert the object to an unsigned integer value.
		 *
		 * @return Unsigned integer representation.
		 */
		virtual unsigned int toUnsignedInt()const	{	return static_cast<unsigned int>(toDouble());}
		/**
		 * Convert the object to an boolean value.
		 *
		 * @return Boolean representation.
		 */
		virtual bool toBool()const					{	return static_cast<bool>(toInt());	}
		/**
		 * Convert the object to JavaScript Object Notation (JSON).
		 *
		 * @return JSON representation;
		 */
		virtual std::string toJSON()const			{	return std::string("\"")+StringUtils::escape(toString())+"\"";	}

		template<class specificAttr_t>
		specificAttr_t * toType() {
			return dynamic_cast<specificAttr_t*>(this);
		}
		template<class specificAttr_t>
		const specificAttr_t * toType() const {
			return dynamic_cast<const specificAttr_t*>(this);
		}

		//! Per default, the comparison returns false for unspecified types.
		virtual bool operator==(const GenericAttribute & )const	{	return false;	};
		bool operator!=(const GenericAttribute & other)const	{	return !(*this==other);	};

	protected:
		template<class specificAttr_t>
		static bool _defaultIsEqual(const specificAttr_t * first,const GenericAttribute * second){
			const specificAttr_t * other = dynamic_cast<const specificAttr_t*>(second);
			return other != nullptr ? first->get() == other->get() : false;
		}
		GenericAttribute() = default;
		GenericAttribute(const GenericAttribute &) = default;
		GenericAttribute(GenericAttribute &&) = default;
		GenericAttribute & operator=(const GenericAttribute &) = default;
		GenericAttribute & operator=(GenericAttribute &&) = default;
};

/**
 * Generic attribute that stores the copy of an arbitrary value.
 * \note As the value is not assumed to have an '==' operator, the result of
 *		the '==' operator of two WrapperAttributes is always false!
 */
template<typename Type>
class WrapperAttribute : public GenericAttribute	{
	public:
		typedef WrapperAttribute<Type> attr_t;

		WrapperAttribute() = default;

		WrapperAttribute(const attr_t& other) : GenericAttribute(), data(*other) {}
		WrapperAttribute(attr_t&& other) : GenericAttribute(), data(std::move(*other)) {}

		//! Forwarding constructor
		template<typename firstArg_t, typename ...args, 
					typename = typename	std::enable_if<!std::is_convertible<firstArg_t, const attr_t&>::value && !std::is_convertible<firstArg_t, attr_t&&>::value>::type> // exclude copy and move constructors
		explicit WrapperAttribute(firstArg_t && param1, args&&... params) : 
			GenericAttribute(), data(std::forward<firstArg_t>(param1),std::forward<args>(params)...) {}


		virtual ~WrapperAttribute() {
		}

		/**
		 * Return the stored value.
		 *
		 * @return The value
		 */
		const Type & get() const		{	return data;	}

		/**
		 * Return a reference to the stored value.
		 *
		 * @return Reference to the value
		 */
		Type & ref()					{	return data;	}

		/**
		 * Return a constant reference to the stored value.
		 *
		 * @return Constant reference to the value
		 */
		const Type & ref() const		{	return data;	}

		/*! If the wrapped value is copy-constructible, a copy is returned.
			Otherwise, a std::logic_error is thrown.	*/
		attr_t * clone() const override	{	return _createClone<Type>(nullptr,this);	}
		
		Type & operator*()				{	return data;	}
		const Type & operator*()const	{	return data;	}
		
		
		//! Copy from another object
		attr_t & operator=(attr_t && other) {
			(**this) = *other;
			return *this;
		}
		
		attr_t & operator=(const attr_t & other){
			(**this) = *other;
			return *this;
		}


	private:
		template<typename t2>
		attr_t * _createClone(typename std::enable_if< std::is_copy_constructible<t2>::value >::type*, const attr_t* obj)const{
			return new attr_t(*obj);
		}
		template<typename t2>
		attr_t * _createClone(typename std::enable_if< !std::is_copy_constructible<t2>::value >::type*, const attr_t*)const{
			throw std::logic_error("Trying to clone unclonable WrapperAttribute.");
		}
		
		Type data;
};

/**
 * Empty base class for _NumberAttribute to allow dynamic casting
 *
 * @author Benjamin Eikel
 * @date 2011-06-10
 */
class GenericNumberAttribute : public GenericAttribute {
	public:
		typedef GenericNumberAttribute attr_t;
	protected:
		virtual ~GenericNumberAttribute() {
		}
};

/**
 * Generic attribute to store a number value.
 */
template<typename Number>
class _NumberAttribute : public GenericNumberAttribute {
	public:
		typedef _NumberAttribute<Number> attr_t;

		explicit _NumberAttribute(Number _n) : GenericNumberAttribute(), data(_n) {
		}
		virtual ~_NumberAttribute() {
		}

		/**
		 * Return the stored number value.
		 *
		 * @return Number value
		 */
		Number get() const {
			return data;
		}

		/**
		 * Update the stored number value.
		 *
		 * @param newValue New number value
		 */
		void set(Number newValue) {
			data = newValue;
		}

		attr_t * clone() const override				{	return new attr_t(*this);	}
		std::string toString() const override		{	return StringUtils::toString<Number>(data);	}
		std::string toJSON() const override			{	return toString();	}
		double toDouble() const override			{	return static_cast<double>(data);	}
		float toFloat() const override				{	return static_cast<float>(data);	}
		int toInt() const override					{	return static_cast<int>(data);	}
		unsigned int toUnsignedInt()const override	{	return static_cast<unsigned int>(data);	}
		bool operator==(const GenericAttribute & other) const override{
			return _defaultIsEqual(this,&other);
		}
	private:
		Number data;
};



/**
 * Generic attribute to store a Bool value.
 */
class BoolAttribute:public GenericAttribute{
	public:
		typedef BoolAttribute attr_t;

		explicit BoolAttribute(bool _n) : GenericAttribute(), data(_n) {
		}
		virtual ~BoolAttribute() {
		}

		/**
		 * Return the stored boolean value.
		 *
		 * @return Bool value
		 */
		bool get() const {
			return data;
		}

		/**
		 * Update the stored boolean value.
		 *
		 * @param newValue New boolean value
		 */
		void set(bool newValue) {
			data = newValue;
		}

		attr_t * clone() const override				{	return new attr_t(*this);	}
		std::string toString() const override			{	return data?"true":"false";	}
		std::string toJSON() const override			{	return toString();	}
		double toDouble() const override				{	return data?1:0;	}
		float toFloat() const override				{	return data?1:0;	}
		int toInt() const override					{	return data?1:0;	}
		bool toBool() const override					{	return data;	 }
		unsigned int toUnsignedInt() const override	{	return data?1:0;	}
		bool operator==(const GenericAttribute & other)const override{
			return _defaultIsEqual(this,&other);
		}
	private:
		bool data;
};

/**
 * Empty base class for _StringAttribute to allow dynamic casting
 *
 * @author Benjamin Eikel
 * @date 2011-06-10
 */
class GenericStringAttribute : public GenericAttribute	{
public:
		typedef GenericStringAttribute attr_t;
	protected:
		virtual ~GenericStringAttribute()	{}
};

/**
 * Generic attribute to store a string.
 */
template<typename StringType>
class _StringAttribute : public GenericStringAttribute{
	public:
		typedef _StringAttribute<StringType> attr_t;

		//! Construct the attribute taking a convertible string as parameter.
		template<typename other_string_t, 
				 typename = typename std::enable_if<
										std::is_convertible<
											other_string_t, 
											StringType>::value>::type>
		explicit _StringAttribute(other_string_t && otherString) : 
			GenericStringAttribute(), data(std::forward<other_string_t>(otherString)) {
		}

		virtual ~_StringAttribute() {
		}

		/**
		 * Return a read-only reference to the stored string.
		 *
		 * @return Reference to string
		 */
		const StringType & get() const {
			return data;
		}

		/**
		 * Replace the stored string by the given one.
		 *
		 * @param newString String replacement
		 */
		void set(const StringType & newString) {
			data = newString;
		}

		attr_t * clone() const override				{	return new _StringAttribute<StringType>(*this);	}
		//! returns true iff the string is "true", "TRUE" or "1"
		bool toBool() const override				{	return data=="true" || data=="TRUE" || data=="1";	}
		std::string toString() const override		{	return StringUtils::toString(data);	}
		std::string toJSON() const override			{	return std::string("\"")+StringUtils::escape(toString())+"\"";	}
		double toDouble() const override			{	return StringUtils::toNumber<double>(data);	}
		float toFloat() const override				{	return StringUtils::toNumber<float>(data);	}
		int toInt() const override					{	return StringUtils::toNumber<int>(data);	}
		unsigned int toUnsignedInt() const override	{	return StringUtils::toNumber<unsigned int>(data);	}
		bool operator==(const GenericAttribute & other) const override{
			return _defaultIsEqual(this,&other);
		}
	private:
		StringType data;
};

StringAttribute * GenericAttribute::createString(const std::string & s) {
	return new StringAttribute(s);
}
StringAttribute * GenericAttribute::createString(std::string && s) {
	return new StringAttribute(std::forward<std::string>(s));
}

/**
 * Generic attribute to store an object pointer. It uses reference counting.
 */
template<class ObjType>
class ReferenceAttribute:public WrapperAttribute<Reference<ObjType>>{
	public:
		typedef ReferenceAttribute<ObjType> attr_t;

		explicit ReferenceAttribute(ObjType * _obj) : WrapperAttribute<Reference<ObjType>>(_obj) {
		}
		virtual ~ReferenceAttribute() {
		}

		/**
		 * Return the stored pointer.
		 *
		 * @return Pointer to object
		 */
		ObjType * get()const			{	return WrapperAttribute<Reference<ObjType>>::ref().get();	}

		attr_t * clone() const override	{	return new ReferenceAttribute<ObjType>(*this);	}
		bool operator==(const GenericAttribute & other) const override {
			return WrapperAttribute<Reference<ObjType>>::_defaultIsEqual(this,&other);
		}

};

/**
 * Generic attribute to store a map data structure.
 */
class GenericAttributeMap : public GenericAttribute	{
	private:
		// GenericAttributeMap has unique ownership of its elements
		std::unordered_map<StringIdentifier, std::unique_ptr<GenericAttribute>> m;

	public:
		typedef GenericAttributeMap attr_t;
		
		typedef std::unordered_map<StringIdentifier, std::unique_ptr<GenericAttribute>>::const_iterator const_iterator;
		typedef std::unordered_map<StringIdentifier, std::unique_ptr<GenericAttribute>>::iterator iterator;

		UTILAPI GenericAttributeMap();
		GenericAttributeMap(const GenericAttributeMap &) = delete;
		GenericAttributeMap(GenericAttributeMap &&) = default;
		UTILAPI virtual ~GenericAttributeMap();

		GenericAttributeMap & operator=(const GenericAttributeMap &) = delete;
		GenericAttributeMap & operator=(GenericAttributeMap &&) = default;

		void clear()		{	m.clear();	}
		void setValue(const StringIdentifier & key,GenericAttribute * attr);
		UTILAPI bool unsetValue(const StringIdentifier & key);
		UTILAPI bool contains(const StringIdentifier & key)const;
		UTILAPI GenericAttribute * getValue(const StringIdentifier & stringId)const;
		template<typename T>T * getValue(const StringIdentifier & key)const			{	return dynamic_cast<T*>(getValue(key));	}
		bool getBool(const StringIdentifier & key,const bool defaultValue=false)const	{
			const GenericAttribute * v = getValue(key);
			return v==nullptr ? defaultValue : v->toBool();
		}
		double getDouble(const StringIdentifier & key, double defaultValue = 0.0) const {
			const GenericAttribute * attribute = getValue(key);
			return attribute == nullptr ? defaultValue : attribute->toDouble();
		}
		float getFloat(const StringIdentifier & key,const float defaultValue=0.0f)const	{
			const GenericAttribute * v = getValue(key);
			return v==nullptr ? defaultValue : v->toFloat();
		}
		int getInt(const StringIdentifier & key,const int defaultValue=0)const	{
			const GenericAttribute * v = getValue(key);
			return v==nullptr ? defaultValue : v->toInt();
		}
		unsigned int getUInt(const StringIdentifier & key,const unsigned int defaultValue=0)const	{
			const GenericAttribute * v = getValue(key);
			return v==nullptr ? defaultValue : v->toUnsignedInt();
		}

		UTILAPI std::string getString(const StringIdentifier & key,const std::string & defaultValue="")const;
		UTILAPI void setString(const StringIdentifier & key,const std::string & value);

		const_iterator begin()const				{	return m.begin();	}
		const_iterator end()const				{	return m.end();	 }
		size_t size()const						{	return m.size();	}
		bool empty() const						{	return m.empty();	}

		UTILAPI GenericAttributeMap * clone() const override;
		UTILAPI std::string toJSON() const override;
		UTILAPI std::string toString() const override;
		UTILAPI bool operator==(const GenericAttribute &) const override;
};

/**
 * Generic attribute to store a list data structure.
 */
class GenericAttributeList : public GenericAttribute	{
	private:
		// GenericAttributeList has unique ownership of its elements
		std::deque<std::unique_ptr<GenericAttribute>> l;

	public:
		typedef GenericAttributeList attr_t;

		typedef std::deque<std::unique_ptr<GenericAttribute>>::const_iterator const_iterator;
		typedef std::deque<std::unique_ptr<GenericAttribute>>::iterator iterator;

		UTILAPI GenericAttributeList();
		GenericAttributeList(const GenericAttributeList &) = delete;
		GenericAttributeList(GenericAttributeList &&) = default;
		UTILAPI virtual ~GenericAttributeList();

		GenericAttributeList & operator=(const GenericAttributeList &) = delete;
		GenericAttributeList & operator=(GenericAttributeList &&) = default;

		void clear()							{	l.clear();			}
		void push_front(GenericAttribute * attr){	l.emplace_front(attr);	}
		void push_back(GenericAttribute * attr)	{	l.emplace_back(attr);	}
		size_t size() const						{	return l.size();	}
		bool empty() const						{	return l.empty();	}

		iterator begin()						{	return l.begin();	}
		const_iterator begin()const				{	return l.begin();	}
		iterator end()							{	return l.end();	 }
		const_iterator end()const				{	return l.end();	 }
		GenericAttribute * front()				{	return l.front().get();	}
		const GenericAttribute * front() const	{	return l.front().get();	}
		GenericAttribute * back()				{	return l.back().get();	}
		const GenericAttribute * back() const	{	return l.back().get();	}

		iterator erase(iterator position)		{	return l.erase(position);	}
		iterator erase(iterator first,iterator last)		{	return l.erase(first,last);	}

		UTILAPI std::string implode(const std::string & separator)const;

		/**
		 * Returns the element with given index (or nullptr if none exists).
		 */
		UTILAPI GenericAttribute * at(int index)const;

		UTILAPI GenericAttributeList * clone() const override;
		UTILAPI std::string toJSON() const override;
		UTILAPI std::string toString() const override;
		UTILAPI bool operator==(const GenericAttribute &) const override;
};

//! @}

}

#endif /* UTIL_GENERICATTRIBUTE_H */
