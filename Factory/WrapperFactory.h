/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_FACTORY_WRAPPERFACTORY_H
#define UTIL_FACTORY_WRAPPERFACTORY_H

#include "FallbackPolicies.h"
#include <functional>
#include <cstddef>
#include <map>

namespace Util {

//! @addtogroup factory
//! @{

/**
 * @brief Generic factory for wrapper objects
 *
 * The factory template can be instaniated to generate different kinds of factories.
 * The generated factory takes objects of type @a InternalType, and generates a wrapper object of type @a WrapperType.
 *
 * @tparam InternalType Base type for all objects that are stored inside the wrapper objects of type @a WrapperType
 * @tparam WrapperType Base type for all objects that are generated by the factory
 * @tparam IdentifierType Type of the identifier that specifies which object creator to call
 * @tparam ObjectCreator Type for callable entities that take an object of type @a InternalType, and generate an object of type @a WrapperType
 * @tparam FallbackPolicy Template with a function @a onUnknownType() that handles the case that the requested object type was not found
 * @author Benjamin Eikel
 * @date 2012-01-31
 * @see Patterns from Chapter 8 in Andrei Alexandrescu: Modern C++ Design. Addison-Wesley, 2001.
 */
template < class InternalType,
		 class WrapperType,
		 typename IdentifierType,
		 typename ObjectCreator = std::function<WrapperType (const InternalType &)>,
		 template<class, typename> class FallbackPolicy = FallbackPolicies::ExceptionFallback >
class WrapperFactory {
	private:
		typedef std::map<IdentifierType, ObjectCreator> registrations_t;
		registrations_t registrations;
	public:
		typedef FallbackPolicy<WrapperType, IdentifierType> fallbackPolicy_t;
		fallbackPolicy_t fallbackPolicy;
		WrapperFactory() : fallbackPolicy() {
		}
		WrapperFactory(fallbackPolicy_t policy) : fallbackPolicy(policy) {
		}

		bool registerType(const IdentifierType & id, ObjectCreator creator) {
			return registrations.insert(typename registrations_t::value_type(id, creator)).second;
		}

		bool unregisterType(const IdentifierType & id) {
			return registrations.erase(id);
		}

		WrapperType create(const IdentifierType & id, const InternalType & object) {
			typename registrations_t::const_iterator it = registrations.find(id);
			if(it != registrations.end()) {
				return (it->second)(object);
			}
			return fallbackPolicy.onUnknownType(std::bind(&WrapperFactory::create, this, std::placeholders::_1, object), id);
		}
};

/**
 * @brief Generic creator for wrapper objects
 *
 * This functor takes an object of type @a InternalBaseType.
 * By using RTTI, it checks if the object is of subtype @a InternalDerivedType.
 * If this is the case, it returns a new wrapper of type @a WrapperDerivedType with the object as constructor parameter.
 *
 * @note Only use this creator for polymorphic types @a InternalBaseType and @a InternalDerivedType.
 * @tparam InternalBaseType Base type of internal objects that are given to the creator
 * @tparam InternalDerivedType Concrete type of internal objects for which a wrapper shall be created
 * @tparam WrapperDerivedType Concrete type of wrapper object that will be created
 * @author Benjamin Eikel
 * @date 2012-01-31
 */
template<class InternalBaseType, class InternalDerivedType, class WrapperDerivedType>
class PolymorphicWrapperCreator {
	public:
		WrapperDerivedType * operator()(InternalBaseType * baseObject) {
			if(InternalDerivedType * derivedObject = dynamic_cast<InternalDerivedType *>(baseObject)) {
				return new WrapperDerivedType(derivedObject);
			}
			return nullptr;
		}
};

//! @}
}

#endif /* UTIL_FACTORY_WRAPPERFACTORY_H */
