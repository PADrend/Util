/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius J�hn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef OBJECTEXTENSION_H_INCLUDED
#define OBJECTEXTENSION_H_INCLUDED

#include "AttributeProvider.h"
#include "GenericAttribute.h"
#include "StringIdentifier.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace Util {

/*! @defgroup objext ObjectExtensions
	ObjectExtensions can be used to dynamically extend objects inheriting from AttributeProvider
	with data and functionality at runtime. Internally, it is a unified interface
	for several different GenericAttributes.
	There are two types of extensions: 1. non polymorphic and 2. polymorphic.
	
	1. non polymorphic extensions ...
		- are based on ordinary classes (without any virtual function).
		- must use the PROVIDES_TYPE_NAME_NV(NameOfTheExtension) macro,
			OR implement a static function getExtensionName() that returns a StringIdentifier with the extension's name.
			OR the extension's name must be provided for each function
		- must provide a copy constructor.
		- do not introduce additional overhead in comparison to a normal GenericAttribute for 
			storing a value.
		- may provide constructors with arbitrary many parameters.
			
	\code
		class MyExtension{
				PROVIDES_TYPE_NAME_NV(MyExtension)
				int someValue;
			public:
				MyExtension(int _someValue) : someValue(_someValue){}
				int getValue()const	{	return someValue;	}
		}
		...
		MyObject * a = new MyObject; // MyObject---|>AttributeProvider
		Util::addObjectExtension<MyExtension>(42);
		...
		Util::requireObjectExtension<MyExtension>(a)->getValue();
	\endcode
	
	2. polymorphic extensions ...
		- introduce an additional memory overhead (pointer + polymorphic object header)
		- may also provide constructors with arbitrary many parameters.
		- must provide a base class that contains:
			- the PROVIDES_TYPE_NAME(NameOfTheExtensionBase) macro (or getExtensionName(), see above)
			- a typedef extentsionBase_t referening to the class itself
			- a (possibly abstract) clone() method returning a pointer of extentsionBase_t
		- Specific extension classes ...
			- inherit from extentsionBase_t.
			- must implement the clone() method.
			- should NOT contain the PROVIDES_TYPE_NAME macro.
	\code
		class MyExtensionBase{
				PROVIDES_TYPE_NAME(MyExtensionBase)
			public:
				typedef MyExtensionBase extentsionBase_t;
				virtual float calcSomething()const = 0;
				virtual MyExtensionBase * clone()const = 0;
		};

		//! ---|> MyExtensionBase
		class MyExt1 : public MyExtensionBase{
			public:
				//! ---|> MyExtensionBase
				float calcSomething() const override {	return 27.0f;	};
				//! ---|> MyExtensionBase
				MyExt1 * clone()const				{	return new MyExt1;	}
		};
		
		...
		MyObject * a = new MyObject; // MyObject---|>AttributeProvider
		Util::addObjectExtension<MyExt1>();
		...
		Util::requireObjectExtension<MyExtensionBase>(a)->calcSomething();
	\endcode
	@{
*/
// ------------------------------------------------------------------------------
//! @internal
namespace _Internals{
	
template<typename extension_t> 
Util::StringIdentifier getObjectExtensionName(decltype(extension_t::getExtensionName) = nullptr){
	return extension_t::getExtensionName();
}
template<typename extension_t> 
Util::StringIdentifier getObjectExtensionName(decltype(extension_t::getClassId) = nullptr){
	return extension_t::getClassId();
}

// ---------------------------
//// non polymorphic object extension

template<typename extension_t,typename object_t,typename ...args>
extension_t * addObjectExtension(typename std::enable_if<!std::is_polymorphic<extension_t>::value>::type*,
								const Util::StringIdentifier & extName,object_t * obj,args&&... params){
	typedef Util::WrapperAttribute<extension_t> attr_t;
	attr_t * attr( new attr_t(std::forward<args>(params)...) );
	obj->setAttribute(extName,attr);
	return &attr->ref();
}

template<typename extension_t> 
extension_t * getObjectExtension(typename std::enable_if<!std::is_polymorphic<extension_t>::value>::type*,
								const Util::StringIdentifier & extName,const Util::AttributeProvider * obj){
	auto attr = obj->getAttribute<Util::WrapperAttribute<extension_t>>(extName);
	return attr ? &attr->ref() : nullptr;
}
// ---------------------------
//// polymorphic object extension

template<class extension_t>
struct PolymorphicObjectExtensionWrapper : public GenericAttribute{
	std::unique_ptr<extension_t> ext;
	explicit PolymorphicObjectExtensionWrapper(extension_t * _ext) : ext(_ext){}

	//! ---|> GenericAttribute
	PolymorphicObjectExtensionWrapper * clone() const override{
		return new PolymorphicObjectExtensionWrapper(ext.get()->clone());
	}
	extension_t * ref()const		{	return ext.get();	}
};

template<typename extension_t,typename object_t,typename ...args>
extension_t * addObjectExtension(typename std::enable_if<std::is_polymorphic<extension_t>::value>::type*,
								const Util::StringIdentifier & extName,object_t * obj,args&&... params){
	auto attr = new PolymorphicObjectExtensionWrapper<typename extension_t::extentsionBase_t>(
						new extension_t(std::forward<args>(params)...));
	obj->setAttribute(extName,attr);
	return static_cast<extension_t*>(attr->ref());
}

template<typename extension_t> 
extension_t * getObjectExtension(typename std::enable_if<std::is_polymorphic<extension_t>::value>::type*,
								const Util::StringIdentifier & extName,const Util::AttributeProvider * obj){
	auto attr = obj->getAttribute<PolymorphicObjectExtensionWrapper<typename extension_t::extentsionBase_t>>(extName);
	return attr ? attr->ref() : nullptr;
}


}
// ---------------------------------------------------------------------------

/*!	Create and add an extension of type extension_t to the given @p object.
	- Additional parameters are passed to the extension's constructor.
	- The extension's name is inquired by calling extension_t::getObjectExtensionName() or extension_t::getClassId()
	- The created extension is returned.	*/
template<typename extension_t,typename object_t,typename ...args>
extension_t * addObjectExtension(object_t * obj,args&&... params){
	return _Internals::addObjectExtension<extension_t,object_t>(nullptr,
						_Internals::getObjectExtensionName<extension_t>(),obj,std::forward<args>(params)...);
}

/*!	Create and add an extension of type extension_t to the given @p object named @p extName.
	@see addObjectExtension(obj, params...)	*/
template<typename extension_t,typename object_t,typename ...args>
extension_t * addObjectExtension(const StringIdentifier & extName,object_t * obj,args&&... params){
	return _Internals::addObjectExtension<extension_t,object_t>(nullptr,extName,obj,std::forward<args>(params)...);
}

/*!	If the given @p object has a extension of type extension_t, it is returned; nullptr otherwise.
	- The extension's name is inquired by calling extension_t::getObjectExtensionName() or extension_t::getClassId() */
template<typename extension_t> 
extension_t * getObjectExtension(const Util::AttributeProvider * obj){
	return getObjectExtension<extension_t>(_Internals::getObjectExtensionName<extension_t>(),obj);
}

/*!	If the given @p object has a extension of type extension_t named @p extName, it is returned; nullptr otherwise.
	@see getObjectExtension(obj)  */
template<typename extension_t> 
extension_t * getObjectExtension(const StringIdentifier & extName,const Util::AttributeProvider * obj){
	return _Internals::getObjectExtension<extension_t>(nullptr,extName,obj);
}

/*!	If the given @p object has a extension of type extension_t, it is returned; otherwise, an exception is thrown.
	- The extension's name is inquired by calling extension_t::getObjectExtensionName() or extension_t::getClassId().	*/
template<typename extension_t> 
extension_t * requireObjectExtension(const Util::AttributeProvider * obj){
	return requireObjectExtension<extension_t>(_Internals::getObjectExtensionName<extension_t>(),obj);
}

/*!	If the given @p object has a extension of type extension_t named @p extName, it is returned; otherwise, an exception is thrown.
	- The extension's name is inquired by calling extension_t::getObjectExtensionName() or extension_t::getClassId().	*/
template<typename extension_t> 
extension_t * requireObjectExtension(const StringIdentifier & extName,const Util::AttributeProvider * obj){
	extension_t * ext = getObjectExtension<extension_t>(extName,obj);
	if(!ext)
		throw std::invalid_argument("Required object extension '"+extName.toString()+"' not found.");
	return ext;
}

/*!	Returns true iff  the @p object has a extension of type extension_t.
	- The extension's name is inquired by calling extension_t::getObjectExtensionName() or extension_t::getClassId().	*/
template<typename extension_t> bool hasObjectExtension(const Util::AttributeProvider * obj){
	return getObjectExtension<extension_t>(obj)!=nullptr;
}

//!	Returns true iff  the @p object has a extension of type extension_t named @p extName.
template<typename extension_t> bool hasObjectExtension(const StringIdentifier & extName,const Util::AttributeProvider * obj){
	return getObjectExtension<extension_t>(extName,obj)!=nullptr;
}

//! @}
}
#endif // OBJECTEXTENSION_H_INCLUDED
