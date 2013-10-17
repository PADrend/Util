/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_DATA_BROADCASTER_H
#define UTIL_DATA_BROADCASTER_H

#include "DataConnection.h"
#include "../ReferenceCounter.h"
#include <vector>

namespace Util {
namespace Network {

/**
 * Broadcasts data to a set of DataConnections.
 * \note The DataConnections can at the same time used for other communications.
 * \note A closed DataConnection is automatically removed if it is closed.
 * \note Deleting a DataBroadcaster does not automatically close the assciated connections.
 * [DataBroadcaster]
 */
class DataBroadcaster : public ReferenceCounter<DataBroadcaster> {
	public:
		typedef DataConnection::dataPacket_t dataPacket_t;
		typedef DataConnection::channelId_t channelId_t;

		DataBroadcaster() : ReferenceCounter_t() {
		}

		void addDataConnection(DataConnection * d) {
			dataConnections.push_back(d);
		}

		//! \see DataConnection::sendKeyValue
		void sendKeyValue(channelId_t channel, const StringIdentifier & key, const dataPacket_t & data) {
			bool needCleanUp = false;
			for(auto & dataConnection : dataConnections) {
				if(dataConnection->isOpen()) {
					dataConnection->sendKeyValue(channel, key, data);
				} else {
					needCleanUp = true;
				}
			}
			if(needCleanUp) {
				removeDeadConnections();
			}
		}

		//! \see DataConnection::sendValue
		void sendValue(channelId_t channel, const dataPacket_t & data) {
			bool needCleanUp = false;
			for(auto & dataConnection : dataConnections) {
				if(dataConnection->isOpen()) {
					dataConnection->sendValue(channel, data);
				} else {
					needCleanUp = true;
				}
			}
			if(needCleanUp) {
				removeDeadConnections();
			}
		}

	private:
		std::vector<Reference<DataConnection>> dataConnections;

		void removeDeadConnections() {
			std::vector<Reference<DataConnection>> temp;
			for(auto & dataConnection : dataConnections) {
				if(dataConnection->isOpen()) {
					temp.emplace_back(dataConnection);
				}
			}
			using std::swap;
			swap(temp, dataConnections);
		}
};

}
}

#endif // UTIL_DATA_BROADCASTER_H
