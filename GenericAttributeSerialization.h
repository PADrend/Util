/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_GENERICATTRIBUTESERIALIZATION_H
#define UTIL_GENERICATTRIBUTESERIALIZATION_H

#include "Factory/WrapperFactory.h"
#include <functional>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <utility>
namespace Util {
class GenericAttribute;
class GenericAttributeMap;

/**
 * @brief Serialization for GenericAttribute
 *
 * Class for serialization and unserialization of GenericAttribute objects.
 * @author Benjamin Eikel
 * @date 2012-03-12
 * @ingroup generic_attr
 */
class GenericAttributeSerialization {
	public:
		typedef std::pair<std::string, std::string> serializer_type_t;
		typedef std::pair<const GenericAttribute *,
						  const GenericAttributeMap *> serializer_parameter_t;

		typedef GenericAttribute * unserializer_type_t;
		typedef std::pair<std::string,
						  const GenericAttributeMap *> unserializer_parameter_t;

	private:
		typedef WrapperFactory<serializer_parameter_t,
							   serializer_type_t,
							   std::type_index> serializer_factory_t;
		UTILAPI static serializer_factory_t & getSerializerFactory();

		typedef WrapperFactory<unserializer_parameter_t,
							   unserializer_type_t,
							   std::string> unserializer_factory_t;
		UTILAPI static unserializer_factory_t & getUnserializerFactory();

	public:
		/**
		 * Initialize the serializers for the standard GenericAttributes.
		 * This function has to be called at least once before using the class.
		 * It is called from init.
		 *
		 * @retval true if the initialization was successful
		 * @retval false if an error occurred
		 */
		UTILAPI static bool init();

		/**
		 * Convert the given generic attribute to a string.
		 * The string is a JSON representation of the generic attribute.
		 * It is constructed as follows:
		 * @code
		 * {
		 *     "type" : "[string identifying the type]",
		 *     "value" : "[string representation of the content]"
		 * }
		 * @endcode
		 *
		 * @param attribute Generic attribute that is to be converted
		 * @return String representation
		 */
		UTILAPI static std::string serialize(const GenericAttribute * attribute);

		/**
		 * Convert the given generic attribute to a string.
		 * Use the given context to look up further information that is needed.
		 *
		 * @param attribute Generic attribute that is to be converted
		 * @param context A map containing specific information that is needed
		 * during unserialization
		 * @return String representation
		 */
		UTILAPI static std::string serialize(const GenericAttribute * attribute, 
									 const GenericAttributeMap * context);

		/**
		 * Convert the given string representation to a generic attribute.
		 *
		 * @param representation String representation that was created by
		 * serialize()
		 * @return Newly constructed, generic attribute
		 * @note The caller has to free the memory for the generic attribute.
		 */
		UTILAPI static GenericAttribute * unserialize(const std::string & representation);

		/**
		 * Convert the given string representation to a generic attribute.
		 * Use the given context to look up further information that is needed.
		 *
		 * @param representation String representation that was created by
		 * serialize()
		 * @param context A map containing specific information that is needed
		 * during unserialization
		 * @return Newly constructed, generic attribute
		 * @note The caller has to free the memory for the generic attribute.
		 */
		UTILAPI static GenericAttribute * unserialize(const std::string & representation, 
											  const GenericAttributeMap * context);

		/**
		 * Register a serializer and unserializer for a generic attribute type.
		 *
		 * @tparam AttributeType Type of the generic attribute to be registered
		 * @param typeName String representation of the generic attribute type that has to be unique for generic attribute serialization
		 * @param serializer Serializer function converting a generic attribute into a @p typeName and a string representation of the content
		 * @param unserialize Unserializer function reconstructing a generic attribute from a string representation
		 * @return @c true if both functions have been registered successfully, @c false otherwise
		 * @note Any previous functions for the given type are unregistered
		 */
		template<typename AttributeType>
		static bool registerSerializer(const std::string & typeName,
									   const std::function<serializer_type_t (const serializer_parameter_t &)> & serializer,
									   const std::function<unserializer_type_t (const unserializer_parameter_t &)> & unserializer) {
			getSerializerFactory().unregisterType(typeid(AttributeType));
			getUnserializerFactory().unregisterType(typeName);
			return getSerializerFactory().registerType(typeid(AttributeType), serializer) &&
				   getUnserializerFactory().registerType(typeName, unserializer);
		}
};

}

#endif /* UTIL_GENERICATTRIBUTESERIALIZATION_H */
