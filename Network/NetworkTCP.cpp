/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012,2015 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "NetworkTCP.h"
#include "../Macros.h"
#include "../Timer.h"
#include "../Utils.h"
#include <algorithm>
#include <iostream>

#ifdef UTIL_HAVE_LIB_SDL2_NET
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL_net.h>
COMPILER_WARN_POP
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <unistd.h>
#endif
#include <cstdint>
#include <vector>

namespace Util {
namespace Network {


#ifdef UTIL_HAVE_LIB_SDL2_NET

extern IPaddress toSDLIPv4Address(const IPv4Address & address);
extern IPv4Address fromSDLIPv4Address(const IPaddress & address);

class TCPConnection::Implementation {
		TCPsocket tcpSocket;
		SDLNet_SocketSet socketSet;
		uint8_t buffer[BUFFER_SIZE];
	public:
		const IPv4Address remoteIp;
		
		Implementation(const IPv4Address & remoteIp_): remoteIp(remoteIp_){
			IPaddress sdlIp = toSDLIPv4Address(remoteIp);

			tcpSocket = SDLNet_TCP_Open(&sdlIp);
			if(!tcpSocket)
				throw std::runtime_error(std::string("SDLNet_TCP_Open: ") + SDLNet_GetError());

			socketSet = SDLNet_AllocSocketSet(1);
			SDLNet_TCP_AddSocket(socketSet, tcpSocket);
		}
		Implementation(TCPsocket&& tcpSocket_,const IPv4Address & remoteIp_): tcpSocket(tcpSocket_),remoteIp(remoteIp_){
			socketSet = SDLNet_AllocSocketSet(1);
			SDLNet_TCP_AddSocket(socketSet, tcpSocket);
		}
		
		bool doSendData(std::vector<uint8_t> & data){
			const int len = data.size();
			if(SDLNet_TCP_Send(tcpSocket, reinterpret_cast<const void *>(data.data()), len) < len){
				WARN("TCPConnection error: doSendData");
				return false;
			}
			return true;
		}
		std::tuple<std::vector<uint8_t>,bool> doReceiveData(){ // -> receivedData,keep open?
			const int socketReady = SDLNet_CheckSockets(socketSet, 0);
			if(socketReady == 0) { // no data received
				return std::make_tuple(std::vector<uint8_t>(),true);
			} else if(socketReady < 0) {
				WARN("TCPConnection error in doReceiveData.");
				return std::make_tuple(std::vector<uint8_t>(),false);
			}
			const int bytesReceived = SDLNet_TCP_Recv(tcpSocket, buffer, BUFFER_SIZE);
			if(bytesReceived <= 0) { // connection closed
				return std::make_tuple(std::vector<uint8_t>(),false);
			}
			return std::make_tuple(std::vector<uint8_t>(buffer,buffer + static_cast<size_t>(bytesReceived)),true);
		}
		
		void doClose(){
			SDLNet_FreeSocketSet(socketSet);
			SDLNet_TCP_Close(tcpSocket);
		}
};

#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)

extern IPv4Address fromSockaddr(const sockaddr_in & sockAddr);
extern sockaddr_in toSockaddr(const IPv4Address & address);

class TCPConnection::Implementation {
		int tcpSocket;
		uint8_t buffer[BUFFER_SIZE];
		pollfd pollDescr;
	public:
		const IPv4Address remoteIp;

		Implementation(const IPv4Address & remoteIp_):remoteIp(remoteIp_){
			tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
			if(tcpSocket == -1) {
				const int error = errno;
				throw std::runtime_error(std::string(strerror(error)));
			}
			sockaddr_in sockAddr = toSockaddr(remoteIp);
			int result = ::connect(tcpSocket, reinterpret_cast<const sockaddr *>(&sockAddr), sizeof(sockaddr_in));
			if(result == -1) {
				const int error = errno;
				throw std::runtime_error(std::string(strerror(error)));
			}
			const int optionTrue = 1;
			setsockopt(tcpSocket, IPPROTO_TCP, TCP_NODELAY, &optionTrue, sizeof(int));
			pollDescr.fd = tcpSocket;
		}
		Implementation(int clientSocket, const IPv4Address & remoteIp_):tcpSocket(clientSocket),remoteIp(remoteIp_){
			pollDescr.fd = tcpSocket;
		}
		
		bool doSendData(std::vector<uint8_t> & data){
			const int len = data.size();
			if(send(tcpSocket, data.data(), len, 0) < len) {
				const int error = errno;
				WARN(std::string(strerror(error)));
				return false;
			}
			return true;
		}
		std::tuple<std::vector<uint8_t>,bool> doReceiveData(){ // -> receivedData,keep open?
			pollDescr.events = POLLIN;
			pollDescr.revents = 0;
			const int result = poll(&pollDescr, 1, 1); // Check if reading is possible. Wait at most 1 ms.

			if(result == 0) {		// Socket is not ready for reading yet. Continue with writing.
				return std::make_tuple(std::vector<uint8_t>(),true);
			} else if(result < 0) { // error
				const int error = errno;
				WARN(std::string(strerror(error)));
				return std::make_tuple(std::vector<uint8_t>(),false);
			}else if(pollDescr.revents != POLLIN) {	// An error event was returned, because only POLLIN was requested.
				const int error = errno;
				WARN(std::string(strerror(error)));
				return std::make_tuple(std::vector<uint8_t>(),false);
			}
			int bytesReceived = recv(tcpSocket, buffer, BUFFER_SIZE, 0);
			if(bytesReceived == 0) {		// Peer has shut down.
				return std::make_tuple(std::vector<uint8_t>(),false);
			} else if(bytesReceived < 0) {	// error
				const int error = errno;
				WARN(std::string(strerror(error)));
				return std::make_tuple(std::vector<uint8_t>(),false);
			}else{
				return std::make_tuple(std::vector<uint8_t>(buffer,buffer + static_cast<size_t>(bytesReceived)),true);
			}
		}
		void doClose(){
			if(::close(tcpSocket) != 0) {
				const int error = errno;
				WARN(std::string(strerror(error)));
			}
			tcpSocket = 0;
		}
};
#endif

// ----------------------------------------------------------------------------------------

//! (static) Factory
Reference<TCPConnection> TCPConnection::connect(const IPv4Address & remoteIp) {
	return new TCPConnection(new TCPConnection::Implementation(remoteIp));
}


//! (ctor)
TCPConnection::TCPConnection(Implementation * implementation_) :
		implementation(implementation_), state(OPEN), inQueueDataSize(0),
		inQueueMutex(), outQueueMutex(),
		thread(std::bind(&TCPConnection::run, this)) {
}

//! (dtor)
TCPConnection::~TCPConnection() {
	close();
}

IPv4Address TCPConnection::getRemoteIp() const {
	return implementation->remoteIp;
}

bool TCPConnection::sendData(const std::vector<uint8_t> & data){
	if(!isOpen())
		return false;
	std::lock_guard<std::mutex> lock(outQueueMutex);
	outQueue.emplace_back(data);
	return true;
}

bool TCPConnection::sendString(const std::string & s) {
	return sendData(std::vector<uint8_t>(s.begin(), s.end()));
}

//! ---|> ThreadObject
void TCPConnection::run() {
	lastActiveTime = Timer::now();
	while(isOpen()) {
		Utils::sleep(1);

		// send outgoing data
		if(!outQueue.empty()){ // this may give a wrong result, but requires no locking!
			std::lock_guard<std::mutex> lock(outQueueMutex);
			while(!outQueue.empty()) {
				if( !implementation->doSendData( outQueue.front() )) {
					setState(CLOSING);
					WARN("TCPConnection could not send data.");
					break;
				}
				outQueue.pop_front();
			}
		}
		// receive data
		while(isOpen()) {
			std::tuple<std::vector<uint8_t>,bool> receivedDataAndStatus = implementation->doReceiveData();
			if( !std::get<1>(receivedDataAndStatus) ){
				setState(CLOSING);
				break;
			}else if( std::get<0>(receivedDataAndStatus).empty() ){
				break;
			}else{
				std::lock_guard<std::mutex> lock(inQueueMutex);
				inQueueDataSize += static_cast<size_t>(std::get<0>(receivedDataAndStatus).size());
				inQueue.emplace_back( std::move( std::get<0>(receivedDataAndStatus)) );
				lastActiveTime = Timer::now();
			}
		}
	}
	implementation->doClose();
}

void TCPConnection::close() {
	if(!isClosed()){
		if(isOpen())
			setState(CLOSING);
		thread.join();
		setState(CLOSED);
	}
}

//! (internal) \note caller needs to lock inQueue
std::vector<uint8_t> TCPConnection::extractDataFromInQueue(size_t numBytes) {
	if(inQueueDataSize < numBytes || numBytes == 0)
		return std::vector<uint8_t>();

	std::vector<uint8_t> data;
	data.reserve(numBytes);
	size_t remainingBytes = numBytes;
	size_t packetsToRemove = 0;
	for (auto it = inQueue.begin(); it != inQueue.end() && remainingBytes > 0; ++it) {
		// take full packet
		if(remainingBytes >= it->size()) {
			data.insert(data.end(), it->begin(), it->end());
			remainingBytes -= it->size();
			++packetsToRemove;
		} else { // only take remaining data
			data.insert(data.end(), it->begin(), it->begin() + remainingBytes);
			const std::vector<uint8_t> remainder(it->begin() + remainingBytes, it->end());
			*it = remainder;
			remainingBytes = 0;
		}
	}
	// remove packets from queue
	if(packetsToRemove == inQueue.size()) {
		inQueue.clear();
	} else {
		for (; packetsToRemove > 0; --packetsToRemove) {
			inQueue.pop_front();
		}
	}
	inQueueDataSize -= numBytes;
	return data;
}


std::vector<uint8_t> TCPConnection::receiveData() {
	if(inQueueDataSize>0){ // use without locking!
		std::lock_guard<std::mutex> lock(inQueueMutex);
		return extractDataFromInQueue(inQueueDataSize);
	}else{
		return std::vector<uint8_t>();
	}
}


std::vector<uint8_t> TCPConnection::receiveData(size_t numBytes) {
	if(inQueueDataSize>=numBytes){ // use without locking!
		std::lock_guard<std::mutex> lock(inQueueMutex);
		return extractDataFromInQueue(numBytes);
	}else{
		return std::vector<uint8_t>();
	}
}


std::string TCPConnection::receiveString(char delimiter/*='\0'*/) {
	if(inQueueDataSize==0){ // use without locking!
		return "";
	}else{
		std::lock_guard<std::mutex> lock(inQueueMutex);

		// search form delimiter
		size_t pos = 0;
		bool found = false;
		for(auto package = inQueue.cbegin(); package != inQueue.cend() && !found; ++package) {
			for(const auto & byteEntry : *package) {
				if(byteEntry == static_cast<uint8_t>(delimiter)) {
					found = true;
					break;
				}
				pos++;
			}
		}

		// extract string
		std::string s;
		if(found) {
			const std::vector<uint8_t> d = extractDataFromInQueue(pos + 1);
			if(!d.empty())
				s.assign(d.begin(), d.end());
		}
		return s;
	}
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// [TCPServer]

#ifdef UTIL_HAVE_LIB_SDL2_NET
class TCPServer::Implementation {
		TCPsocket serverSocket;
	
	public:
		static Implementation* create(uint16_t port){
			IPaddress sdlIp;
			/// Create server-socket
			int val = SDLNet_ResolveHost(&sdlIp, nullptr, port);
			if(-1 == val) 
				throw std::runtime_error("Cannot resolve host name or address");
			TCPsocket serverSocket = SDLNet_TCP_Open(&sdlIp);
			if(!serverSocket)
				throw std::runtime_error(std::string("SDLNet_TCP_Open: ") + SDLNet_GetError());

			return new Implementation(std::move(serverSocket));
		}

		Implementation(TCPsocket socket) : serverSocket(std::forward<TCPsocket>(socket)) {}

		std::tuple<Reference<TCPConnection>,bool> doGetIncomingConnection(){ // -> connection, ok?
			TCPsocket clientSocket = SDLNet_TCP_Accept(serverSocket);
			if(clientSocket) {
				IPaddress * remote_sdlIp = SDLNet_TCP_GetPeerAddress(clientSocket);
				auto remoteIp = fromSDLIPv4Address(*remote_sdlIp);
				return std::make_tuple(new TCPConnection(new TCPConnection::Implementation(std::move(clientSocket), remoteIp)),true);
			}
			return std::make_tuple(nullptr,true);
		}
		void doClose(){
			SDLNet_TCP_Close(serverSocket);
		}

};
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
class TCPServer::Implementation {
		int tcpServerSocket;
	
	public:
		static Implementation* create(uint16_t port){
			auto tcpServerSocket = socket(AF_INET, SOCK_STREAM, 0);
			if(tcpServerSocket == -1) {
				const int error = errno;
				throw std::runtime_error(std::string(strerror(error)));
			}
			// Enable the socket to be bound to a previously used address again.
			const int optionTrue = 1;
			{
				const int result = setsockopt(tcpServerSocket, SOL_SOCKET, SO_REUSEADDR, &optionTrue, sizeof(int));
				if(result == -1) {
					const int error = errno;
					throw std::runtime_error(std::string(strerror(error)));
				}
			}
			{
				const int result = setsockopt(tcpServerSocket, IPPROTO_TCP, TCP_NODELAY, &optionTrue, sizeof(int));
				if(result == -1) {
					const int error = errno;
					throw std::runtime_error(std::string(strerror(error)));
				}
			}
			{
				sockaddr_in sockAddr;
				memset(&sockAddr, 0, sizeof(sockaddr_in));
				sockAddr.sin_family = AF_INET;
				sockAddr.sin_port = htons(port);
				sockAddr.sin_addr.s_addr = 0x00000000; // INADDR_ANY without old-style cast
				const int result = ::bind(tcpServerSocket, reinterpret_cast<const sockaddr *> (&sockAddr), sizeof(sockaddr_in));
				if(result == -1) {
					const int error = errno;
					throw std::runtime_error(std::string(strerror(error)));
				}
			}
			{
				const int result = listen(tcpServerSocket, 8);
				if(result == -1) {
					const int error = errno;
					throw std::runtime_error(std::string(strerror(error)));
				}
			}
			return new Implementation(tcpServerSocket);
		}

		Implementation(int socket) : tcpServerSocket(socket) {}
		
		std::tuple<Reference<TCPConnection>,bool> doGetIncomingConnection(){ // -> connection, ok?
			pollfd set;
			set.fd = tcpServerSocket;
			set.events = POLLIN;
			set.revents = 0;

			// Check if a readable event is available (see accept(2)).
			const int result = poll(&set, 1, 5);
			if(result < 0) {
				const int error = errno;
				WARN(std::string(strerror(error)));
				return std::make_tuple(nullptr,false);
			}else if(result > 0) {
				if(set.revents != POLLIN) {
					const int error = errno;
					WARN(std::string(strerror(error)));
					return std::make_tuple(nullptr,false);
				}

				sockaddr_in clientAddr;
				socklen_t clientAddrSize = sizeof(sockaddr_in);
				int clientSocket = accept(tcpServerSocket, reinterpret_cast<sockaddr *> (&clientAddr), &clientAddrSize);

				auto remoteIp = fromSockaddr(clientAddr);
				return std::move(std::make_tuple(new TCPConnection(new TCPConnection::Implementation(clientSocket, remoteIp)),true));
			}
			return std::make_tuple(nullptr,true);
		}
		void doClose(){
			if(::close(tcpServerSocket) != 0) {
				const int error = errno;
				WARN(std::string(strerror(error)));
			}
			tcpServerSocket = 0;
		}		
};
#endif

//! (static) Factory
TCPServer * TCPServer::create(uint16_t port) {
	return new TCPServer(std::move(Implementation::create(port)));
}

//! (ctor) TCPServer
TCPServer::TCPServer(Implementation * implementation_) :
		implementation(implementation_), 
		state(OPEN), 
		queueMutex(),
		thread(std::bind(&TCPServer::run, this)) {
}

//! (dtor) TCPServer
TCPServer::~TCPServer() {
	close();
}

Reference<TCPConnection> TCPServer::getIncomingConnection() {
	std::lock_guard<std::mutex> lock(queueMutex);
	Reference<TCPConnection> connection;
	if(!incomingConnections.empty()) {
		connection = incomingConnections.front();
		incomingConnections.pop_front();
	}
	return connection;
}

void TCPServer::run() {
	while(isOpen()) {
		Utils::sleep(1);
		std::tuple<Reference<TCPConnection>,bool> incomingConnectionAndStatus( implementation->doGetIncomingConnection() );
		if(std::get<0>(incomingConnectionAndStatus)){
			std::lock_guard<std::mutex> lock(queueMutex);
			incomingConnections.emplace_back(std::move(std::get<0>(incomingConnectionAndStatus)));
		}else if(!std::get<1>(incomingConnectionAndStatus)){ // error occurred
			setState(CLOSING);
		}
	}
	implementation->doClose();
}

void TCPServer::close() {
	if(!isClosed()){
		if(isOpen())
			setState(CLOSING);
		thread.join();
		setState(CLOSED);
	}
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		while(!incomingConnections.empty()) {
			incomingConnections.front()->close();
			incomingConnections.pop_front();
		}
	}
}

}
}
