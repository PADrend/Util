/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_DATA_CONNECTION_H
#define UTIL_DATA_CONNECTION_H

#include "../ReferenceCounter.h"
#include "../StringIdentifier.h"
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Util {
namespace Network {

class TCPConnection;

/**
 * A high level wrapper for a network connection of two endpoints (using tcp).
 * It allows synchronizing of key-value pairs and sending of small data packages
 * using multiple virtual channels. These channels allow multiple interleaved
 * communications using a single network connection.
 *
 * \note Internally creates an additional thread.
 * \note Is not designed for sending huge blocks of data at once.
 * \note The maximum length of a key is 256 characters - additional characters are truncated.
 * \note The given TCPConnection must only be accessed by the DataConnection
 *			or the data will get corrupted!
 * [DataConnection]
 */
class DataConnection : public ReferenceCounter<DataConnection>{
	public:
		typedef uint16_t channelId_t;
		typedef std::vector<uint8_t> dataPacket_t;
		typedef std::deque<std::pair<channelId_t, dataPacket_t>> incomingValueStorage_t;
		typedef std::vector<std::pair<channelId_t, dataPacket_t>> outgoingValueStorage_t;
		typedef std::map<std::pair<channelId_t, StringIdentifier>, dataPacket_t> keyValueStorage_t;
		typedef std::function<void (channelId_t,const dataPacket_t &)> valueTypeHandler_t;
		typedef std::function<void (channelId_t,const StringIdentifier &,const dataPacket_t &)> keyValueTypeHandler_t;

		static const channelId_t FALLBACK_HANDLER;

		//! (ctor)
		DataConnection(TCPConnection * connection);
		~DataConnection();

		bool isOpen() const;
		void close();

		/*!	Send the given key,data - pair to the connected endpoint.
			\note it is not guaranteed that this specific data is transferred for
					the given key. If a new value is send to the same key, the old
					one is skipped.
			\note it is not guaranteed that the key value pairs are end in the order
					of this function call.*/
		void sendKeyValue(channelId_t channel,const StringIdentifier & key,const dataPacket_t & data){
			std::lock_guard<std::mutex> lock(outgoingKeyValuePairsMutex);
			outgoingKeyValuePairs[std::make_pair(channel, key)] = data;
		}
		
		/*! Send the given data to the connected endpoint.
			If the connection is open, it will be received in the same order as
			it has been sent and no data should be lost.
			\note don't send too much data at once. */
		void sendValue(channelId_t channel,const dataPacket_t & data){
			std::lock_guard<std::mutex> lock(outgoingValuesMutex);
			outgoingValues.emplace_back(channel, data);
		}
		
		/*! \note prefer using a type-handler (and call only from a single thread!)
			\return true iff data could be extracted.	*/
		bool extractIncomingKeyValuePair(channelId_t & channel,StringIdentifier & key,dataPacket_t & data){
			std::lock_guard<std::mutex> lock(incomingKeyValuePairsMutex);
			if(incomingKeyValuePairs.empty()) {
				return false;
			}
			channel = incomingKeyValuePairs.begin()->first.first;
			key = incomingKeyValuePairs.begin()->first.second;
			data = incomingKeyValuePairs.begin()->second;
			incomingKeyValuePairs.erase(incomingKeyValuePairs.begin());
			return true;
		}

		/*! \note prefer using a type-handler (and call only from a single thread!)
			\return true iff data could be extracted.	*/
		bool extractIncomingValue(channelId_t & channel,dataPacket_t & data){
			std::lock_guard<std::mutex> lock(incomingValuesMutex);
			if(incomingValues.empty()) {
				return false;
			}
			channel = incomingValues.front().first;
			data = incomingValues.front().second;
			incomingValues.pop_front();
			return true;
		}
		
		void registerKeyValueChannelHandler(channelId_t type,const keyValueTypeHandler_t & h){
			std::lock_guard<std::mutex> lock(keyValueChannelHandlersMutex);
			keyValueChannelHandlers[type] = h;
		}
		void registerValueChannelHandler(channelId_t type,const valueTypeHandler_t & h){
			std::lock_guard<std::mutex> lock(valueChannelHandlersMutex);
			valueChannelHandlers[type] = h;
		}
		void removeKeyValueChannelHandler(channelId_t type){
			std::lock_guard<std::mutex> lock(keyValueChannelHandlersMutex);
			keyValueChannelHandlers.erase(type);
		}
		void removeValueChannelHandler(channelId_t type){
			std::lock_guard<std::mutex> lock(valueChannelHandlersMutex);
			valueChannelHandlers.erase(type);
		}

		
		/*! Use the registered handler to handle the incoming data.
			\note \todo (all data of unknown type is send to handler 0xFFFF or ignored).
			\param if a time is given,after the given amount of ms, the handling is stopped.	*/
		void handleIncomingData(float ms=-1.0f);
	private:
		Reference<TCPConnection> connection;

		std::mutex incomingKeyValuePairsMutex;
		keyValueStorage_t incomingKeyValuePairs;

		std::mutex outgoingKeyValuePairsMutex;
		keyValueStorage_t outgoingKeyValuePairs;

		std::mutex incomingValuesMutex;
		incomingValueStorage_t incomingValues;

		std::mutex outgoingValuesMutex;
		outgoingValueStorage_t outgoingValues;

		std::mutex keyValueChannelHandlersMutex;
		std::unordered_map<channelId_t, keyValueTypeHandler_t> keyValueChannelHandlers;

		std::mutex valueChannelHandlersMutex;
		std::unordered_map<channelId_t, valueTypeHandler_t> valueChannelHandlers;

		mutable std::mutex runningMutex;
		bool running;

		std::thread thread;

		void run();
		bool isRunning() const {
			std::lock_guard<std::mutex> lock(runningMutex);
			return running;
		}
		void stop() {
			std::lock_guard<std::mutex> lock(runningMutex);
			running = false;
		}

		void dataError();
};

}
}

#endif // UTIL_DATA_CONNECTION_H
