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
#ifndef UTIL_RESOURCES_BUFFER_H_
#define UTIL_RESOURCES_BUFFER_H_

#include "Resource.h"
#include "ResourceFormat.h"

namespace Util {

class Buffer;
using BufferHandle = Reference<Buffer>;

struct BufferConfig {
	uint64_t byteSize = 0;
	ResourceFormat format = ResourceFormat::BYTES;
	std::string debugName;
};

struct BufferRange {
	BufferRange(uint64_t _byteOffset=0, uint64_t _byteSize=~0ull) : byteOffset(_byteOffset), byteSize(_byteSize) {}

	uint64_t byteOffset = 0;
	uint64_t byteSize = ~0ull;

	constexpr BufferRange& set(uint64_t _byteOffset=0, uint64_t _byteSize=~0ull) { byteOffset = _byteOffset; byteSize = _byteSize; return *this; }
	constexpr BufferRange& setByteOffset(uint64_t _byteOffset=0) { byteOffset = _byteOffset; return *this; }
	constexpr BufferRange& setByteSize(uint64_t _byteSize=~0ull) { byteSize = _byteSize; return *this; }

	static BufferRange resolve(const BufferConfig& config) { return BufferRange(0, config.byteSize); }
	static BufferRange resolve(const BufferHandle& buffer);
	constexpr bool isEntireBuffer(const BufferConfig& config) const { return (byteOffset == 0) && (byteSize == ~0ull || byteSize == config.byteSize); }
	bool isEntireBuffer(const BufferHandle& buffer) const;
	constexpr bool operator==(const BufferRange& other) const { return byteOffset == other.byteOffset && byteSize == other.byteSize; }
};

/**
 * @brief Represents a contiguous allocation of memory on a device.
 * 
 */
class Buffer : public Resource {
	PROVIDES_TYPE_NAME(Buffer)
protected:
	Buffer(const BufferConfig& config) : Resource(), config(config) {}
public:
	static BufferHandle create(const BufferConfig& config) { return new Buffer(config); }

	const BufferConfig config;
};

inline BufferRange BufferRange::resolve(const BufferHandle& buffer) { return buffer ? resolve(buffer->config) : BufferRange(0,0); }
inline bool BufferRange::isEntireBuffer(const BufferHandle& buffer) const { return buffer ? isEntireBuffer(buffer->config) : false; }

} // Util

#endif // UTIL_RESOURCES_BUFFER_H_