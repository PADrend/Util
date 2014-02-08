/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_SYNCHRONIZER_H
#define UTIL_SYNCHRONIZER_H

#include "Network.h"
#include "../Concurrency/UserThread.h"

namespace Util{
namespace Network{

class UDPNetworkSocket;

//! ClockSynchronizer ---|> UserThread
class ClockSynchronizer : private Concurrency::UserThread {
	public:
		enum mode_t{
			SERVER,CLIENT
		};

		static ClockSynchronizer * createServer(uint16_t port);
		static ClockSynchronizer * createClient(const IPv4Address & remoteIP);

		/** Default destructor */
		virtual ~ClockSynchronizer();

		float getClockSec()const;

	/*! @name State */
	// @{
	private:
		enum state_t {
			RUNNING,
			CLOSING,
			CLOSED
		}state;

	public:
		state_t getState()const      {   return state;   }
		bool isRunning()const        {   return state==RUNNING; }
		void close();

	// @}

	/*! @name ---|> UserThread */
	// @{
	private:
		void run() override;
	// @}

	private:
		ClockSynchronizer(mode_t mode, UDPNetworkSocket *socket );
		void runServer();
		void runClient();

		mode_t mode;
		std::unique_ptr<UDPNetworkSocket> mySocket;
		float diff;
};

}
}

#endif // UTIL_SYNCHRONIZER_H
