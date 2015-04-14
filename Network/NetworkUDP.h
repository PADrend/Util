/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_NETWORKUDP_H
#define UTIL_NETWORKUDP_H

#include "Network.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace Util {
namespace Network {

struct InternalUDPSocketData_t;

class UDPNetworkSocket;

UDPNetworkSocket * createUDPNetworkSocket(uint16_t port, int maxPktSize);

/**
 * [UDPNetworkSocket]
 */
class UDPNetworkSocket {
	public:
		static const int defaultMaxPktSize = 1024;

		explicit UDPNetworkSocket(uint16_t port = 0, int maxPktSize = defaultMaxPktSize);
		virtual ~UDPNetworkSocket();

		bool open();
		bool isOpen() const;
		void close();

		struct Packet {
			IPv4Address source;
			std::vector<uint8_t> packetData;
			Packet(const std::string & s) :
				source(), packetData(s.begin(), s.end()) {
			}
			Packet(const uint8_t * _data, size_t dataSize) :
				source(), packetData(_data, _data + dataSize) {
			}
			Packet(std::vector<uint8_t> && _data) :
				source(), packetData(std::move(_data)) {
			}
			std::string getString() const {
				return std::string(packetData.begin(), packetData.end());
			}
		};

		/**
		 * \return If data is available, a Packet-object with the data
		 *         nullptr otherwise
		 * \note The caller is responsible for deleting the received packet.
		 */
		Packet * receive();
		int sendData(const uint8_t * data, size_t dataSize);

		bool sendData(const uint8_t * data, size_t dataSize, const IPv4Address & ip);

		int sendString(const std::string & data);

		void addTarget(const IPv4Address & address);
		void removeTarget(const IPv4Address & address);

		uint16_t getPort() const;
	private:
		std::unique_ptr<InternalUDPSocketData_t> data;
};

}
}

#endif // UTIL_NETWORKUDP_H
