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

struct InternalIPAddressData_t;

class IPAddress {
	public:
		IPAddress();
		IPAddress(const InternalIPAddressData_t & internalData);
		IPAddress(uint32_t _host, uint16_t _port);
		IPAddress(const IPAddress & source);
		~IPAddress();

		IPAddress & operator=(const IPAddress & source);
		void setHost(uint32_t host);
		void setPort(uint16_t port);
		bool operator<(const IPAddress & p) const;
		bool isValid() const;
		std::string toString() const;
		std::string getHostAsString() const;
		uint32_t getHost() const;
		uint16_t getPort() const;
		const InternalIPAddressData_t * getData() const;

		static IPAddress resolveHost(const std::string & host, uint16_t port);
	private:
		std::unique_ptr<InternalIPAddressData_t> data;
};

}
}

#endif // UTIL_NETWORK_H
