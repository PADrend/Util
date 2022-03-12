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
#ifndef UTIL_HANDLES_H_
#define UTIL_HANDLES_H_

#include "Utils.h"

namespace Util {

//! Utility macro to declare opaque handles
#define DECLARE_HANDLE(HandleName, TypeId, TypeBits) enum class HandleName##Handle : uint64_t { Invalid = 0, Type = TypeId }; \
inline bool isValid(HandleName##Handle h) { return h != HandleName##Handle::Invalid && (static_cast<uint64_t>(h) >> (64-TypeBits)) == static_cast<uint64_t>(HandleName##Handle::Type); } \
inline bool checkType(uint64_t handle, HandleName##Handle type) { static mask = bitmask<uint64_t>(TypeBits, 64-TypeBits); return ((handle & mask) >> (64-TypeBits)) == static_cast<uint64_t>(HandleName##Handle::Type); } \
inline bool operator!(HandleName##Handle h) { return !isValid(h); }

//------------------

//! Generic opaque handle which can be cast to/from other opaque handles with type checking.
enum class GenericHandle : uint64_t { Invalid = 0 };
//! Checks if the handle is valid.
inline bool isValid(GenericHandle h) { return h != GenericHandle::Invalid; }
//! Checks if the handle is not valid.
inline bool operator!(GenericHandle h) { return !isValid(h); }
//! Cast generic handle to specific handle if possible.
template<typename Handle> inline Handle toHandle(GenericHandle handle) { return checkType(static_cast<uint64_t>(handle), Handle::Type) ? static_cast<Handle>(handle) : Handle::Invalid; }
//! Cast specific handle to generic handle.
template<typename Handle> inline GenericHandle toGeneric(Handle handle) { return static_cast<GenericHandle>(handle); }

//---------------------

} // Util

#endif // UTIL_HANDLES_H_