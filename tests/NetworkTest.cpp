/*
	This file is part of the Util library.
	Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch.hpp>
#include "Network/DataConnection.h"
#include "Network/Network.h"
#include "Network/NetworkTCP.h"
#include "Network/NetworkUDP.h"
#include "StringIdentifier.h"
#include "Utils.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <random>
#include <vector>
#include <iostream>

using namespace Util::Network;

TEST_CASE("NetworkTest_testTCP", "[NetworkTest]") {
	uint_fast8_t tryCount;

	tryCount = 0;
	std::unique_ptr<TCPServer> tcpServer(TCPServer::create(45000));
	REQUIRE(tcpServer.get() != nullptr);
	while (!tcpServer->isOpen() && tryCount < 10) {
		Util::Utils::sleep(1);
		++tryCount;
	}
	REQUIRE(tcpServer->isOpen());

	tryCount = 0;
	Util::Reference<TCPConnection> tcpClient;
	const IPv4Address address = IPv4Address::resolveHost("127.0.0.1", 45000);
	while (tcpClient.get() == nullptr && tryCount < 10) {
		Util::Utils::sleep(1);
		tcpClient = TCPConnection::connect(address);
		++tryCount;
	}
	REQUIRE(tcpClient->isOpen());

	tryCount = 0;
	Util::Reference<TCPConnection> tcpServerClientConnection;
	while (tcpServerClientConnection.get() == nullptr && tryCount < 10) {
		Util::Utils::sleep(1);
		tcpServerClientConnection = tcpServer->getIncomingConnection();
		++tryCount;
	}
	REQUIRE(tcpServerClientConnection.get() != nullptr);

	std::default_random_engine engine;
	std::uniform_int_distribution<uint8_t> distribution(0, 255);

	const size_t sizes[6] = { 128, 256, 512, 1024, 2048, 4096 };
	for (uint_fast8_t s = 0; s < 6; ++s) {
		std::vector<uint8_t> original(sizes[s]);
		uint8_t * ptr = original.data();

		for (uint_fast8_t run = 0; run < 10; ++run) {
			for (uint_fast32_t i = 0; i < sizes[s]; ++i) {
				ptr[i] = distribution(engine);
			}
			tcpClient->sendData(original);

			tryCount = 0;
			while (tcpServerClientConnection->getAvailableDataSize() < sizes[s] && tryCount < 10) {
				Util::Utils::sleep(5);
				++tryCount;
			}
			REQUIRE(tryCount < 10);

			const std::vector<uint8_t> received(tcpServerClientConnection->receiveData(sizes[s]));
			REQUIRE(!received.empty());
			REQUIRE(received == original);
		}
	}
}

TEST_CASE("NetworkTest_testUDP", "[NetworkTest]") {
	// Make sure the socket receive size is large enough.
	UDPNetworkSocket udpSocketServer(45000, 10240);
	udpSocketServer.open();

	UDPNetworkSocket udpSocketClient;
	udpSocketClient.open();
	const IPv4Address toAddress = IPv4Address::resolveHost("127.0.0.1", 45000);

	std::default_random_engine engine;
	std::uniform_int_distribution<uint8_t> distribution(0, 255);

	const size_t sizes[7] = { 128, 256, 512, 1024, 2048, 4096, 8192 };
	for (uint_fast8_t s = 0; s < 7; ++s) {
		std::vector<uint8_t> original(sizes[s]);
		uint8_t * ptr = original.data();

		for (uint_fast8_t run = 0; run < 10; ++run) {
			for (uint_fast32_t i = 0; i < sizes[s]; ++i) {
				ptr[i] = distribution(engine);
			}
			udpSocketClient.sendData(original.data(), original.size(), toAddress);

			std::unique_ptr<UDPNetworkSocket::Packet> packet(udpSocketServer.receive());
			REQUIRE(packet.get() != nullptr);
			REQUIRE(packet->packetData == original);
		}
	}
}

TEST_CASE("NetworkTest_testUDPgetPort", "[NetworkTest]") {
	// Make sure the socket receive size is large enough.
	UDPNetworkSocket udpSocketServer(0, 10240);
	udpSocketServer.open();

	UDPNetworkSocket udpSocketClient;
	udpSocketClient.open();
	const IPv4Address toAddress = IPv4Address::resolveHost("127.0.0.1", udpSocketServer.getPort());

	std::default_random_engine engine;
	std::uniform_int_distribution<uint8_t> distribution(0, 255);

	const size_t sizes[7] = { 128, 256, 512, 1024, 2048, 4096, 8192 };
	for (uint_fast8_t s = 0; s < 7; ++s) {
		std::vector<uint8_t> original(sizes[s]);
		uint8_t * ptr = original.data();

		for (uint_fast8_t run = 0; run < 10; ++run) {
			for (uint_fast32_t i = 0; i < sizes[s]; ++i) {
				ptr[i] = distribution(engine);
			}
			udpSocketClient.sendData(original.data(), original.size(), toAddress);

			std::unique_ptr<UDPNetworkSocket::Packet> packet(udpSocketServer.receive());
			REQUIRE(packet.get() != nullptr);
			REQUIRE(packet->packetData == original);
		}
	}
}

TEST_CASE("NetworkTest_testDataConnection", "[NetworkTest]") {
	const uint8_t maxTries = 20;
	const uint16_t numChannels = 10;
	uint_fast8_t tryCount;
	
	tryCount = 0;
	std::unique_ptr<TCPServer> tcpServer(TCPServer::create(45002));
	REQUIRE(tcpServer.get() != nullptr);
	while (!tcpServer->isOpen() && tryCount < maxTries) {
		Util::Utils::sleep(1);
		++tryCount;
	}
	REQUIRE(tcpServer->isOpen());

	tryCount = 0;
	Util::Reference<TCPConnection> tcpClient;
	const IPv4Address address = IPv4Address::resolveHost("127.0.0.1", 45002);
	while (tcpClient.get() == nullptr && tryCount < maxTries) {
		Util::Utils::sleep(1);
		tcpClient = TCPConnection::connect(address);
		++tryCount;
	}
	REQUIRE(tcpClient->isOpen());

	tryCount = 0;
	Util::Reference<TCPConnection> tcpServerClientConnection;
	while (tcpServerClientConnection.get() == nullptr && tryCount < maxTries) {
		Util::Utils::sleep(1);
		tcpServerClientConnection = tcpServer->getIncomingConnection();
		++tryCount;
	}
	REQUIRE(tcpServerClientConnection.get() != nullptr);

	DataConnection clientDataConnection(tcpClient.get());
	REQUIRE(clientDataConnection.isOpen());
	DataConnection serverDataConnection(tcpServerClientConnection.get());
	REQUIRE(serverDataConnection.isOpen());

	std::default_random_engine engine;
	std::uniform_int_distribution<uint8_t> dataDistribution(0, 255);
	std::uniform_int_distribution<uint16_t> keyDistribution;

	const size_t sizes[7] = { 128, 256, 512, 1024, 2048, 4096, 8192 };

	// Send values from client to server
	SECTION("Send values from client to server") {
		for (const auto & size : sizes) {
			std::vector<uint8_t> original(size);

			for (uint16_t channel = 0; channel < numChannels; ++channel) {
				for (uint_fast32_t i = 0; i < size; ++i) {
					original[i] = dataDistribution(engine);
				}
				clientDataConnection.sendValue(channel, original);

				uint16_t receivedChannel;
				std::vector<uint8_t> receivedData;

				tryCount = 0;
				while (!serverDataConnection.extractIncomingValue(receivedChannel, receivedData) && tryCount < maxTries) {
					Util::Utils::sleep(5);
					++tryCount;
				}
				REQUIRE(tryCount < maxTries);

				REQUIRE(channel == receivedChannel);
				REQUIRE(original == receivedData);
			}
		}
	}
	
	// Send values from server to client
	SECTION("Send values from server to client") {
		for (const auto & size : sizes) {
			std::vector<uint8_t> original(size);

			for (uint16_t channel = 0; channel < numChannels; ++channel) {
				for (uint_fast32_t i = 0; i < size; ++i) {
					original[i] = dataDistribution(engine);
				}
				serverDataConnection.sendValue(channel, original);

				uint16_t receivedChannel;
				std::vector<uint8_t> receivedData;

				tryCount = 0;
				while (!clientDataConnection.extractIncomingValue(receivedChannel, receivedData) && tryCount < maxTries) {
					Util::Utils::sleep(5);
					++tryCount;
				}
				REQUIRE(tryCount < maxTries);

				REQUIRE(channel == receivedChannel);
				REQUIRE(original == receivedData);
			}
		}
	}
	
	// Send (key, value) pairs from client to server
	SECTION("Send (key, value) pairs from client to server") {
		for (const auto & size : sizes) {
			std::vector<uint8_t> original(size);

			for (uint16_t channel = 0; channel < numChannels; ++channel) {
				for (uint_fast32_t i = 0; i < size; ++i) {
					original[i] = dataDistribution(engine);
				}
				const Util::StringIdentifier key(keyDistribution(engine));
				clientDataConnection.sendKeyValue(channel, key, original);

				uint16_t receivedChannel;
				Util::StringIdentifier receivedKey;
				std::vector<uint8_t> receivedData;

				tryCount = 0;
				while (!serverDataConnection.extractIncomingKeyValuePair(receivedChannel, receivedKey, receivedData) && tryCount < maxTries) {
					Util::Utils::sleep(5);
					++tryCount;
				}
				REQUIRE(tryCount < maxTries);

				REQUIRE(channel == receivedChannel);
				REQUIRE(key.toString() == receivedKey.toString());
				REQUIRE(original == receivedData);
			}
		}
	}

	// Send (key, value) pairs from server to client
	SECTION("Send (key, value) pairs from client to server") {
		for (const auto & size : sizes) {
			std::vector<uint8_t> original(size);

			for (uint16_t channel = 0; channel < numChannels; ++channel) {
				for (uint_fast32_t i = 0; i < size; ++i) {
					original[i] = dataDistribution(engine);
				}
				const Util::StringIdentifier key(keyDistribution(engine));
				serverDataConnection.sendKeyValue(channel, key, original);

				uint16_t receivedChannel;
				Util::StringIdentifier receivedKey;
				std::vector<uint8_t> receivedData;

				tryCount = 0;
				while (!clientDataConnection.extractIncomingKeyValuePair(receivedChannel, receivedKey, receivedData) && tryCount < maxTries) {
					Util::Utils::sleep(5);
					++tryCount;
				}
				REQUIRE(tryCount < maxTries);

				REQUIRE(channel == receivedChannel);
				REQUIRE(key.toString() == receivedKey.toString());
				REQUIRE(original == receivedData);
			}
		}
	}

	// Check channel handlers on server side	
	SECTION("Check channel handlers on server side") {
		class HandlerChecker {
			private:
				const DataConnection::channelId_t checkChannelId;
				const Util::StringIdentifier checkKey;
				const DataConnection::dataPacket_t checkData;
				const uint32_t expectedNumberOfValueChecks;
				uint32_t maximumNumberOfKeyValueChecks;
				uint32_t actualNumberOfValueChecks;
				uint32_t actualNumberOfKeyValueChecks;

			public:
				HandlerChecker(DataConnection::channelId_t channelId, 
							   const Util::StringIdentifier & key, 
							   const DataConnection::dataPacket_t & data, 
							   uint32_t checks) :
					checkChannelId(channelId), 
					checkKey(key), 
					checkData(data), 
					expectedNumberOfValueChecks(checks),
					maximumNumberOfKeyValueChecks(0),
					actualNumberOfValueChecks(0),
					actualNumberOfKeyValueChecks(0) {
				}
				~HandlerChecker() {
					REQUIRE(expectedNumberOfValueChecks == actualNumberOfValueChecks);
					// Multiple messages with the same key must only arrive once per receive call.
					REQUIRE(maximumNumberOfKeyValueChecks >= actualNumberOfKeyValueChecks);
				}

				void checkValue(DataConnection::channelId_t channelId, const DataConnection::dataPacket_t & data) {
					REQUIRE(channelId == checkChannelId);
					REQUIRE(checkData == data);
					++actualNumberOfValueChecks;
				}

				void checkKeyValue(DataConnection::channelId_t channelId, const Util::StringIdentifier & key, const DataConnection::dataPacket_t & data) {
					REQUIRE(channelId == checkChannelId);
					REQUIRE(checkKey.toString() == key.toString());
					REQUIRE(checkData == data);
					++actualNumberOfKeyValueChecks;
				}

				void allowAdditionalKeyValueCheck() {
					++maximumNumberOfKeyValueChecks;
				}

				bool finished() const {
					return actualNumberOfValueChecks >= expectedNumberOfValueChecks;
				}
		};
		for (const auto & size : sizes) {
			std::vector<uint8_t> original(size);

			for (uint16_t channel = 0; channel < numChannels; ++channel) {
				for (uint_fast32_t i = 0; i < size; ++i) {
					original[i] = dataDistribution(engine);
				}
				const uint32_t numChecks = 10;
				const Util::StringIdentifier key(keyDistribution(engine));
				HandlerChecker checker(channel, key, original, numChecks);
				serverDataConnection.registerValueChannelHandler(channel, 
						std::bind(&HandlerChecker::checkValue, &checker, std::placeholders::_1, std::placeholders::_2));
				serverDataConnection.registerKeyValueChannelHandler(channel, 
						std::bind(&HandlerChecker::checkKeyValue, &checker, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

				for(uint_fast32_t count = 0; count < numChecks; ++count) {
					clientDataConnection.sendValue(channel, original);
					clientDataConnection.sendKeyValue(channel, key, original);
				}
				tryCount = 0;
				while (!checker.finished() && tryCount < maxTries) {
					serverDataConnection.handleIncomingData();
					// Keys are only unique during one receive call.
					checker.allowAdditionalKeyValueCheck();
					Util::Utils::sleep(5);
					++tryCount;
				}
				REQUIRE(tryCount < maxTries);

				serverDataConnection.removeKeyValueChannelHandler(channel);
			}
		}
	}

	clientDataConnection.close();
	REQUIRE(!clientDataConnection.isOpen());
	serverDataConnection.close();
	REQUIRE(!serverDataConnection.isOpen());
}

TEST_CASE("NetworkTest_testTCPClientExit", "[NetworkTest]") {
	uint_fast8_t tryCount;

	tryCount = 0;
	std::unique_ptr<TCPServer> tcpServer(TCPServer::create(45001));
	REQUIRE(tcpServer.get() != nullptr);
	while (!tcpServer->isOpen() && tryCount < 10) {
		Util::Utils::sleep(1);
		++tryCount;
	}
	REQUIRE(tcpServer->isOpen());

	tryCount = 0;
	Util::Reference<TCPConnection> tcpClient;
	const IPv4Address address = IPv4Address::resolveHost("127.0.0.1", 45001);
	while (tcpClient.get() == nullptr && tryCount < 10) {
		Util::Utils::sleep(1);
		tcpClient = TCPConnection::connect(address);
		++tryCount;
	}
	REQUIRE(tcpClient->isOpen());

	tryCount = 0;
	Util::Reference<TCPConnection> tcpServerClientConnection;
	while (tcpServerClientConnection.get() == nullptr && tryCount < 10) {
		Util::Utils::sleep(1);
		tcpServerClientConnection = tcpServer->getIncomingConnection();
		++tryCount;
	}
	REQUIRE(tcpServerClientConnection.get() != nullptr);

	std::default_random_engine engine;
	std::uniform_int_distribution<uint8_t> distribution(0, 255);

	const size_t size = 8192;
	std::vector<uint8_t> original(size);
	uint8_t * ptr = original.data();
	for (uint_fast32_t i = 0; i < size; ++i) {
		ptr[i] = distribution(engine);
	}

	// Initially send some data.
	for (uint_fast8_t run = 0; run < 10; ++run) {
		tcpServerClientConnection->sendData(original);
	}

	for (uint_fast8_t run = 0; run < 100; ++run) {
		// Send even more data.
		tcpServerClientConnection->sendData(original);

		if (run < 50) {
			tryCount = 0;
			while (tcpClient->getAvailableDataSize() < size && tryCount < 10) {
				Util::Utils::sleep(30);
				++tryCount;
			}
			REQUIRE(tryCount < 10);
			// Receive smaller packets than the ones that were sent.
			std::vector<uint8_t> receivedA(tcpClient->receiveData(size / 4));
			const std::vector<uint8_t> receivedB(tcpClient->receiveData(size / 4));
			const std::vector<uint8_t> receivedC(tcpClient->receiveData(size / 4));
			const std::vector<uint8_t> receivedD(tcpClient->receiveData(size / 4));
			REQUIRE(!receivedA.empty());
			REQUIRE(!receivedB.empty());
			REQUIRE(!receivedC.empty());
			REQUIRE(!receivedD.empty());

			receivedA.insert(receivedA.end(), receivedB.begin(), receivedB.end());
			receivedA.insert(receivedA.end(), receivedC.begin(), receivedC.end());
			receivedA.insert(receivedA.end(), receivedD.begin(), receivedD.end());
			REQUIRE(receivedA == original);
		} else {
			// Kill the client. The server still sends more data.
			tcpClient = nullptr;
		}
	}
}
