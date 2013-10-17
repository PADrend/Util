/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "DataConnection.h"
#include "NetworkTCP.h"
#include "../Concurrency/Concurrency.h"
#include "../Concurrency/Lock.h"
#include "../Concurrency/Mutex.h"
#include "../Concurrency/UserThread.h"
#include "../Concurrency/Thread.h"
#include "../Macros.h"
#include "../Timer.h"
#include "../Utils.h"

namespace Util {
namespace Network {

const DataConnection::channelId_t DataConnection::FALLBACK_HANDLER = 0xFFFF;

struct DataConnection::WorkerThread : public Concurrency::UserThread{
	Reference<TCPConnection> connection;
	Concurrency::Mutex * incomingKeyValuePairsMutex;
	keyValueStorage_t & incomingKeyValuePairs;
	Concurrency::Mutex * outgoingKeyValuePairsMutex;
	keyValueStorage_t & outgoingKeyValuePairs;
	Concurrency::Mutex * incomingValuesMutex;
	incomingValueStorage_t & incomingValues;
	Concurrency::Mutex * outgoingValuesMutex;
	outgoingValueStorage_t & outgoingValues;

	WorkerThread(TCPConnection * _connection,
				 Concurrency::Mutex * _incomingKeyValuePairsMutex,
				 keyValueStorage_t & _incomingKeyValuePairs,
				 Concurrency::Mutex * _outgoingKeyValuePairsMutex,
				 keyValueStorage_t & _outgoingKeyValuePairs,
				 Concurrency::Mutex * _incomingValuesMutex,
				 incomingValueStorage_t & _incomingValues,
				 Concurrency::Mutex * _outgoingValuesMutex,
				 outgoingValueStorage_t & _outgoingValues) :
		connection(_connection),
		incomingKeyValuePairsMutex(_incomingKeyValuePairsMutex), incomingKeyValuePairs(_incomingKeyValuePairs), 
		outgoingKeyValuePairsMutex(_outgoingKeyValuePairsMutex), outgoingKeyValuePairs(_outgoingKeyValuePairs),
		incomingValuesMutex(_incomingValuesMutex), incomingValues(_incomingValues), 
		outgoingValuesMutex(_outgoingValuesMutex), outgoingValues(_outgoingValues) {
	}
	virtual ~WorkerThread(){}
	
	static const uint16_t MSG_TYPE_VALUE = 0;
	static const uint16_t MSG_TYPE_KEY_VALUE = 1;
	
	//! ---|> Concurrency::UserThread
	void run() override{
		static_assert(sizeof(channelId_t)==2,"channelId_t should be uint16_t");
		uint32_t incomingSize = 0;
		while(true){
			if(!connection->mayBeOpen() && !connection->isOpen())
				break;
			bool busy = false;
			if(!outgoingValues.empty()) // may be wrong!
			{ // send values
				outgoingValueStorage_t internalValues;
				{
					auto lock = Concurrency::createLock(*outgoingValuesMutex);
					using std::swap;
					swap(outgoingValues, internalValues);
				}
				for(const auto & channelData : internalValues) {
					// 4 size, 2 packet type, 2 channelNr, data
					const std::size_t msgSize = (4 + 2 + 2) + channelData.second.size();
					
					struct Header {
						uint32_t m_size;
						uint16_t m_type;
						uint16_t m_channel;
						// four byte aligned
						
						Header(uint32_t p_size, uint16_t p_type, uint16_t p_channel) :
							m_size(p_size), m_type(p_type), m_channel(p_channel) {
						}
					};
					const Header header(msgSize, MSG_TYPE_VALUE, channelData.first);
					const uint8_t * headerPtr = reinterpret_cast<const uint8_t *>(&header);
					
					std::vector<uint8_t> msg;
					msg.reserve(msgSize);
					msg.insert(msg.end(), headerPtr, headerPtr + sizeof(Header));
					msg.insert(msg.end(), channelData.second.begin(), channelData.second.end());
					connection->sendData(msg);
					busy = true;
				}
			}
			if(!outgoingKeyValuePairs.empty()) // may be wrong!
			{ // send key values
				keyValueStorage_t internalValues;
				{
					auto lock = Concurrency::createLock(*outgoingKeyValuePairsMutex);
					using std::swap;
					swap(outgoingKeyValuePairs, internalValues);
				}
				for(const auto & kValue : internalValues) {
					const dataPacket_t & data = kValue.second;
					
					const uint16_t channelId = kValue.first.first;
					const std::string key = kValue.first.second.toString();
					const uint16_t keyLen = key.length()>0xffff ? 0xffff : static_cast<uint16_t>(key.length());
					
					// 4 size, 2 packet type, 2 data type, 2 keyLength, key, data
					const std::size_t msgSize = (4 + 2 + 2 + 2) + data.size() + keyLen;
					
					struct Header {
						uint32_t m_size;
						uint16_t m_type;
						uint16_t m_channel;
						// four byte aligned
						
						Header(uint32_t p_size, uint16_t p_type, uint16_t p_channel) :
							m_size(p_size), m_type(p_type), m_channel(p_channel) {
						}
					};
					const Header header(msgSize, MSG_TYPE_KEY_VALUE, channelId);
					const uint8_t * headerPtr = reinterpret_cast<const uint8_t *>(&header);
					const uint8_t * keyLenPtr = reinterpret_cast<const uint8_t *>(&keyLen);
					
					std::vector<uint8_t> msg;
					msg.reserve(msgSize);
					msg.insert(msg.end(), headerPtr, headerPtr + sizeof(Header));
					msg.insert(msg.end(), keyLenPtr, keyLenPtr + sizeof(uint16_t));
					msg.insert(msg.end(), key.begin(), std::next(key.begin(), keyLen));
					msg.insert(msg.end(), data.begin(), data.end());
					connection->sendData(msg);
					busy = true;
				}
			}
			// no pending data? -> try to receive msg header (=the size as uint32_t)
			if(incomingSize==0){
				const std::vector<uint8_t> sizeData = connection->receiveData(4);
				if( sizeData.size()==4 ){
					const uint32_t completeDataSize = *reinterpret_cast<const uint32_t*>(sizeData.data());
					if(completeDataSize<6){
						dataError();
						return;
					}
					incomingSize = completeDataSize-4;
					busy = true;
//					std::cout << "DataSize: "<<completeDataSize;
				}
			}
			// receive messages
			if(incomingSize>0){
				busy = true;
				const std::vector<uint8_t> msg = connection->receiveData(incomingSize);
				if(!msg.empty()){
					incomingSize = 0;
					const uint16_t msgType = *reinterpret_cast<const uint16_t*>(msg.data());
//					std::cout << "msgType: "<<msgType;
					// receive value
					if(msgType==MSG_TYPE_VALUE){
						if(msg.size()<4 ){ // uint16_t msgType, uint16_t channelId
							dataError();
							return;
						}
						const uint16_t channelId = *reinterpret_cast<const uint16_t*>(msg.data()+2);
						const std::vector<uint8_t> data(std::next(msg.begin(), 4), msg.end());
						auto lock = Concurrency::createLock(*incomingValuesMutex);
						incomingValues.emplace_back(channelId, data);
					}// receive key value
					else if(msgType==MSG_TYPE_KEY_VALUE){
						if(msg.size()<6 ){ // uint16_t msgType, uint16_t channelId, uint16_t keyLength
							dataError();
							return;
						}
						const uint16_t channelId = *reinterpret_cast<const uint16_t*>(msg.data()+2);
						const uint16_t keyLength = *reinterpret_cast<const uint16_t*>(msg.data()+4);
						const uint32_t keyEnd = 6+keyLength;
						if(msg.size()<keyEnd){
							dataError();
							return;
						}
						const std::string key(std::next(msg.data(),6),std::next(msg.data(),keyEnd) );
						
						const std::vector<uint8_t> data(std::next(msg.begin(), keyEnd), msg.end());
						auto lock = Concurrency::createLock(*incomingKeyValuePairsMutex);
						incomingKeyValuePairs[std::make_pair(channelId, key)] = data;
					}else{
						WARN("DataConnection invalid message type");
					}
				}
				
			}
			if(!busy)
				Utils::sleep(1);
		}
	}
	void dataError(){
		connection->close();
		WARN("DataConnection data corruption.");
	}
	
	bool isOpen()const	{	return connection->isOpen();	}
	void close()		{	return connection->close();	}
	
};


//!(ctor)
DataConnection::DataConnection(TCPConnection * _connection) :
	incomingKeyValuePairsMutex(Concurrency::createMutex()),
	outgoingKeyValuePairsMutex(Concurrency::createMutex()),
	incomingValuesMutex(Concurrency::createMutex()),
	outgoingValuesMutex(Concurrency::createMutex()),
	keyValueChannelHandlersMutex(Concurrency::createMutex()),
	valueChannelHandlersMutex(Concurrency::createMutex()),
	internals(new WorkerThread(_connection,
							   incomingKeyValuePairsMutex.get(), incomingKeyValuePairs,
							   outgoingKeyValuePairsMutex.get(), outgoingKeyValuePairs,
							   incomingValuesMutex.get(), incomingValues,
							   outgoingValuesMutex.get(), outgoingValues)) {
	internals->start();
}

//!(dtor)
DataConnection::~DataConnection(){
	if(internals->isOpen())
		internals->close();
	if(internals->isActive())
		internals->join();
}

bool DataConnection::isOpen()const{
	return internals->isOpen();
}
void DataConnection::close(){
	return internals->close();
}
 // \todo FALLBACK_HANDLER
 
void DataConnection::handleIncomingData(float ms){
	Timer t;
	t.reset();
	bool busy = true;
	while(busy && (ms<0.0f || t.getMilliseconds()<ms)){
		busy = false;
		if(!incomingValues.empty()) // may be wrong!
		{
			auto lockI = Concurrency::createLock(*incomingValuesMutex);
			for(const auto & msg : incomingValues) {
				busy = true;
				const channelId_t channelId = msg.first;
				
				std::unordered_map<channelId_t, valueTypeHandler_t>::const_iterator handlerIt;
				bool handlerFound = false;
				{
					auto lock = Concurrency::createLock(*valueChannelHandlersMutex);
					handlerIt = valueChannelHandlers.find(channelId);
					handlerFound = handlerIt!=valueChannelHandlers.end();
					if(!handlerFound){
						handlerIt = valueChannelHandlers.find(FALLBACK_HANDLER);
						handlerFound = handlerIt!=valueChannelHandlers.end();
					}
				}
				if(handlerFound)
					handlerIt->second(channelId,msg.second);
			}
			incomingValues.clear();
		}
		if(!incomingKeyValuePairs.empty()) // may be wrong!
		{
			auto lockI = Concurrency::createLock(*incomingKeyValuePairsMutex);
			for(const auto & msg : incomingKeyValuePairs) {
				busy = true;
				const channelId_t channelId = msg.first.first;
				
				std::unordered_map<channelId_t, keyValueTypeHandler_t>::const_iterator handlerIt;
				bool handlerFound = false;
				{
					auto lock = Concurrency::createLock(*keyValueChannelHandlersMutex);
					handlerIt = keyValueChannelHandlers.find(channelId);
					handlerFound = handlerIt!=keyValueChannelHandlers.end();
					if(!handlerFound){
						handlerIt = keyValueChannelHandlers.find(FALLBACK_HANDLER);
						handlerFound = handlerIt!=keyValueChannelHandlers.end();
					}
				}
				if(handlerFound){
					handlerIt->second(channelId, msg.first.second, msg.second);
				}
			}
			incomingKeyValuePairs.clear();
		}
	}
}

}
}
