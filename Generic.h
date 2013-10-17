/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_GENERIC_H
#define UTIL_GENERIC_H

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace Util {

/**
 * @brief Container for a value with generic type
 * 
 * A Generic object is able to store a single object of different types. This
 * allows for the usage of Generic as variable with a type that does not have
 * to be known at compile-time.
 * @code
 * Util::Generic value;
 * value = 42.0;
 * std::cout << value.contains<std::string>() << std::endl;
 * value = std::string("fourty-two");
 * std::cout << value.contains<std::string>() << std::endl;
 * std::cout << value.ref<std::string>() << std::endl;
 * value = false;
 * @endcode
 * 
 * @author Benjamin Eikel
 * @date 2013-05-28
 */
class Generic {
	private:
		//! Abstract base class for all template instantiations in subclasses
		struct AbstractStorage {
			virtual ~AbstractStorage() {
			}
			virtual AbstractStorage * clone() const = 0;
		};
		template<typename type_t>
		struct Storage : public AbstractStorage {
			//! Data that is stored here
			type_t data;

			//! Construct the storage taking a convertible object as parameter.
			template<typename other_type_t, 
					 typename = typename std::enable_if<std::is_convertible<
															other_type_t,
															type_t
													   >::value
										 >::type>
			explicit Storage(other_type_t && object) : 
				AbstractStorage(), data(std::forward<other_type_t>(object)) {
			}
			virtual ~Storage() {
			}
			Storage * clone() const override {
				return new Storage(*this);
			}
		};

		std::unique_ptr<AbstractStorage> content;

	public:
		//! Construct with invalid value
		Generic() : content() {
		}

		//! Construct with a copy of the given object
		template<typename type_t>
		explicit Generic(type_t && object) : 
			content(new Storage<typename std::decay<type_t>::type>(
												std::forward<type_t>(object))) {
		}

		//! Copy construct from another generic object
		Generic(const Generic & other) : 
			content(other.content ? other.content->clone() : nullptr) {
		}
		//! Move construct from another generic object
		Generic(Generic &&) = default;

		//! Destroy the generic object
		~Generic() = default;

		//! Copy from another object
		template<typename type_t>
		Generic & operator=(type_t && object) {
			*this = Generic(object);
			return *this;
		}
		//! Move construct from another generic object
		Generic & operator=(Generic &&) = default;

		//! Check if the generic object contains any data
		bool valid() const {
			return content.operator bool();
		}

		//! Check if the stored data is of the given type
		template<typename type_t>
		bool contains() const {
			return get<type_t>() != nullptr;
		}

		/**
		 * Access the data of the generic object by pointer.
		 * 
		 * @return Pointer to the stored data, or @c nullptr if no data is
		 * stored or the data is not of the requested type.
		 */
		template<typename type_t>
		type_t * get() {
			if(!content) {
				return nullptr;
			}
			auto ptr = dynamic_cast<Storage<typename std::decay<type_t>::type> *>(content.get());
			if(ptr == nullptr) {
				return nullptr;
			}
			return &ptr->data;
		}

		/**
		 * Access the data of the generic object for reading by pointer.
		 * 
		 * @return Pointer to the stored data, or @c nullptr if no data is
		 * stored or the data is not of the requested type.
		 */
		template<typename type_t>
		const type_t * get() const {
			// Save a second implementation by using const_cast here
			return const_cast<Generic *>(this)->get<type_t>();
		}

		/**
		 * Access the data of the generic object by reference.
		 * 
		 * @throw std::bad_cast if no data is stored or the data is not of the
		 * requested type
		 * @return Reference to the stored data
		 */
		template<typename type_t>
		type_t & ref() {
			auto ptr = get<type_t>();
			if(ptr == nullptr) {
				throw std::bad_cast();
			}
			return *ptr;
		}

		/**
		 * Access the data of the generic object for reading by reference.
		 * 
		 * @throw std::bad_cast if no data is stored or the data is not of the
		 * requested type
		 * @return Reference to the stored data
		 */
		template<typename type_t>
		const type_t & ref() const {
			// Save a second implementation by using const_cast here
			return const_cast<Generic *>(this)->ref<type_t>();
		}
};

}

#endif /* UTIL_GENERIC_H */
