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

#ifdef UTIL_HAVE_LIB_SDL2_NET
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL_net.h>
COMPILER_WARN_POP
#elif UTIL_HAVE_LIB_ASIO
#include <asio.hpp>
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#endif

#include "../Macros.h"
#include "../StringUtils.h"
#include <cstdlib>
#include <ctime>
#include <memory>
#include <sstream>

#include "../LibRegistry.h"
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define SDL_NET_FULL_VERSION_STRING  "SDL_net " STR(SDL_NET_MAJOR_VERSION) "." STR(SDL_NET_MINOR_VERSION) "." STR(SDL_NET_PATCHLEVEL) " (www.libsdl.org/projects/SDL_net/)"

static bool libNameInitailized = [](){	
	Util::LibRegistry::registerLibVersionString("LibSDL2Net",SDL_NET_FULL_VERSION_STRING); 
	return true;
}();

namespace Util {
namespace Network {
	
#ifdef UTIL_HAVE_LIB_ASIO
using namespace asio::ip;
#endif
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

#ifdef UTIL_HAVE_LIB_ASIO
asio::io_context& getAsioContext() {
	static asio::io_context context;
	if(context.stopped())
		context.run();
	return context;
}
#endif

struct InternalIPv4AddressData_t {
#ifdef UTIL_HAVE_LIB_SDL2_NET
		IPaddress sdlAddress;
#elif UTIL_HAVE_LIB_ASIO
	address_v4 host;
	uint16_t port;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
		sockaddr_in address;
#endif
};

#ifdef UTIL_HAVE_LIB_SDL2_NET
IPv4Address fromSDLIPv4Address(const IPaddress & sdlIp);
IPv4Address fromSDLIPv4Address(const IPaddress & sdlIp) {
	InternalIPv4AddressData_t data;
	data.sdlAddress = sdlIp;
	return IPv4Address(data);
}
IPaddress toSDLIPv4Address(const IPv4Address & address);
IPaddress toSDLIPv4Address(const IPv4Address & address) {
	return address.getData()->sdlAddress;
}
#elif UTIL_HAVE_LIB_ASIO
IPv4Address fromAsioUdpEndpoint(const udp::endpoint & ep) {
	InternalIPv4AddressData_t data;
	data.host = ep.address().to_v4();
	data.port = ep.port();
	return IPv4Address(data);
}
IPv4Address fromAsioTcpEndpoint(const tcp::endpoint & ep) {
	InternalIPv4AddressData_t data;
	data.host = ep.address().to_v4();
	data.port = ep.port();
	return IPv4Address(data);
}
udp::endpoint toAsioUdpEndpoint(const IPv4Address & address) {
	return udp::endpoint(address.getData()->host, address.getData()->port);
}
tcp::endpoint toAsioTcpEndpoint(const IPv4Address & address) {
	return tcp::endpoint(address.getData()->host, address.getData()->port);
}
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
IPv4Address fromSockaddr(const sockaddr_in & sockAddr);
IPv4Address fromSockaddr(const sockaddr_in & sockAddr) {
	InternalIPv4AddressData_t data;
	data.address = sockAddr;
	return IPv4Address(data);
}
sockaddr_in toSockaddr(const IPv4Address & address);
sockaddr_in toSockaddr(const IPv4Address & address) {
	return address.getData()->address;
}
#endif

IPv4Address::IPv4Address() :
	data(new InternalIPv4AddressData_t) {
#if defined(UTIL_HAVE_LIB_SDL2_NET) || defined(UTIL_HAVE_LIB_ASIO)
	setHost(0);
	setPort(0);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	memset(&(data->address), 0, sizeof(sockaddr_in));
#endif
}

IPv4Address::IPv4Address(const InternalIPv4AddressData_t & internalData) :
	data(new InternalIPv4AddressData_t(internalData)) {
}

IPv4Address::IPv4Address(uint32_t _host, uint16_t _port) :
	data(new InternalIPv4AddressData_t) {
	setHost(_host);
	setPort(_port);
}
IPv4Address::IPv4Address(const IPv4Address & source) :
	data(new InternalIPv4AddressData_t(*source.data)) {
}
IPv4Address::~IPv4Address() = default;
IPv4Address & IPv4Address::operator=(const IPv4Address & source) {
	// Handle self assignment gracefully.
	*data = *source.data;
	return *this;
}
void IPv4Address::setHost(uint32_t host) {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	data->sdlAddress.host = ((host & 0x000000FF) << 24) + ((host & 0x0000FF00) << 8) + ((host & 0x00FF0000) >> 8) + ((host & 0xFF000000) >> 24);
#elif UTIL_HAVE_LIB_ASIO
	data->host = make_address_v4(host);
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	data->address.sin_addr.s_addr = htonl(host);
#endif
}
void IPv4Address::setPort(uint16_t port) {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	data->sdlAddress.port = ((port & 0x00FF) << 8) + ((port & 0xFF00) >> 8);
#elif UTIL_HAVE_LIB_ASIO
	data->port = port;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	data->address.sin_port = htons(port);
#endif
}
bool IPv4Address::operator<(const IPv4Address & p) const {
	return getHost() != p.getHost() ? (getHost() < p.getHost()) : (getPort() < p.getPort());
}
bool IPv4Address::isValid() const {
	return getHost() != 0 && getPort() != 0;
}
std::string IPv4Address::toString() const {
	std::ostringstream s;
	s << getHostAsString() << ":" << getPort();
	return s.str();
}
std::string IPv4Address::getHostAsString() const {
	std::ostringstream s;
	const uint32_t host = getHost();
	s << ((host & 0xFF000000) >> 24) << "." << ((host & 0xFF0000) >> 16) << "." << ((host & 0xFF00) >> 8) << "." << (host & 0xFF);
	return s.str();
}
uint32_t IPv4Address::getHost() const {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	const uint32_t & host = data->sdlAddress.host;
	return ((host & 0x000000FF) << 24) + ((host & 0x0000FF00) << 8) + ((host & 0x00FF0000) >> 8) + ((host & 0xFF000000) >> 24);
#elif UTIL_HAVE_LIB_ASIO
	return data->host.to_uint();
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	return ntohl(data->address.sin_addr.s_addr);
#else
	return 0;
#endif
}
uint16_t IPv4Address::getPort() const {
#ifdef UTIL_HAVE_LIB_SDL2_NET
	return ((data->sdlAddress.port & 0x00FF) << 8) + ((data->sdlAddress.port & 0xFF00) >> 8);
#elif UTIL_HAVE_LIB_ASIO
	return data->port;
#elif defined(__linux__) || defined(__unix__) || defined(ANDROID)
	return ntohs(data->address.sin_port);
#else
	return 0;
#endif
}
const InternalIPv4AddressData_t * IPv4Address::getData() const {
	return data.get();
}
IPv4Address IPv4Address::resolveHost(const std::string & host, uint16_t port) {
	IPv4Address ip;
#ifdef UTIL_HAVE_LIB_SDL2_NET
	if (SDLNet_ResolveHost(&(ip.data->sdlAddress), host.c_str(), port) == -1) {
		return IPv4Address();
	}
#elif UTIL_HAVE_LIB_ASIO
	const std::string portString(StringUtils::toString(port));
	tcp::resolver resolver(getAsioContext());
	auto it = resolver.resolve(host, portString);
	tcp::resolver::iterator end;
	if(it != end) {
		ip.data->host = it->endpoint().address().to_v4();
		ip.data->port = it->endpoint().port();
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
		return IPv4Address();
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
