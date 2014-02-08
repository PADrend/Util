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

namespace Util {
namespace Network {

bool init();

// --------------------------------------

struct InternalIPv4AddressData_t;

class IPv4Address {
	public:
		IPv4Address();
		IPv4Address(const InternalIPv4AddressData_t & internalData);
		IPv4Address(uint32_t _host, uint16_t _port);
		IPv4Address(const IPv4Address & source);
		~IPv4Address();

		IPv4Address & operator=(const IPv4Address & source);
		void setHost(uint32_t host);
		void setPort(uint16_t port);
		bool operator<(const IPv4Address & p) const;
		bool isValid() const;
		std::string toString() const;
		std::string getHostAsString() const;
		uint32_t getHost() const;
		uint16_t getPort() const;
		const InternalIPv4AddressData_t * getData() const;

		static IPv4Address resolveHost(const std::string & host, uint16_t port);
	private:
		std::unique_ptr<InternalIPv4AddressData_t> data;
};

}
}

#endif // UTIL_NETWORK_H
