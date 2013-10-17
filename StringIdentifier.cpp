/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "StringIdentifier.h"
#include "Concurrency/Concurrency.h"
#include "Concurrency/Lock.h"
#include "Concurrency/Mutex.h"
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <unordered_map>

namespace Util {

typedef std::unordered_map<uint32_t, std::string> stringIdTable_t;

static stringIdTable_t & getStringIdTable() {
	static std::unique_ptr<stringIdTable_t> stringIdTable(new stringIdTable_t);
	return *stringIdTable.get();
}

static Concurrency::Mutex & getLookupTableMutex() {
	static std::unique_ptr<Concurrency::Mutex> mutex(Concurrency::createMutex());
	return *mutex;
}

uint32_t StringIdentifier::calcId(const std::string & s) {
	uint32_t id = calcHash(s);
	auto lock = Concurrency::createLock(getLookupTableMutex());
	stringIdTable_t & stringIdTable = getStringIdTable();

	while (true) {
		auto entry = stringIdTable.find(id);
		if (entry == stringIdTable.cend()) {
			// id not found -> insert it
			stringIdTable.emplace(id, s);
			break;
		} else if (s == entry->second) {
			// same string already inserted
			break;
		} else {
			// collision
			++id;
		}
	}
	return id;
}

std::string StringIdentifier::toString() const {
	auto lock = Concurrency::createLock(getLookupTableMutex());
	stringIdTable_t & stringIdTable = getStringIdTable();
	auto entry = stringIdTable.find(value);
	if (entry == stringIdTable.cend()) {
		std::stringstream s;
		s << "_strId_" << value;
		stringIdTable[value] = s.str();
		return s.str();
	}
	return entry->second;
}

uint32_t StringIdentifier::calcHash(const std::string & s) {
	uint32_t h = 0;
	for(const auto & c : s) {
		h ^= (((static_cast<uint32_t>(c) + h) * 1234393) % 0xffffff);
	}
	return h;
}

}
