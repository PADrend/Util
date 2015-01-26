/*
	This file is part of the Util library.
	Copyright (C) 2007-2014 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "NetworkUDP.h"
#include "../Macros.h"
#include <ctime>
#include <iostream>
#include <map>
#include <set>

#ifdef UTIL_HAVE_LIB_SDL2_NET
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL_net.h>
COMPILER_WARN_POP
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace Util {
namespace Network {

#ifdef UTIL_HAVE_LIB_SDL2_NET
extern IPaddress toSDLIPv4Address(const IPv4Address & address);
extern IPv4Address fromSDLIPv4Address(const IPaddress & address);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
extern IPv4Address fromSockaddr(const sockaddr_in & sockAddr);
extern sockaddr_in toSockaddr(const IPv4Address & address);
#endif

typedef std::set<IPv4Address> targetSet_t;

struct InternalUDPSocketData_t {
		uint16_t port;
		int maxPktSize;
		targetSet_t targets;
#ifdef UTIL_HAVE_LIB_SDL2_NET
		UDPsocket udpsock;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
		int udpSocket;
#endif
};

//! (static)
UDPNetworkSocket * createUDPNetworkSocket(uint16_t port/*=0*/, int maxPktSize/*=1024*/) {
	return new UDPNetworkSocket(port, maxPktSize);
}

// ----------------------------

//! (ctor)
UDPNetworkSocket::UDPNetworkSocket(uint16_t _port/*=0*/, int _maxPktSize/*=1024*/) :
	data(new InternalUDPSocketData_t) {
	data->port = _port;
	data->maxPktSize = _maxPktSize;
#ifdef UTIL_HAVE_LIB_SDL2_NET
	data->udpsock = nullptr;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	data->udpSocket = 0;
#endif
}


//! (dtor)
UDPNetworkSocket::~UDPNetworkSocket() {
	close();
}

/**
 * Open a UPD-Socket.
 * If _port_ is 0, a random port is chosen and written to _port_.
 * \return true if successful
 */
bool UDPNetworkSocket::open() {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	data->udpsock = SDLNet_UDP_Open(data->port);
	if (!data->udpsock) {
		std::cerr << "SDLNet_UDP_Open: " << SDLNet_GetError() << "\n";
		return false;
	}
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	data->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (data->udpSocket == -1) {
		int error = errno;
		WARN(std::string(strerror(error)));
		return false;
	}
	// Activate nonblocking operation.
	int statusFlags = fcntl(data->udpSocket, F_GETFL);
	if(statusFlags == -1) {
		int error = errno;
		WARN(std::string(strerror(error)));
		return false;
	}
	int result = fcntl(data->udpSocket, F_SETFL, statusFlags | O_NONBLOCK);
	if(result == -1) {
		int error = errno;
		WARN(std::string(strerror(error)));
		return false;
	}

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockaddr_in));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(data->port);
	sockAddr.sin_addr.s_addr = 0x00000000; // INADDR_ANY without old-style cast
	result = bind(data->udpSocket, reinterpret_cast<const sockaddr *> (&sockAddr), sizeof(sockaddr_in));
	if (result == -1) {
		int error = errno;
		WARN(std::string(strerror(error)));
		return false;
	}
#endif
	return true;
}

bool UDPNetworkSocket::isOpen() const {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	return data->udpsock != nullptr;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	return data->udpSocket > 0;
#else
	return false;
#endif
}

void UDPNetworkSocket::close() {
	if (isOpen()) {
#ifdef UTIL_HAVE_LIB_SDL2_NET
		SDLNet_UDP_Close(data->udpsock);
		data->udpsock = nullptr;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
		if (::close(data->udpSocket) != 0) {
			int error = errno;
			WARN(std::string(strerror(error)));
		}
		data->udpSocket = 0;
#endif
	}
}

UDPNetworkSocket::Packet * UDPNetworkSocket::receive() {
	if (!isOpen()) 
		return nullptr;

#ifdef UTIL_HAVE_LIB_SDL2_NET
	UDPpacket * p = SDLNet_AllocPacket(data->maxPktSize + 1);
	if (p == nullptr) {
		std::cerr << "SDLNet_AllocPacket: " << SDLNet_GetError() << "\n";
		return nullptr;
	}
	int r = SDLNet_UDP_Recv(data->udpsock, p);
	if (r <= 0) {
		SDLNet_FreePacket(p);
		return nullptr;
	}
	if (p->len == p->maxlen) {
		WARN("Maximum UDP packet size exceeded.");
	}
	auto myPacket = new Packet(p->data, p->len);
	myPacket->source = fromSDLIPv4Address(p->address);

	SDLNet_FreePacket(p);
	return myPacket;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	std::vector<uint8_t> buffer(data->maxPktSize + 1);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockaddr_in));
	socklen_t len = sizeof(sockAddr);
	const ssize_t bytesReceived = recvfrom(data->udpSocket, buffer.data(), buffer.size(), 0, reinterpret_cast<sockaddr *>(&sockAddr), &len);
	if (bytesReceived == -1) {
		int error = errno;
		WARN(std::string(strerror(error)));
		return nullptr;
	}
	if (bytesReceived == buffer.size()) {
		WARN("Maximum UDP packet size exceeded.");
	} else {
		buffer.resize(bytesReceived);
	}
	Packet * packet = new Packet(std::move(buffer));
	packet->source = fromSockaddr(sockAddr);
	return packet;
#else
	return nullptr;
#endif
}

//! \return number of targets
int UDPNetworkSocket::sendString(const std::string & _data) {
	return sendData(reinterpret_cast<const uint8_t *> (_data.c_str()), _data.length());
}

//! \return number of targets
int UDPNetworkSocket::sendData(const uint8_t * _data, size_t _dataSize) {
	if (!isOpen() || data->targets.empty() )
		return 0;

	int sendCounter = 0;
#ifdef UTIL_HAVE_LIB_SDL2_NET
	UDPpacket * p = SDLNet_AllocPacket(_dataSize);

	if (p == nullptr) {
		std::cerr << "SDLNet_AllocPacket: " << SDLNet_GetError() << "\n";
		return sendCounter;
	}
	std::copy(_data, _data + _dataSize, p->data);
	p->len = _dataSize;
	for(const auto & target : data->targets) {
		p->address = toSDLIPv4Address(target);
		sendCounter += SDLNet_UDP_Send(data->udpsock, -1, p);
	}
	SDLNet_FreePacket(p);
	p = nullptr;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	for(const auto & target : data->targets) {
		sockaddr_in sockAddr = toSockaddr(target);
		ssize_t bytesSent = sendto(data->udpSocket, _data, _dataSize, 0, reinterpret_cast<const sockaddr *> (&sockAddr), sizeof(sockaddr_in));
		if (bytesSent == -1) {
			int error = errno;
			WARN(std::string(strerror(error)));
		} else {
			++sendCounter;
		}
	}
#endif
	return sendCounter;
}

bool UDPNetworkSocket::sendData(const uint8_t * _data, size_t _dataSize, const IPv4Address & _address) {
	if (!isOpen())
		return false;
#ifdef UTIL_HAVE_LIB_SDL2_NET
	UDPpacket * p = SDLNet_AllocPacket(_dataSize);
	std::copy(_data, _data + _dataSize, p->data);
	p->len = _dataSize;
	p->address = toSDLIPv4Address(_address);
	int i = SDLNet_UDP_Send(data->udpsock, -1, p);
	SDLNet_FreePacket(p);
	p = nullptr;
	return i > 0;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	sockaddr_in sockAddr = toSockaddr(_address);
	ssize_t bytesSent = sendto(data->udpSocket, _data, _dataSize, 0, reinterpret_cast<const sockaddr *> (&sockAddr), sizeof(sockAddr));
	if (bytesSent == -1) {
		int error = errno;
		WARN(std::string(strerror(error)));
		return false;
	}
	return true;
#else
	return false;
#endif
}

void UDPNetworkSocket::addTarget(const IPv4Address & address) {
	if (address.isValid())
		data->targets.insert(address);
}

void UDPNetworkSocket::removeTarget(const IPv4Address & address) {
	data->targets.erase(address);
}

}
}
