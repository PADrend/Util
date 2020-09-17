/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_NETWORK_H
#define UTIL_NETWORK_H

#include <cstdint>
#include <memory>
#include <string>
#include <functional>

namespace Util {
namespace Network {

bool init();

// --------------------------------------

struct InternalIPv4AddressData_t;

class IPv4Address {
	public:
		UTILAPI IPv4Address();
		UTILAPI IPv4Address(const InternalIPv4AddressData_t & internalData);
		UTILAPI IPv4Address(uint32_t _host, uint16_t _port);
		UTILAPI IPv4Address(const IPv4Address & source);
		UTILAPI ~IPv4Address();

		UTILAPI IPv4Address & operator=(const IPv4Address & source);
		UTILAPI void setHost(uint32_t host);
		UTILAPI void setPort(uint16_t port);
		UTILAPI bool operator<(const IPv4Address & p) const;
		UTILAPI bool isValid() const;
		UTILAPI std::string toString() const;
		UTILAPI std::string getHostAsString() const;
		UTILAPI uint32_t getHost() const;
		UTILAPI uint16_t getPort() const;
		UTILAPI const InternalIPv4AddressData_t * getData() const;

		UTILAPI static IPv4Address resolveHost(const std::string & host, uint16_t port);
	private:
		std::unique_ptr<InternalIPv4AddressData_t> data;
};

}
}

#endif // UTIL_NETWORK_H
