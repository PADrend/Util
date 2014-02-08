/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "ClockSynchronizer.h"
#include "NetworkUDP.h"
#include "../Macros.h"
#include "../Timer.h"
#include "../Utils.h"
#include <iostream>

namespace Util {
namespace Network {


//! (static)
ClockSynchronizer * ClockSynchronizer::createServer(uint16_t port){
	auto socket = new UDPNetworkSocket(port);
	if(!socket->open()){
		delete socket;
		return nullptr;
	}
	return new ClockSynchronizer(SERVER,socket);
}

//! (static)
ClockSynchronizer * ClockSynchronizer::createClient(const IPv4Address & remoteIP){
	auto socket = new UDPNetworkSocket;
	if(!socket->open()){
		delete socket;
		return nullptr;
	}
	socket->addTarget(remoteIP);
	return new ClockSynchronizer(CLIENT,socket);
}

//! (ctor)
ClockSynchronizer::ClockSynchronizer(mode_t _mode, UDPNetworkSocket *socket ):
		Concurrency::UserThread(),
		state(CLOSED),mode(_mode),mySocket(socket),diff(0){
	Concurrency::UserThread::start();
}

//! (dtor)
ClockSynchronizer::~ClockSynchronizer(){
	close();
}

float ClockSynchronizer::getClockSec()const{
	return Timer::now()+diff;
}

//! ---|> UserThread
void ClockSynchronizer::run(){
	state=RUNNING;

	if(mode==SERVER){
		runServer();
	}else{
		runClient();
	}
	mySocket->close();
	mySocket.reset();
	state=CLOSED;
}

//! (internal)
void ClockSynchronizer::runServer(){
	const std::string requestString("rqTime");
	const std::vector<uint8_t> request(requestString.begin(), requestString.end());
	std::vector<uint8_t> answer(10);
	const std::string responseString("reTime");
	std::copy(responseString.begin(),responseString.end(),answer.begin());

	while(getState()==RUNNING){
		Utils::sleep(1); // todo: test if this does not harm too much
		UDPNetworkSocket::Packet * p=mySocket->receive();
		if(p==nullptr)
			continue;

		if( ! (p->packetData == request)){
			WARN( std::string("Unknown request: ")+p->getString());
			continue;
		}
//        std::cout << "Send answer "<<getClock()<<"\n";

		*(reinterpret_cast<float *> (answer.data() + 6)) = Timer::now();
		mySocket->sendData(answer.data(), answer.size(), p->source);

//        mySocket->addTarget(p->source);
//        *((uint32_t*)(answer.begin()+6))=(uint32_t)clock();
//        mySocket->sendData(answer);
//        std::cout << "Send answer "<<getClock()<<"\n";
//        mySocket->removeTarget(p->source);
	}
}

//! (internal)
void ClockSynchronizer::runClient(){
	const std::string requestString("rqTime");
	const std::vector<uint8_t> request(requestString.begin(), requestString.end());
	const std::string responseString="reTime";
	while(getState()==RUNNING){
		Utils::sleep(453); // todo: adjust later...

		const float startTime=Timer::now();
		mySocket->sendData(request.data(), request.size());
//        std::cout << "Send Request"<< mySocket<<" "<<getClock()<<" \n";

		UDPNetworkSocket::Packet * p=nullptr;
		while( p == nullptr) {
			p=mySocket->receive();
			if(Timer::now()-startTime > 1.0) //! \note Cl: this may be the cause of the disconnection bug!?!
				break;
		}
		if(p==nullptr){
			WARN( std::string("Sync-Server timeout."));
			continue;
		}
		if(p->packetData.size()!=10 || p->getString().find(responseString)!=0){
			WARN( std::string("Unknown answer: ")+p->getString());
			continue;
		}
		const float latency = (Timer::now() - startTime) * 0.5;
		float serverTime = (*(reinterpret_cast<float *> (p->packetData.data() + 6)) + latency);

		float newDiff = serverTime - Timer::now();
		diff= diff==0 ? newDiff : (diff*4.0 +newDiff)/5.0;

//        std::cout << "Latency:"<<latency <<"\tDiff: "<<newDiff<<"\tsmoothed:"<<diff<<"\n";
	}
}

void ClockSynchronizer::close(){
	if(getState()==RUNNING){
		state=CLOSING;
	}
	if(UserThread::isActive()){
		UserThread::join();
	}
}

}
}
