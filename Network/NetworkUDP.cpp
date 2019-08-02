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
#elif UTIL_HAVE_LIB_ASIO
#include <asio.hpp>
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
#elif UTIL_HAVE_LIB_ASIO
using namespace asio;
using namespace asio::ip;
extern io_context& getAsioContext();
extern IPv4Address fromAsioUdpEndpoint(const udp::endpoint & ep);
extern udp::endpoint toAsioUdpEndpoint(const IPv4Address & address);
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
		UDPsocket udpSocket;
#elif UTIL_HAVE_LIB_ASIO
	InternalUDPSocketData_t() : udpSocket(getAsioContext()) {}
		udp::socket udpSocket;
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
	data->udpSocket = nullptr;
#elif UTIL_HAVE_LIB_ASIO
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
	data->udpSocket = SDLNet_UDP_Open(data->port);
	if (!data->udpSocket) {
		std::cerr << "SDLNet_UDP_Open: " << SDLNet_GetError() << "\n";
		return false;
	}
#elif UTIL_HAVE_LIB_ASIO
	try {
		udp::endpoint endpoint(udp::v4(), data->port);
		//data->udpSocket.close();
		data->udpSocket.open(udp::v4());
		data->udpSocket.bind(endpoint);
	} catch(asio::system_error e) {
		WARN(e.what());
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
	return data->udpSocket != nullptr;
#elif UTIL_HAVE_LIB_ASIO
	return data->udpSocket.is_open();
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	return data->udpSocket > 0;
#else
	return false;
#endif
}

void UDPNetworkSocket::close() {
	if (isOpen()) {
#ifdef UTIL_HAVE_LIB_SDL2_NET
		SDLNet_UDP_Close(data->udpSocket);
		data->udpSocket = nullptr;
#elif UTIL_HAVE_LIB_ASIO
		try {
			data->udpSocket.close();
		} catch(asio::system_error e) {
			WARN(e.what());
		}
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
	int r = SDLNet_UDP_Recv(data->udpSocket, p);
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
#elif UTIL_HAVE_LIB_ASIO
	size_t len = data->udpSocket.available();
	if(len == 0)
		return nullptr;
	std::vector<uint8_t> buffer(data->maxPktSize + 1);
	udp::endpoint endpoint;
	data->udpSocket.receive_from(asio::buffer(buffer), endpoint);
	if (len == buffer.size()) {
		WARN("Maximum UDP packet size exceeded.");
	} else {
		buffer.resize(len);
	}
	auto packet = new Packet(std::move(buffer));
	packet->source = fromAsioUdpEndpoint(endpoint);
	return packet;
	
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	std::vector<uint8_t> buffer(data->maxPktSize + 1);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockaddr_in));
	socklen_t len = sizeof(sockAddr);
	const ssize_t bytesReceived = recvfrom(data->udpSocket, buffer.data(), buffer.size(), 0, reinterpret_cast<sockaddr *>(&sockAddr), &len);
	if (bytesReceived == -1) {
		int error = errno;
		if(error != EAGAIN && error != EWOULDBLOCK)
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
		sendCounter += SDLNet_UDP_Send(data->udpSocket, -1, p);
	}
	SDLNet_FreePacket(p);
	p = nullptr;
#elif UTIL_HAVE_LIB_ASIO
	for(const auto & target : data->targets) {
		auto endpoint = toAsioUdpEndpoint(target);
		try {
			data->udpSocket.send_to(asio::buffer(_data, _dataSize), endpoint);
			++sendCounter;
		} catch(asio::system_error e) {
			WARN(e.what());
		}
	}
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
	int i = SDLNet_UDP_Send(data->udpSocket, -1, p);
	SDLNet_FreePacket(p);
	p = nullptr;
	return i > 0;
#elif UTIL_HAVE_LIB_ASIO
	auto endpoint = toAsioUdpEndpoint(_address);
	try {
		data->udpSocket.send_to(asio::buffer(_data, _dataSize), endpoint);
	} catch(asio::system_error e) {
		WARN(e.what());
		return false;
	}
	return true;
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

uint16_t UDPNetworkSocket::getPort() const {
	if(data->port > 0)
		return data->port;
#ifdef UTIL_HAVE_LIB_SDL2_NET
	IPaddress* sdlAddress = SDLNet_UDP_GetPeerAddress(data->udpSocket, -1);
	if(!sdlAddress) {
		std::cerr << "SDLNet_UDP_GetPeerAddress: " << SDLNet_GetError() << "\n";
		return 0;
	}
	IPv4Address address = fromSDLIPv4Address(*sdlAddress);
	return address.getPort();
#elif UTIL_HAVE_LIB_ASIO
	return data->udpSocket.local_endpoint().port();
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	struct sockaddr_in localAddress;
	socklen_t addressLength = sizeof(localAddress);
	if(getsockname(data->udpSocket, reinterpret_cast<struct sockaddr*>(&localAddress), &addressLength)) {
		int error = errno;
		WARN(std::string(strerror(error)));
		return 0;
	}
	IPv4Address address = fromSockaddr(localAddress);
	return address.getPort();
#else
	return 0;
#endif
}

}
}
