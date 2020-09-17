/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_SYNCHRONIZER_H
#define UTIL_SYNCHRONIZER_H

#include "Network.h"
#include <mutex>
#include <thread>

namespace Util{
namespace Network{

class UDPNetworkSocket;

class ClockSynchronizer {
	public:
		UTILAPI static ClockSynchronizer * createServer(uint16_t port);
		UTILAPI static ClockSynchronizer * createClient(const IPv4Address & remoteIP);

		/** Default destructor */
		UTILAPI ~ClockSynchronizer();

		UTILAPI float getClockSec()const;

	/*! @name State */
	// @{
	private:
		bool running;
		mutable std::mutex runningMutex;

	public:
		UTILAPI bool isRunning() const;
		UTILAPI void close();

	// @}

	private:
		UTILAPI ClockSynchronizer(bool startServer, UDPNetworkSocket * socket);
		UTILAPI void runServer();
		UTILAPI void runClient();

		std::unique_ptr<UDPNetworkSocket> mySocket;
		float diff;
		std::thread thread;
};

}
}

#endif // UTIL_SYNCHRONIZER_H
