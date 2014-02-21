/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_NETWORK_TCP_H
#define UTIL_NETWORK_TCP_H

#include "Network.h"
#include "../ReferenceCounter.h"
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace Util {
namespace Network {

class TCPServer;

/**
 * TCP Connection between two endpoints.
 */
class TCPConnection : public ReferenceCounter<TCPConnection> {
	public:
		friend class TCPServer;

		static const size_t BUFFER_SIZE = 4096;

		/*! Tries to open a connection to a TCP-Server at given address.
		 \return TCPConnection if successful, nullptr on failure */
		static Reference<TCPConnection> connect(const IPv4Address & remote);

	private:
		mutable std::mutex connectionDataMutex;
		struct InternalData;
		std::unique_ptr<InternalData> connectionData;
		TCPConnection(InternalData && internalData);

	public:
		virtual ~TCPConnection();

		float getLastActiveTime() const;
		IPv4Address getRemoteIp() const;

		/*! @name State */
		// @{
	private:
		enum state_t {
			OPEN, CLOSING, CLOSED
		} volatile state;
		mutable std::mutex stateMutex;

		void setState(state_t newState) {
			std::lock_guard<std::mutex> lock(stateMutex);
			state = newState;
		}
	public:
		state_t getState() const {
			std::lock_guard<std::mutex> lock(stateMutex);
			return state;
		}
		bool isOpen() const{
			return getState() == OPEN;
		}
		bool mayBeOpen()const volatile{
			return state == OPEN;
		}
		void close();
		// @}

		/*! @name Data handling */
		// @{
	private:
		std::deque<std::vector<uint8_t>> inQueue;
		std::deque<std::vector<uint8_t>> outQueue;
		volatile size_t inQueueDataSize;
		mutable std::mutex inQueueMutex;
		mutable std::mutex outQueueMutex;
		std::thread thread;

		void run();
		std::vector<uint8_t> extractDataFromInQueue(size_t numBytes);

	public:
		bool sendData(const std::vector<uint8_t> & data);

		/*! \note The string should end with a termination symbol, e.g. 0
		 Otherwise it can't be reextracted properly.   */
		bool sendString(const std::string & s);

		/*! Returns all received data as a std::vector<uint8_t> Object
		 or nullptr if nothing was received. */
		std::vector<uint8_t> receiveData();

		/*! Returns numBytes many bytes of received data as std::vector<uint8_t> Object
		 or nullptr if less data was received. */
		std::vector<uint8_t> receiveData(size_t numBytes);

		/*! Returns a string ending with @p delimiter or "" if no delimiter is
		 found. */
		std::string receiveString(char delimiter = '\0');

		/*! Returns the number of bytes in the input buffer */
		size_t getAvailableDataSize() const {
			std::lock_guard<std::mutex> lock(inQueueMutex);
			return inQueueDataSize;
		}
		// @}
};

/**
 * TCP Server which creates TCPConnections
 */
class TCPServer {
	public:
		/*! (Factory) Try to create a TCPServer listening on the given port.
		 Returns nullptr, if the port can not be opened. */
		static TCPServer * create(uint16_t port);

	private:
		std::mutex serverDataMutex;
		struct InternalData;
		std::unique_ptr<InternalData> serverData;
		TCPServer(InternalData && internalData);

	public:
		virtual ~TCPServer();

		/*! @name State */
		// @{
	private:
		enum state_t {
			OPEN, CLOSING, CLOSED
		} state;
		mutable std::mutex stateMutex;

		void setState(state_t newState) {
			std::lock_guard<std::mutex> lock(stateMutex);
			state = newState;
		}
	public:
		state_t getState() const {
			std::lock_guard<std::mutex> lock(stateMutex);
			return state;
		}
		bool isOpen() const {
			return getState() == OPEN;
		}


		/*! Stops the server. New incoming connections are closed, old
		 connections persist. */
		void close();
		// @}

		/*! @name Connection handling */
		// @{
	private:
		std::deque<Reference<TCPConnection>> newConnectionsQueue;
		std::mutex queueMutex;
		
		std::thread connectionHandlingThread;

		void run();

	public:
		/*! Returns a new Connection or nullptr if there is none.  Each incoming
		 connection is only reported once.   */
		Reference<TCPConnection> getIncomingConnection();
		// @}
};

}
}

#endif // UTIL_NETWORK_TCP_H
