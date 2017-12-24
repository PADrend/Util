#
# This file is part of the Util library.
# Copyright (C) 2009-2013 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
target_sources(Util PRIVATE
	IO/AbstractFSProvider.cpp
	IO/ArchiveProvider.cpp
	IO/DBFSProvider.cpp
	IO/FileLocator.cpp
	IO/FileName.cpp
	IO/FileUtils.cpp
	IO/FSProvider.cpp
	IO/NetProvider.cpp
	IO/SerialProvider.cpp
	IO/TemporaryDirectory.cpp
	IO/ZIPProvider.cpp
)
# Install the header files
install(FILES
	AbstractFSProvider.h
	ArchiveProvider.h
	DBFSProvider.h
	FileLocator.h
	FileName.h
	FileUtils.h
	FSProvider.h
	NetProvider.h
	SerialProvider.h
	TemporaryDirectory.h
	ZIPProvider.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/IO
	COMPONENT headers
)
