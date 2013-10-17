/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Network.h"
#include "../Macros.h"
#include "../StringUtils.h"
#include <cstdlib>
#include <ctime>
#include <memory>
#include <sstream>

#ifdef UTIL_HAVE_LIB_SDL2_NET
#include <SDL_net.h>
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#endif

namespace Util {
namespace Network {

/**
 * (static) Init network system (SDL_net).
 */
bool init() {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	if (SDLNet_Init() < 0) {
		WARN("SDLNet_Init: " + std::string(SDLNet_GetError()));
		return false;
	}
#endif /* UTIL_HAVE_LIB_SDL2_NET */
	return true;
}
// -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------


struct InternalIPAddressData_t {
#ifdef UTIL_HAVE_LIB_SDL2_NET
		IPaddress sdlAddress;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
		sockaddr_in address;
#endif
};

#ifdef UTIL_HAVE_LIB_SDL2_NET
IPAddress fromSDLIPAddress(const IPaddress & sdlIp);
IPAddress fromSDLIPAddress(const IPaddress & sdlIp) {
	InternalIPAddressData_t data;
	data.sdlAddress = sdlIp;
	return IPAddress(data);
}
IPaddress toSDLIPAddress(const IPAddress & address);
IPaddress toSDLIPAddress(const IPAddress & address) {
	return address.getData()->sdlAddress;
}
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
IPAddress fromSockaddr(const sockaddr_in & sockAddr);
IPAddress fromSockaddr(const sockaddr_in & sockAddr) {
	InternalIPAddressData_t data;
	data.address = sockAddr;
	return IPAddress(data);
}
sockaddr_in toSockaddr(const IPAddress & address);
sockaddr_in toSockaddr(const IPAddress & address) {
	return address.getData()->address;
}
#endif

IPAddress::IPAddress() :
	data(new InternalIPAddressData_t) {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	setHost(0);
	setPort(0);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	memset(&(data->address), 0, sizeof(sockaddr_in));
#endif
}

IPAddress::IPAddress(const InternalIPAddressData_t & internalData) :
	data(new InternalIPAddressData_t(internalData)) {
}

IPAddress::IPAddress(uint32_t _host, uint16_t _port) :
	data(new InternalIPAddressData_t) {
	setHost(_host);
	setPort(_port);
}
IPAddress::IPAddress(const IPAddress & source) :
	data(new InternalIPAddressData_t(*source.data)) {
}
IPAddress::~IPAddress() = default;
IPAddress & IPAddress::operator=(const IPAddress & source) {
	// Handle self assignment gracefully.
	*data = *source.data;
	return *this;
}
void IPAddress::setHost(uint32_t host) {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	data->sdlAddress.host = ((host & 0x000000FF) << 24) + ((host & 0x0000FF00) << 8) + ((host & 0x00FF0000) >> 8) + ((host & 0xFF000000) >> 24);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	data->address.sin_addr.s_addr = htonl(host);
#endif
}
void IPAddress::setPort(uint16_t port) {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	data->sdlAddress.port = ((port & 0x00FF) << 8) + ((port & 0xFF00) >> 8);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	data->address.sin_port = htons(port);
#endif
}
bool IPAddress::operator<(const IPAddress & p) const {
	return getHost() != p.getHost() ? (getHost() < p.getHost()) : (getPort() < p.getPort());
}
bool IPAddress::isValid() const {
	return getHost() != 0 && getPort() != 0;
}
std::string IPAddress::toString() const {
	std::ostringstream s;
	s << getHostAsString() << ":" << getPort();
	return s.str();
}
std::string IPAddress::getHostAsString() const {
	std::ostringstream s;
	const uint32_t host = getHost();
	s << ((host & 0xFF000000) >> 24) << "." << ((host & 0xFF0000) >> 16) << "." << ((host & 0xFF00) >> 8) << "." << (host & 0xFF);
	return s.str();
}
uint32_t IPAddress::getHost() const {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	const uint32_t & host = data->sdlAddress.host;
	return ((host & 0x000000FF) << 24) + ((host & 0x0000FF00) << 8) + ((host & 0x00FF0000) >> 8) + ((host & 0xFF000000) >> 24);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	return ntohl(data->address.sin_addr.s_addr);
#else
	return 0;
#endif
}
uint16_t IPAddress::getPort() const {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	return ((data->sdlAddress.port & 0x00FF) << 8) + ((data->sdlAddress.port & 0xFF00) >> 8);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	return ntohs(data->address.sin_port);
#else
	return 0;
#endif
}
const InternalIPAddressData_t * IPAddress::getData() const {
	return data.get();
}
IPAddress IPAddress::resolveHost(const std::string & host, uint16_t port) {
	IPAddress ip;
#ifdef UTIL_HAVE_LIB_SDL2_NET
	if (SDLNet_ResolveHost(&(ip.data->sdlAddress), host.c_str(), port) == -1) {
		return IPAddress();
	}
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	const std::string portString(StringUtils::toString(port));
	addrinfo hints;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_INET; // IPv4
	addrinfo * info;
	int result = getaddrinfo(host.c_str(), portString.c_str(), &hints, &info);
	if (result != 0) {
		WARN(std::string(gai_strerror(result)));
		return IPAddress();
	}
	ip.data->address = *reinterpret_cast<sockaddr_in *> (info->ai_addr);
	freeaddrinfo(info);
#endif
	return ip;
}
// -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------


}
}
