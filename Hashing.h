/*
	This file is part of the Util library.
	Copyright (C) 2007-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef UTIL_HASHING_H_
#define UTIL_HASHING_H_

#include <cstddef>
#include <cstdint>
#include <string>

namespace Util {

//! @addtogroup util_helper
//! @{

//! @name Hashing
//! @{

uint32_t calcHash(const uint8_t * ptr,size_t size);

std::string md5(const std::string& str);

template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

inline void hash_param(size_t &seed) {}

template <typename T>
inline void hash_param(size_t &seed, const T &value) {
	hash_combine(seed, value);
}

template <typename T, typename... Args>
inline void hash_param(size_t& seed, const T& first_arg, const Args& ...args) {
	hash_combine(seed, first_arg);
	hash_param(seed, args...);
}

template <typename T>
inline size_t hash(const T& arg) {
	std::hash<T> hasher;
	return hasher(arg);
}

constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

/** FNV1a c++11 constexpr compile time hash functions, 32 bit
 * str should be a null terminated string literal, value should be left out 
 * e.g hash32("example")
 * code license: public domain or equivalent
 * post: https://notes.underscorediscovery.com/constexpr-fnv1a/
 */
inline constexpr uint32_t hash32(const char* const str, const uint32_t value = val_32_const) noexcept {
	return (str[0] == '\0') ? value : hash32(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
}

//! FNV1a c++11 constexpr compile time hash functions, 64 bit @see{hash32()}
inline constexpr uint64_t hash64(const char* const str, const uint64_t value = val_64_const) noexcept {
	return (str[0] == '\0') ? value : hash64(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
}

//! @}
//! @}
} /* Util */

#endif /* end of include guard: UTIL_HASHING_H_ */
