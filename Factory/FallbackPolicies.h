/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_FACTORY_FALLBACKPOLICIES_H
#define UTIL_FACTORY_FALLBACKPOLICIES_H

#include <functional>
#include <cstddef>
#include <exception>
#include <ostream>

namespace Util {
namespace FallbackPolicies {

/**
 * @brief Possible fallback policy for a factory
 *
 * In case of an unknown type, an exception is thrown.
 *
 * @see Factory or WrapperFactory for description of template parameters
 * @author Benjamin Eikel
 * @date 2012-01-31
 * @see Section 8.6 in Andrei Alexandrescu: Modern C++ Design. Addison-Wesley, 2001.
 */
template <class ObjectType, typename IdentifierType>
class ExceptionFallback {
	public:
		class Exception : public std::exception {
			private:
				IdentifierType id;
			public:
				Exception(IdentifierType  unknownId) : id(std::move(unknownId)) {
				}
				virtual ~Exception() noexcept {
				}
				const char * what() const noexcept override {
					return "Request for unknown type to factory.";
				}
				const IdentifierType & getIdentifier() const {
					return id;
				}
		};

		typedef std::function<ObjectType (const IdentifierType &)> FactoryCall;

		ObjectType onUnknownType(FactoryCall /*functor*/, const IdentifierType & id) {
			throw Exception(id);
		}
};

/**
 * @brief Possible fallback policy for a factory
 *
 * In case of an unknown type, @a nullptr is returned by the factory.
 *
 * @see Factory or WrapperFactory for description of template parameters
 * @author Benjamin Eikel
 * @date 2012-01-31
 */
template <class ObjectType, typename IdentifierType>
class NULLFallback {
	public:
		typedef std::function<ObjectType (const IdentifierType &)> FactoryCall;

		ObjectType onUnknownType(FactoryCall /*functor*/, const IdentifierType & /*id*/) {
			return nullptr;
		}
};

/**
 * @brief Possible fallback policy for a factory
 *
 * In case of an unknown type, a default object is constructed.
 * The default object is requested from the factory by using the specified identifier.
 * An error message is output to the given stream.
 *
 * @see Factory or WrapperFactory for description of template parameters
 * @author Benjamin Eikel
 * @date 2012-02-01
 */
template <class ObjectType, typename IdentifierType>
class DefaultCreatorFallback {
	private:
		IdentifierType defaultIdentifier;
		std::ostream & output;
	public:
		DefaultCreatorFallback(const IdentifierType & id, std::ostream & outputStream) :
			defaultIdentifier(id), output(outputStream) {
		}

		//! Type of a function object that encapsulates a call to the factory that uses this fallback.
		typedef std::function<ObjectType (const IdentifierType &)> FactoryCall;

		/**
		 * Use the given function object calling the create function of the factory.
		 * Give the default identifier to the create function to create a default object.
		 */
		ObjectType onUnknownType(FactoryCall functor, const IdentifierType & id) {
			output << "Warning: Using standard object factory for \"" << id << "\"." << std::endl;
			return functor(defaultIdentifier);
		}
};

}
}

#endif /* UTIL_FACTORY_FALLBACKPOLICIES_H */
