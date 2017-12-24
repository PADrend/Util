#
# This file is part of the Util library.
# Copyright (C) 2009-2013 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
target_sources(Util PRIVATE
	Network/ClockSynchronizer.cpp
	Network/DataConnection.cpp
	Network/Network.cpp
	Network/NetworkTCP.cpp
	Network/NetworkUDP.cpp
)
# Install the header files
install(FILES
	ClockSynchronizer.h
	DataBroadcaster.h
	DataConnection.h
	Network.h
	NetworkTCP.h
	NetworkUDP.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Network
	COMPONENT headers
)
