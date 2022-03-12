/*
	This file is part of the Platform for Algorithm Development and Rendering (PADrend).
	Web page: http://www.padrend.de/
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2014-2022 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_HANDLELUT_H_
#define UTIL_HANDLELUT_H_

#include "Handle.h"

#include <unordered_map>
#include <optional>
#include <functional>
#include <cstdint>

namespace Util {

/**
 * @brief Compact lookup table for opaque handles to generic data
 * This lookup table can be used to compactly store & efficiently access objects by provide an opaque handle to the object.
 * An opaque handle is a type-safe unique identifier for an arbitrary object.
 * Internally the handle is implemented as a 64-bit integer which encodes the type, index into the lookup table, owner-id, optional user data
 * and a checksum to distinguish between removed objects with the same index.
 * The handle also encodes an owner-id to distinguish between handles of the same type created by different LUTs.
 * The owner-id is per-instance and has to be defined on construction.
 * 
 * @note by default, the LUT can store ~1Mil. objects (20 bits) with 24 bits of user-data.
 * 
 * @tparam HandleType_t opaque type of the handle 
 * @tparam ObjectType_t type of the mapped object
 * @tparam CHECKSUM_BITS number of checksum bits (default: 12; at most 16)
 * @tparam INDEX_BITS number of index bits (default: 20)
 * @tparam OWNER_BITS number of owner bits (default: 4)
 * @tparam TYPE_BITS number of type bits (default: 4)
 */
template<typename HandleType_t, typename ObjectType_t, uint32_t CHECKSUM_BITS=12, uint32_t INDEX_BITS=20, uint32_t OWNER_BITS=4, uint32_t TYPE_BITS=4>
class HandleLookupTable {
public:
	static_assert(CHECKSUM_BITS <= 16, "Checksum bits must be less than or equal to 16");
	static_assert(INDEX_BITS <= 32, "Index bits must be less than or equal to 32");
	static_assert(OWNER_BITS <= 32, "Owner bits must be less than or equal to 32");
	static_assert(CHECKSUM_BITS+INDEX_BITS+TYPE_BITS <= 64, "Checksum bits + index bits + type bits must be less than or equal to 64");
	using HandleType = HandleType_t;
	using ObjectType = ObjectType_t;
	using ObjectList = FreeList<ObjectType>;

	HandleLookupTable(uint32_t id=0) : ownerId(id) {}

	const uint32_t ownerId;

	static const uint32_t ChecksumBits = CHECKSUM_BITS;
	static const uint32_t IndexBits = INDEX_BITS;
	static const uint32_t OwnerBits = OWNER_BITS;
	static const uint32_t TypeBits = TYPE_BITS;
	static const uint32_t UserDataBits = 64 - ChecksumBits - IndexBits - OwnerBits - TypeBits;

	static const uint32_t IndexOffset = 0;
	static const uint32_t ChecksumOffset = IndexBits;
	static const uint32_t OwnerOffset = 64 - TypeBits - OwnerBits;
	static const uint32_t TypeOffset = 64 - TypeBits;
	static const uint32_t UserDataOffset = IndexBits + ChecksumBits;

	static const uint64_t INDEX_MASK = bitmask<uint64_t>(IndexBits, IndexOffset);
	static const uint64_t CHECKSUM_MASK = bitmask<uint64_t>(ChecksumBits, ChecksumOffset);
	static const uint64_t TYPE_MASK = bitmask<uint64_t>(TypeBits, TypeOffset);
	static const uint64_t OWNER_MASK = bitmask<uint64_t>(OwnerBits, OwnerOffset);
	static const uint64_t USERDATA_MASK = ~(INDEX_MASK | CHECKSUM_MASK | OWNER_MASK | TYPE_MASK);

	//! Returns maximum number of objects that can be stored in the lookup table
	static uint32_t maxCapacity() { return 1 << IndexBits; }
	//! decodes the index into the lookup table of the handle
	static int32_t decodeIndex(HandleType handle) { return static_cast<int32_t>(static_cast<uint64_t>(handle) & INDEX_MASK); }
	//! decodes the checksum of the handle
	static uint16_t decodeChecksum(HandleType handle) { return static_cast<uint16_t>((static_cast<uint64_t>(handle) & CHECKSUM_MASK) >> ChecksumOffset); }
	//! decodes the owner-id of the handle
	static uint32_t decodeOwnerId(HandleType handle) { return static_cast<uint32_t>((static_cast<uint64_t>(handle) & OWNER_MASK) >> OwnerOffset); }
	//! decodes the user-data of the handle
	static uint64_t decodeUserData(HandleType handle) { return (static_cast<uint64_t>(handle) & USERDATA_MASK) >> UserDataOffset; }
	//! checks if the given 64-bit integer is a valid handle
	static bool checkType(uint64_t handle) { return ((handle & TYPE_MASK) >> TypeOffset) == static_cast<uint64_t>(HandleType::Type); }
	//! encodes the index into the lookup table into a 64-bit integer
	static uint64_t encodeIndex(int32_t index) { return static_cast<uint64_t>(index) & INDEX_MASK; }
	//! encodes a checksum into a 64-bit integer
	static uint64_t encodeChecksum(uint16_t checksum) { return (static_cast<uint64_t>(checksum) << ChecksumOffset) & CHECKSUM_MASK; }
	//! encodes user-data into a 64-bit integer
	static uint64_t encodeUserData(uint64_t userData) { return (userData << UserDataOffset) & USERDATA_MASK; }
	//! encodes owner-id into a 64-bit integer
	static uint64_t encodeOwnerId(uint32_t ownerId) { return (static_cast<uint64_t>(ownerId) << OwnerOffset) & USERDATA_MASK; }
	//! encodes the type of the handle into the MSBs of a 64-bit integer
	static uint64_t encodeType() { return (static_cast<uint64_t>(HandleType::Type) << TypeOffset) & TYPE_MASK; }

	//! checks if the given handle is owned by this LUT
	bool checkOwner(HandleType handle) { return static_cast<uint32_t>((handle & OWNER_MASK) >> OwnerOffset) == ownerId; }

	//! Stores an object into the lookup table and returns a handle.
	HandleType createHandle(const ObjectType& object, uint64_t userData=0) {
		if(static_cast<uint32_t>(lut.range()) >= maxCapacity())
			return HandleType::Invalid;
		int32_t index = lut.insert(object);
		while(index >= checksums.size()) checksums.push_back(0);
		int32_t checksum = checksums[index];
		return static_cast<HandleType>(encodeType() | encodeOwnerId(ownerId) | encodeUserData(userData) | encodeChecksum(checksum) | encodeIndex(index));
	}
	
	//! Stores an object into the lookup-table and returns a handle.
	HandleType createHandle(ObjectType&& object, uint64_t userData=0) {
		if(static_cast<uint32_t>(lut.range()) >= maxCapacity())
			return HandleType::Invalid;
		int32_t index = lut.insert(std::move(object));
		while(index >= checksums.size()) checksums.push_back(0);
		int32_t checksum = checksums[index];
		return static_cast<HandleType>(encodeType() | encodeOwnerId(ownerId) | encodeUserData(userData) | encodeChecksum(checksum) | encodeIndex(index));
	}

	//! Resolves the stored object of a handle and returns a copy of it.
	std::optional<ObjectType> resolveHandle(HandleType handle) const {
		const int32_t index = decodeIndex(handle);
		const uint16_t checksum = decodeChecksum(handle);
		return (handle != HandleType::Invalid && checkOwner(handle) && lut.valid(index) && checksum == checksums[index]) ? std::optional<ObjectType>(lut[index]) : std::nullopt;
	}

	//! Checks if the given handle has an associated object in the lookup-table.
	bool isValid(HandleType handle) const {
		const int32_t index = decodeIndex(handle);
		const uint16_t checksum = decodeChecksum(handle);
		return handle != HandleType::Invalid && checkOwner(handle) && lut.valid(index) && checksum == checksums[index];
	}

	//! Directly accesses the stored object without checking the validity of the handle (call isValid first).
	ObjectType& getObject(HandleType handle) { return lut[decodeIndex(handle)]; }

	//! Releases the stored object and returns a copy of it (if it exists).
	std::optional<ObjectType> releaseHandle(HandleType handle) {
		const int32_t index = decodeIndex(handle);
		const uint16_t checksum = decodeChecksum(handle);
		if(handle != HandleType::Invalid && checkOwner(handle) && lut.valid(index) && checksum == checksums[index]) {
			std::optional<ObjectType> value = std::move(lut[index]);
			lut.erase(index);
			++checksums[index];
			checksums[index] &= (CHECKSUM_MASK >> IndexBits); // overflow resets checksum
			return value;
		}
		return std::nullopt;
	}

	//! clears the lookup-table
	void clear() { lut.clear(); checksums.clear(); }

	typename ObjectList::const_iterator begin() const { return lut.begin(); }
	typename ObjectList::const_iterator end() const { return lut.end(); }
private:
	ObjectList lut;
	SmallList<uint16_t> checksums;
};

//---------------------------------------------------------

/**
 * @brief Handle LUT that allows storing not-owned data
 * 
 * 
 * @tparam HandleType_t 
 * @tparam ObjectType_t 
 * @tparam CHECKSUM_BITS 
 * @tparam INDEX_BITS 
 * @tparam OWNER_BITS 
 * @tparam TYPE_BITS 
 */
template<typename HandleType_t, typename ObjectType_t, uint32_t CHECKSUM_BITS=12, uint32_t INDEX_BITS=20, uint32_t OWNER_BITS=4, uint32_t TYPE_BITS=4>
class MixedHandleLookupTable {
public:
	using HandleType = HandleType_t;
	using ObjectType = ObjectType_t;

	//! checks if the given handle is owned by this LUT
	bool checkOwner(HandleType handle) { return lut.checkOwner(handle); }

	//! Stores an object into the lookup table and returns a handle.
	HandleType createHandle(const ObjectType& object, uint64_t userData=0) {
		return lut.createHandle(object, userData);
	}
	
	//! Stores an object into the lookup-table and returns a handle.
	HandleType createHandle(ObjectType&& object, uint64_t userData=0) {
		return lut.createHandle(object, userData);
	}

	//! Resolves the stored object of a handle and returns a copy of it.
	std::optional<ObjectType> resolveHandle(HandleType handle) const {
		if(lut.checkOwner(handle)) {
			return lut.resolveHandle(handle);
		} else if(auto it = external.find(handle); it != external.end()) {
			return std::optional<ObjectType>(*it);
		}
		return std::nullopt;
	}

	//! Checks if the given handle has an associated object in the lookup-table.
	bool isValid(HandleType handle) const {
		const int32_t index = decodeIndex(handle);
		const uint16_t checksum = decodeChecksum(handle);
		return handle != HandleType::Invalid && checkOwner(handle) && lut.valid(index) && checksum == checksums[index];
	}

	//! Directly accesses the stored object without checking the validity of the handle (call isValid first).
	ObjectType& getObject(HandleType handle) { return lut[decodeIndex(handle)]; }

	//! Releases the stored object and returns a copy of it (if it exists).
	std::optional<ObjectType> releaseHandle(HandleType handle) {
		const int32_t index = decodeIndex(handle);
		const uint16_t checksum = decodeChecksum(handle);
		if(handle != HandleType::Invalid && checkOwner(handle) && lut.valid(index) && checksum == checksums[index]) {
			std::optional<ObjectType> value = std::move(lut[index]);
			lut.erase(index);
			++checksums[index];
			checksums[index] &= (CHECKSUM_MASK >> IndexBits); // overflow resets checksum
			return value;
		}
		return std::nullopt;
	}

	//! clears the lookup-table
	void clear() { lut.clear(); checksums.clear(); }
private:
	HandleLookupTable<HandleType_t,ObjectType_t,CHECKSUM_BITS,INDEX_BITS,OWNER_BITS,TYPE_BITS> lut;
	std::unordered_map<HandleType_t, ObjectType_t> external;
};

} // Util

#endif // UTIL_HANDLELUT_H_